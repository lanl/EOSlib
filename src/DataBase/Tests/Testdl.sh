#/bin/sh

# fetch environment variables
source ../../../SetEnv

CXX="${CMAKE_CXX_COMPILER} -std=c++11"

# compile shared object
$CXX -I$EOSLIB_INCLUDE -fPIC -c shared_obj.C  \
    || { echo "compile failed"; exit 1; }


# shared library
$CXX -shared -o libshared_obj.${EXT} \
       shared_obj.o -L$EOSLIB_SHARED_LIBRARY_PATH -lMath

# compile
$CXX -I$EOSLIB_INCLUDE -c Testdl.C  \
    || { echo "compile failed"; exit 1; }

# load
$CXX -o Testdl Testdl.o \
    -L$EOSLIB_SHARED_LIBRARY_PATH \
    -lDataBase -ldl \
     ./libshared_obj.${EXT}

# Run
./Testdl
