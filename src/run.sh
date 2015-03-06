#!/bin/sh
path=$PWD/$0
#echo "# " ${path##*/}
#echo "# " ${path%.*}
cd ${path%.*}
${path%.*}/run $*
cd -
