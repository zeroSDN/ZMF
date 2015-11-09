#!/bin/bash

# Script for initializing this ZMF directory

echo "### Installing ZMF environment ###"
echo ""
echo "Initializing zhe ZMF environment"
echo "THIS MAY TAKE SOME TIME"
echo 

ZDIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
echo "Installing to directory: $ZDIR"

echo ""
echo "Press enter to continue or wait 10s (or Ctrl+C to cancel)"
read -t 10


cd $ZDIR


echo $ZDIR > ./util/zmf-dir.txt
echo "set(ZMF_DIR $ZDIR)" > ./util/zmf-dir_cmake.txt


cd util
echo
echo "## Start init ZMF dependencies"
if ./init_dependencies.sh; then
	echo "# Init ZMF dependencies success"
else
	result=$?
	echo "!! Failed to init ZMF dependencies"
	exit ${result}
fi
cd ..


echo
echo "## Start build ZMF"
if ./build-zmf.sh; then
	echo "# Build ZMF success"
else
	result=$?
	echo "!! Failed to build ZMF"
	exit ${result}
fi

echo "### Finished Installing ZMF environment ###"