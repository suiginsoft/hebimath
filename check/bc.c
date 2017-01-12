/*
 * hebimath - arbitrary precision arithmetic library
 * See LICENSE file for copyright and license details
 */

#include "check.h"
#include <limits.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <fcntl.h>
#include <unistd.h>

static int bcrunning;
static FILE *bcpipe[2];
static char *bcopbuf;
static char *actualbuf;
static size_t bcopbufsz;
static size_t actualbufsz;

static void
bcterm(void)
{
	if (!bcrunning)
		return;
	fclose(bcpipe[0]);
	fclose(bcpipe[1]);
	free(bcopbuf);
	free(actualbuf);
	bcopbuf = NULL;
	bcopbufsz = 0;
	actualbuf = NULL;
	actualbufsz = 0;
	bcrunning = 0;
}

static void
bcinit(void)
{
	char *argv[2] = { "bc", NULL };
	int rdpipefd[2], wrpipefd[2];
	pid_t pid;

	assert(!bcrunning);

	/* open bidirectional pipes to communicate with bc(1) */
	if (pipe(rdpipefd) < 0) {
		perror("unable to open pipe");
		exit(EXIT_FAILURE);
	}

	if (pipe(wrpipefd) < 0) {
		perror("unable to open pipe");
		exit(EXIT_FAILURE);
	}

	/* fork and exec bc(1) */
	if ((pid = fork()) < 0) {
		perror("unable to fork to execute bc");
		exit(EXIT_FAILURE);
	} else if (!pid) {
		if (dup2(wrpipefd[0], STDIN_FILENO) < 0 ||
				dup2(rdpipefd[1], STDOUT_FILENO) < 0) {
			perror("unable to duplicate file descriptors for bc");
			_Exit(EXIT_FAILURE);
		}
		close(rdpipefd[0]);
		close(rdpipefd[1]);
		close(wrpipefd[0]);
		close(wrpipefd[1]);
		if (execvp(argv[0], argv) < 0)
			perror("unable to execute bc");
		_Exit(EXIT_FAILURE);
	}

	/* reopen pipe using C FILEs */
	if (!(bcpipe[0] = fdopen(rdpipefd[0], "r")) ||
			!(bcpipe[1] = fdopen(wrpipefd[1], "w"))) {
		perror("unable to establish streams for pipe to bc");
		exit(EXIT_FAILURE);
	}
	close(rdpipefd[1]);
	close(wrpipefd[0]);

	/* register termination handler */
	bcrunning = 1;
	atexit(bcterm);
}

char *
bcreadln(void)
{
	char *s = NULL;
	size_t n, i = 0, slen = 0;
	char *line = NULL;
	size_t linersv = 0;
	ssize_t linelen;
	int incomplete;

	if (!bcrunning)
		bcinit();

	/* flush pending writes */
	if (fflush(bcpipe[1]) == EOF) {
		perror("unable to flush stream to bc");
		exit(EXIT_FAILURE);
	}

	/* read in and combine sequence of lines escaped by backslash */
	do {
		if ((linelen = getline(&line, &linersv, bcpipe[0])) < 0) {
			perror("unable to receive result from bc");
			exit(EXIT_FAILURE);
		}
		incomplete = 0;
		n = (size_t)linelen;
		if (n > 0 && line[n-1] == '\n')
			n--;
		if (n > 0 && line[n-1] == '\\') {
			incomplete = 1;
			n--;
		}
		if (i+n >= slen) {
			slen = MAX(256, 2 * slen);
			if (!(s = realloc(s, slen))) {
				perror("unable to realloc bc result buffer");
				exit(EXIT_FAILURE);
			}
		}
		memcpy(s+i, line, n);
		i += n;
	} while (incomplete);

	assert(s);
	free(line);
	s[i] = '\0';
	return s;
}

void
bcwrite(const char *str)
{
	size_t n;

	assert(str);

	if (!bcrunning)
		bcinit();

	n = strlen(str);
	if (n && fwrite(str, 1, n, bcpipe[1]) != n) {
		perror("unable to send operation to bc");
		exit(EXIT_FAILURE);
	}

	if ((!n || str[n-1] != '\n') && fputc('\n', bcpipe[1]) == EOF) {
		perror("unable to send operation to bc");
		exit(EXIT_FAILURE);
	}
}

