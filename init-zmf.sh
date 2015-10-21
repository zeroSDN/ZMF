echo "### Installing ZSDN environment ###"
echo ""
echo "I will try to get it work"
echo "THIS MAY TAKE SOME TIME"

echo ""

ZDIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
echo "Installing to directory: $ZDIR"

echo ""
echo "Press enter to continue (or Ctrl+C to cancel)"

read

cd $ZDIR

echo $ZDIR > ./zmf-dir.txt
echo "set(ZSDN_DIR $ZDIR)" > ./zmf-dir_cmake.txt

cd util
if ./init_dependencies.sh; then
	echo "# init_dependencies success"
else
	result=$?
	echo "!! Failed to init_dependencies"
	exit ${result}
fi
cd ..


if ./build-zmf.sh; then
	echo "# Build ALL success"
else
	result=$?
	echo "!! Failed to build ALL"
	exit ${result}
fi

echo "### Finished Its-Not-Working Script ###"