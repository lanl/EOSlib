#!/bin/sh

PWD=`dirname $0`

exec perl $PWD/MaterialData.pl "$@" &
