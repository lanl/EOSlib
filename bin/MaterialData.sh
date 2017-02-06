#!/bin/sh

prog=$0
#  Follow symbolic links for absolute path to build (root) directory
while [ -L $prog ] ; do
    link=`ls -l $prog | sed -e 's/.*->[ 	]*//'`
    case $link in
    /*)
        prog=$link;;
     *)
        prog="`dirname $prog`/$link";;
    esac
done

BUILD=`(cd \`dirname $prog\`/.. ; pwd)`

export EOSLIB_DATA_PATH=$BUILD/DATA
export EOSLIB_SHARED_LIBRARY_PATH=$BUILD/lib
export PATH=$BUILD/bin:$PATH

export EOSdata=$EOSLIB_DATA_PATH/EOS.data
export LD_LIBRARY_PATH=$EOSLIB_SHARED_LIBRARY_PATH:$LD_LIBRARY_PATH

exec perl $BUILD/GUI/MaterialData.pl "DataFile=$EOSdata" "$@" &
