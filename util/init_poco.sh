echo "POCO Dependency Init SCRIPT"

cd ..
ZDIR=$(<zmf-dir.txt)
echo "ZMF-Dir: $ZDIR"

cd ~/Downloads


wget -O poco.tar.gz http://pocoproject.org/releases/poco-1.6.1/poco-1.6.1.tar.gz

rm -rf poco-1.6.1

tar -xzf poco.tar.gz

cd poco-1.6.1
if [ "$1" == "-pi" ]; then
	cp $ZDIR/util/PI_POCO_CONFIG ./build/config
	./configure --config=PI_POCO_CONFIG --no-tests --no-samples --prefix=$ZDIR/dependencies
else
	./configure --no-tests --no-samples --prefix=$ZDIR/dependencies
fi
make -j4
make install
cd ..

rm -rf poco-1.6.1

rm poco.tar.gz

echo "Fininshed, please invalidate caches of your IDE if necesary"