#!/bin/sh

LD_LIBRARY_PATH=`pwd`

echo "running $1 tests"
echo "==============================================="

shift
while [[ $# -gt 0 ]]; do
	echo "$1" && $1;
	shift
done

echo "==============================================="
echo "all done"