char *
bcputs(const char *str)
{
	bcwrite(str);
	return bcreadln();
}

char *
vbcprintf(const char *format, va_list ap)
{
	va_list ap2;
	char* p;
	int c;

	assert(format);

	if (!bcrunning)
		bcinit();

	va_copy(ap2, ap);
	c = vfprintf(bcpipe[1], format, ap2);
	va_end(ap2);

	if (c < 0) {
		perror("unable to send operation to bc");
		exit(EXIT_FAILURE);
	}

	if ((!(p = strrchr(format, '\n')) || p[1] != '\0') &&
			fputc('\n', bcpipe[1]) == EOF) {
		perror("unable to send operation to bc");
		exit(EXIT_FAILURE);
	}

	return bcreadln();
}

char *
bcprintf(const char *format, ...)
{
	va_list ap;
	char *r;

	va_start(ap, format);
	r = vbcprintf(format, ap);
	va_end(ap);
	return r;
}

static void
checkresults(const char *actual, const char *expected, const char *operation)
{
	assert(actual);
	assert(expected);
	assert(operation);

	if (strcmp(actual, expected)) {
		(void)fprintf(stderr,
			"ASSERTION FAILURE\nactual result does not match expected result\n"
			"iteration: %ld pass: %ld\n"
			"actual:\n%s\n"
			"expected:\n%s\n"
			"operation:\n%s\n",
			check_iter, check_pass, actual, expected, operation);
		if (!check_skip_error) {
			(void)fflush(stderr);
			abort();
		}
	} else if (check_verbose) {
		(void)printf(
			"iteration: %ld pass: %ld\n"
			"actual:\n%s\n"
			"expected:\n%s\n"
			"operation:\n%s\n\n",
			check_iter, check_pass, actual, expected, operation);
	}
}

static void
vscheckbc(const char *actual, const char *bcopfmt, va_list ap)
{
	va_list ap2;
	char *expected = NULL;
	size_t n = 4096;
	int r;
	
	assert(actual);
	assert(bcopfmt);

	do {
		if (n >= bcopbufsz) {
			bcopbufsz = MAX(n + 1, bcopbufsz * 2);
			bcopbuf = realloc(bcopbuf, bcopbufsz);
		}
		va_copy(ap2, ap);
		r = vsnchkprintf(bcopbuf, bcopbufsz, bcopfmt, ap2);
		va_end(ap2);
		assert(r >= 0);
		n = (size_t)r;
	} while (n >= bcopbufsz);

	expected = bcputs(bcopbuf);
	checkresults(actual, expected, bcopbuf);
	free(expected);
}

void
zcheckbc(hebi_zsrcptr restrict actual, const char *bcopfmt, ...)
{
	size_t n = 1024;
	va_list ap;

	assert(actual);
	assert(bcopfmt);

	do {
		if (n >= actualbufsz) {
			actualbufsz = MAX(n + 1, actualbufsz * 2);
			actualbuf = realloc(actualbuf, actualbufsz);
		}
		n = hebi_zgetstr(actualbuf, actualbufsz, actual, 10, 0);
	} while (n >= actualbufsz);

	va_start(ap, bcopfmt);
	vscheckbc(actualbuf, bcopfmt, ap);
	va_end(ap);
}

void
zcheckstr(hebi_zsrcptr restrict actual, const char *expected, const char* operation)
{
	size_t n = 1024;

	assert(actual);
	assert(expected);
	assert(operation);

	do {
		if (n >= actualbufsz) {
			actualbufsz = MAX(n + 1, actualbufsz * 2);
			actualbuf = realloc(actualbuf, actualbufsz);
		}
		n = hebi_zgetstr(actualbuf, actualbufsz, actual, 10, 0);
	} while (n >= actualbufsz);

	checkresults(actualbuf, expected, operation);
}
