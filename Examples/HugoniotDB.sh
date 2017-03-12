#/bin/sh

# fetch environment variables
source ../../build/SetEnv

# with macports, default is to use g++ compiler
# while cmake uses Xcode c++ compiler
# different compilers can give load errors due to name mangling
#
# ToDo: cmake variable ${CMAKE_CXX_COMPILER}
#       or in build/CMakeCache.txt pickout
#          CMAKE_CXX_COMPILER:FILEPATH=
#       or fill in with cmake configure_file command
if [ `uname -s` = 'Linux' ]; then
    CXX=g++
else
    CXX=/usr/bin/c++
fi

# compile
$CXX -I$EOSLIB_INCLUDE -c HugoniotDB.C

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
