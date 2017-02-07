#!/bin/sh

CWD=`pwd`
GUI=$CWD/`dirname $0`

cd $GUI/../../build;
pwd
echo DEBUG
source SetEnv

cd $CWD
exec perl $GUI/MaterialData.pl "DataFile=$EOSLIB_DATA_PATH/EOS.data" &
