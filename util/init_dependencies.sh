if [ "$1" == "-pi" ]; then
	echo "init deps for pi"
	bash ../util/init_poco.sh -pi
	bash ../util/init_protobuf.sh -pi
	bash ../util/init_zmq.sh -pi
else
	echo "init deps for x64"
	bash ../util/init_poco.sh
	bash ../util/init_protobuf.sh
	bash ../util/init_zmq.sh
fi