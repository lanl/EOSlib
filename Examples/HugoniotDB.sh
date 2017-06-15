#/bin/sh

# fetch environment variables
source ../../build/SetEnv

CXX="${CMAKE_CXX_COMPILER} -std=c++11"

# compile
$CXX -I$EOSLIB_INCLUDE -c HugoniotDB.C  \
    || { echo "compile failed"; exit 1; }

# load
$CXX -o HugoniotDB HugoniotDB.o \
    -L$EOSLIB_SHARED_LIBRARY_PATH \
    -lEOS \
    -Wl,-rpath,$EOSLIB_SHARED_LIBRARY_PATH
#
# Note: if environment variable is set
#   export LD_LIBRARY_PATH=$EOSLIB_SHARED_LIBRARY_PATH:$LD_LIBRARY_PATH
# then loader argument
#   -Wl,-rpath,$EOSLIB_SHARED_LIBRARY_PATH
# is not needed
#

# Run
./HugoniotDB
