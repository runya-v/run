#!/bin/sh

if [ $# ]
then
    if [ -d $1 ]
    then
        OUT_PATH="$PWD/Layout"

        if [ -d $OUT_PATH ]
        then
            rm -rf $OUT_PATH
        fi

        mkdir $OUT_PATH

        for X in $(find $1 -name "*\.html"); do iconv -f WINDOWS-1251 -t UTF-8 $X  > $OUT_PATH/${X##*/}; done
        exit 0
    fi
fi

echo "use: layout_iconv.sh <lauoyt path>"
exit 1
