#!/bin/bash

mkdir mydir
chmod 777 mydir

echo 2023 > myfile

mv moveme mydir/

cp copyme mydir/copied

cat readme

gcc bad.c 2>err.txt

mkdir gen
n=10
if (($# > 0))
then
	let n=$1
fi
while (($n > 0))
do
	touch gen/"$n.txt"
	let n=n-1
done
