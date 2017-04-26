#/bin/sh

# fetch environment variables
source ../../build/SetEnv

CXX=/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++

# compile
$CXX -I$EOSLIB_INCLUDE -c Test.C  \
    || { echo "compile failed"; exit 1; }

# load
$CXX -o Test Test.o \
    -L$EOSLIB_SHARED_LIBRARY_PATH \
    -lExtEOS

# Run
./Test
