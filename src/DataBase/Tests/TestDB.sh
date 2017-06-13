#/bin/sh

# fetch environment variables
source ../../../SetEnv

CXX="${CMAKE_CXX_COMPILER} -std=c++11"

# compile
$CXX -I$EOSLIB_INCLUDE -c TestDB.C  \
    || { echo "compile failed"; exit 1; }

# load
$CXX -o TestDB TestDB.o \
    -L$EOSLIB_SHARED_LIBRARY_PATH \
    -lDataBase

# Run
./TestDB
