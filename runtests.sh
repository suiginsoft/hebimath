#!/bin/sh

LD_LIBRARY_PATH=$(pwd)

printf "running %s tests\n" "$1"
printf "=============================================\n"

shift
while [ $# -gt 0 ]; do
	printf "%s\n" "$1" && $1;
	shift
done

printf "=============================================\n"
printf "all done\n"
