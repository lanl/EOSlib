#/bin/sh

# fetch environment variables
source ../../build/SetEnv

CXX=${CMAKE_CXX_COMPILER}

# compile
$CXX -I$EOSLIB_INCLUDE -c LowLevelHugoniot-new.C \
    || { echo "compile failed"; exit 1; }


# load
$CXX -o LowLevelHugoniot-new LowLevelHugoniot-new.o \
    -L$EOSLIB_SHARED_LIBRARY_PATH \
    -lEOS -lEOStypes \
    -Wl,-rpath,$EOSLIB_SHARED_LIBRARY_PATH
#
# Notes:
# -lIdealGas can be replaced by -lMaterials
#
# if environment variable is set
#   export LD_LIBRARY_PATH=$EOSLIB_SHARED_LIBRARY_PATH:$LD_LIBRARY_PATH
# then loader argument
#   -Wl,-rpath,$EOSLIB_SHARED_LIBRARY_PATH
# is not needed
#

# Run
./LowLevelHugoniot-new
