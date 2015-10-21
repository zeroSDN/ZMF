echo "ZMQ Dependency Init SCRIPT"

cd ..
ZDIR=$(<zmf-dir.txt)
echo "ZMF-Dir: $ZDIR"

cd ~/Downloads

wget -O libsodium.tar.gz https://github.com/jedisct1/libsodium/releases/download/1.0.3/libsodium-1.0.3.tar.gz
wget -O zmq.tar.gz http://download.zeromq.org/zeromq-4.1.2.tar.gz
wget -O zmqpp.tar.gz https://github.com/zeromq/zmqpp/archive/4.1.2.tar.gz

rm -rf libsodium-1.0.3
rm -rf zmqpp-4.1.2
rm -rf zeromq-4.1.2

tar -xzf libsodium.tar.gz
tar -xzf zmq.tar.gz
tar -xzf zmqpp.tar.gz

cd libsodium-1.0.3
if [ "$1" == "-pi" ]; then
	./configure CC=arm-linux-gnueabihf-gcc CXX=arm-linux-gnueabihf-g++ --with-sysroot=$HOME/raspberrypi/rootfs --host=arm-linux-gnueabihf --prefix=$ZDIR/dependencies
else
	./configure --prefix=$ZDIR/dependencies
fi
make -j4
make install
cd ..

cd zeromq-4.1.2
export sodium_CFLAGS="-I$ZDIR/dependencies/include"
export sodium_LIBS="-L$ZDIR/dependencies/lib/"
export CPATH="$ZDIR/dependencies/include/"
export LIBRARY_PATH="$ZDIR/dependencies/lib/"
export LD_LIBRARY_PATH="$ZDIR/dependencies/lib/"
export LD_RUN_PATH="$ZDIR/dependencies/lib/"
export PKG_CONFIG_PATH="$ZDIR/dependencies/lib/pkgconfig"
export CFLAGS=$(pkg-config --cflags libsodium)
export LDFLAGS=$(pkg-config --libs libsodium)
if [ "$1" == "-pi" ]; then
	./configure CC=arm-linux-gnueabihf-gcc CXX=arm-linux-gnueabihf-g++ --with-sysroot=$HOME/raspberrypi/rootfs --host=arm-linux-gnueabihf --prefix=$ZDIR/dependencies
else
	./configure --prefix=$ZDIR/dependencies
fi
make -j4
make install
cd ..

cd zmqpp-4.1.2
if [ "$1" == "-pi" ]; then
	cmake -DCMAKE_INSTALL_PREFIX=$ZDIR/dependencies/ -DZEROMQ_LIB_DIR=$ZDIR/dependencies/lib/ -DZEROMQ_INCLUDE_DIR=$ZDIR/dependencies/include/ -DCMAKE_TOOLCHAIN_FILE=$HOME/raspberrypi/pi.cmake
	make -j4
	make install
else
	cmake -DCMAKE_INSTALL_PREFIX=$ZDIR/dependencies/ -DZEROMQ_LIB_DIR=$ZDIR/dependencies/lib/ -DZEROMQ_INCLUDE_DIR=$ZDIR/dependencies/include/
	make -j4
	make install
fi
cd ..

rm -rf libsodium-1.0.3
rm -rf zeromq-4.1.2
rm -rf zmqpp-4.1.2

rm libsodium.tar.gz
rm zmq.tar.gz
rm zmqpp.tar.gz

echo "Fininshed, please invalidate caches of your IDE if necesary"
