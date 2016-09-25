
set -e  # exit immediately on error
set -x  # display all commands

MINICHAT_ROOT=`pwd`

mkdir -p $MINICHAT_ROOT/sbin
mkdir -p $MINICHAT_ROOT/etc

if [ ! -f $MINICHAT_ROOT/sbin/redis-server ]; then
    cd $MINICHAT_ROOT/third_party

    if [ ! -f 3.2.3.tar.gz ]; then
        wget https://github.com/antirez/redis/archive/3.2.3.tar.gz
    fi

    tar zxvf 3.2.3.tar.gz
    cd redis-3.2.3

    make
    cp src/redis-server $MINICHAT_ROOT/sbin
    cp redis.conf $MINICHAT_ROOT/etc

    cd $MINICHAT_ROOT
fi

if [ ! -f $MINICHAT_ROOT/third_party/hiredis/libhiredis.a ]; then
    cd $MINICHAT_ROOT/third_party/

    if [ ! -f hiredis/Makefile ]; then
        git clone https://github.com/redis/hiredis.git
    fi

    cd hiredis

    make static

    cd $MINICHAT_ROOT
fi

if [ ! -f $MINICHAT_ROOT/third_party/r3c/libr3c.a ]; then
    cd $MINICHAT_ROOT/third_party

    if [ ! -f r3c/r3c.cpp ]; then
        git clone https://github.com/eyjian/r3c.git
    fi

    make
fi

if [ ! -f $MINICHAT_ROOT/third_party/protobuf/bin/protoc ]; then
    cd $MINICHAT_ROOT/third_party

    if [ ! -f protobuf-cpp-3.0.0.tar.gz ]; then
        wget https://github.com/google/protobuf/releases/download/v3.0.0/protobuf-cpp-3.0.0.tar.gz
    fi	

    tar zxvf protobuf-cpp-3.0.0.tar.gz
    cd protobuf-3.0.0

    ./configure --prefix=`pwd`/../protobuf
    make -j2
    make install

    cd $MINICHAT_ROOT
fi

if [ ! -f $MINICHAT_ROOT/phxrpc/lib/libphxrpc.a ]; then
    cd $MINICHAT_ROOT/

    if [ ! -f phxrpc/phxrpc.mk ]; then
        git clone https://github.com/liusifan/phxrpc.git
    fi

    cd phxrpc

    rm -rf third_party
    ln -s ../third_party .

    make debug=y

    cd $MINICHAT_ROOT
fi

if [ ! -f $MINICHAT_ROOT/third_party/yassl/lib/libyassl.a ]; then
    cd $MINICHAT_ROOT/third_party

    if [ ! -f yassl/autogen.sh ]; then
        git clone https://github.com/yassl/yassl.git
    fi

    cd yassl

    ./autogen.sh
    ./configure
    make

    cp src/.libs/libyassl.a lib
    cp taocrypt/src/.libs/libtaocrypt.a lib
fi

if [ ! -f $MINICHAT_ROOT/third_party/easyloggingpp/easylogging++.h ]; then
    cd $MINICHAT_ROOT/third_party

    if [ ! -f 9.84.tar.gz ]; then
        wget https://github.com/easylogging/easyloggingpp/archive/9.84.tar.gz
    fi

    tar zxvf 9.84.tar.gz

    mkdir -p easyloggingpp

    cp easyloggingpp-9.84/src/easylogging++.h  easyloggingpp
fi

if [ ! -f $MINICHAT_ROOT/third_party/easyloggingpp/easylogging++.h ]; then
    cd $MINICHAT_ROOT/third_party

    if [ ! -f 9.84.tar.gz ]; then
        wget https://github.com/easylogging/easyloggingpp/archive/9.84.tar.gz
    fi

    tar zxvf 9.84.tar.gz

    mkdir -p easyloggingpp

    cp easyloggingpp-9.84/src/easylogging++.h  easyloggingpp
fi

cd $MINICHAT_ROOT/minichat

make

