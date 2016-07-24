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
static char *hresbuf;
static size_t bcopbufsz;
static size_t hresbufsz;

static void
bcterm(void)
{
	if (!bcrunning)
		return;
	fclose(bcpipe[0]);
	fclose(bcpipe[1]);
	free(bcopbuf);
	free(hresbuf);
	bcopbuf = NULL;
	bcopbufsz = 0;
	hresbuf = NULL;
	hresbufsz = 0;
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

static
int vsnzprintf(
		char *restrict s,
		size_t n,
		const char *restrict format,
		va_list ap )
{
	size_t f = 0, i = 0;
	int c, j, k, ll;

	for (;;) {
		c = format[f++];
		if (c == '\0') {
			break;
		} else if (c != '%') {
			if (i < n)
				s[i] = (char)c;
			++i;
			continue;
		}
		k = n > i ? n-i : 0;
		j = ll = 0;
	restart:
		c = format[f++];
		switch (c) {
		case 'l':
			if (++ll > 2)
				assert(!"bad format string");
			goto restart;
		case 'i':
		case 'd':
			if (ll == 0)
				j = snprintf(s+i, k, "%d", va_arg(ap, int));
			else if (ll == 1)
				j = snprintf(s+i, k, "%ld", va_arg(ap, long));
			else
				j = snprintf(s+i, k, "%lld", va_arg(ap, long long));
			break;
		case 'u':
			if (ll == 0)
				j = snprintf(s+i, k, "%u", va_arg(ap, unsigned int));
			else if (ll == 1)
				j = snprintf(s+i, k, "%lu", va_arg(ap, unsigned long));
			else
				j = snprintf(s+i, k, "%llu", va_arg(ap, unsigned long long));
			break;
		case 's':
			j = snprintf(s+i, k, "%s", va_arg(ap, const char*));
			break;
		case 'Z':
			if (ll)
				assert(!"bad format string");
			j = (int)hebi_zgetstr(s+i, k, va_arg(ap, hebi_zsrcptr), 10);
			break;
		default:
			assert(!"bad format string");
			break;
		}
		assert(j >= 0);
		i += j;
	}

	if (i < n)
		s[i] = '\0';
	else if (n)
		s[n-1] = '\0';
	return i < INT_MAX ? i : INT_MAX;
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
	char* p;

	assert(format);

	if (!bcrunning)
		bcinit();

	if (vfprintf(bcpipe[1], format, ap) < 0) {
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
vscheckbc(const char *restrict hres, const char *restrict bcop, va_list ap)
{
	char *bcres = NULL;
	size_t n = 4096;
	int r;
	
	assert(hres && bcop);

	do {
		if (n >= bcopbufsz) {
			bcopbufsz = MAX(n + 1, bcopbufsz * 2);
			bcopbuf = realloc(bcopbuf, bcopbufsz);
		}
		r = vsnzprintf(bcopbuf, bcopbufsz, bcop, ap);
		assert(r >= 0);
		n = (size_t)r;
	} while (n >= bcopbufsz);

	bcres = bcputs(bcopbuf);
	assert(bcres);

	if (strcmp(hres, bcres)) {
		(void)fprintf(stderr,
			"hebi operation does not match bc operation\n"
			"iteration: %ld\n"
			"hebi:\n%s\n"
			"bc:\n%s\n"
			"operation:\n%s\n",
			check_iter, hres, bcres, bcopbuf);
		if (!check_skip_error) {
			(void)fflush(stderr);
			abort();
		}
	} else if (check_verbose) {
		(void)printf(
			"iteration:%ld pass: %ld\n"
			"hebi:\n%s\n"
			"bc:\n%s\n"
			"operation:\n%s\n\n",
			check_iter, check_pass, hres, bcres, bcopbuf);
	}

	free(bcres);
}

void
scheckbc(const char *restrict hres, const char *restrict bcop, ...)
{
	va_list ap;

	va_start(ap, bcop);
	vscheckbc(hres, bcop, ap);
	va_end(ap);
}

void
zcheckbc(hebi_zsrcptr restrict hres, const char *restrict bcop, ...)
{
	size_t n = 1024;
	va_list ap;

	do {
		if (n >= hresbufsz) {
			hresbufsz = MAX(n + 1, hresbufsz * 2);
			hresbuf = realloc(hresbuf, hresbufsz);
		}
		n = hebi_zgetstr(hresbuf, hresbufsz, hres, 10);
	} while (n >= hresbufsz);

	va_start(ap, bcop);
	vscheckbc(hresbuf, bcop, ap);
	va_end(ap);
}
