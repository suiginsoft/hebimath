include config.mk

CC = gcc
CFLAGS = -O0 -fprofile-arcs -ftest-coverage \
	-std=c99 -pedantic -pthread \
	-Wall -Wextra -Waggregate-return -Wconversion -Wshadow
