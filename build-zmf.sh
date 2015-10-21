
echo "### Start Build ZMF ###"


# Parse Parameters
SkiptTests=true
BuildTarget="default"

while getopts hb:tc flag; do
  case $flag in
    h)
      echo "--- HELP ---:";
      echo "<-t> to execute unittests during build";
      echo "<-b pi> to build for Raspberry Pi ARM";
      exit 0;
      ;;
    b)
      BuildTarget=$OPTARG;
      ;;
    t)
      SkiptTests=$false;
      ;;
    c)
      echo "Clear parameter redundant"
      ;;
    ?)
	  echo "!!Unknown parameter: $flag"	
      ;;
  esac
done


# Construct BuildArgs
BuildArgs=""

if [ "$SkiptTests" = true ] ; then
	echo "Build skipping Tests"
	BuildArgs=$BuildArgs" -DNoTests=ON"
else
	echo "Build with Tests"
fi

if [ "$BuildTarget" = "pi" ] ; then
	echo "Building for RasPi ARM target"
	BuildArgs=$BuildArgs" -DCMAKE_TOOLCHAIN_FILE=$HOME/raspberrypi/pi.cmake"
elif [ "$BuildTarget" = "default" ] ; then
	echo "Build with default target"
else 
	echo "!!Unknown build target"	
	exit 1
fi


cd src


# Clear ZMF
find . -name CMakeCache.txt -delete
find . -name Makefile -delete
find . -name cmake_install.cmake -delete
find . -name CMakeFiles -type d -exec rm -rf {} +
echo "# Cleard ZMF"


# Build ZMF
cmake$BuildArgs .
if cmake --build .; then
	echo "# Cmake ZMF success"
else
	result=$?
	echo "!! Failed to Cmake ZMF: "${result}
	exit ${result}
fi
cd ..

echo "### Finished Build ZMF ###"