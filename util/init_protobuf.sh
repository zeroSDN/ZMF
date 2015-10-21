echo "PROTOBUF Dependency Init SCRIPT"

cd ..
ZDIR=$(<zmf-dir.txt)
echo "ZMF-Dir: $ZDIR"

cd ~/Downloads

wget -O protobuf.tar.gz https://github.com/google/protobuf/releases/download/v2.6.1/protobuf-2.6.1.tar.gz

rm -rf protobuf-2.6.1

tar -xzf protobuf.tar.gz

cd protobuf-2.6.1
if [ "$1" == "-pi" ]; then
	./configure CC=arm-linux-gnueabihf-gcc CXX=arm-linux-gnueabihf-g++ --with-protoc=protoc --with-sysroot=/home/raspberrypi/rootfs --host=arm-linux-gnueabihf --prefix=$ZDIR/dependencies
else
	./configure --prefix=$ZDIR/dependencies
fi
make -j4
make install
cd ..

rm -rf protobuf-2.6.1

rm protobuf.tar.gz

echo "Fininshed, please invalidate caches of your IDE if necesary"