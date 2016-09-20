test -f Makefile && make clean

rm -f *.h *.cc *.cpp Makefile *.conf

../../phxrpc/codegen/phxrpc_pb2server -I ../../phxrpc -I ../../third_party/protobuf/include -f cert.proto -d .


