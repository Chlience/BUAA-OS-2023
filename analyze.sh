#!/bin/bash

if [ $# -eq 1 ];
then
    # Your code here. (1/4)
    grep -E 'WARN|ERROR' $1 > bug.txt
else
    case $2 in
    "--latest")
        # Your code here. (2/4)
        tail -n 5 $1
    ;;
    "--find")
        # Your code here. (3/4)
        grep -E $3 $1 >$3.txt
    ;;
    "--diff")
        # Your code here. (4/4)
        if diff -q $1 $3 >/dev/null
        then
            echo same
        else
            echo notsame
        fi
    ;;
    esac
fi
