mkdir  mydir
chmod 777 mydir

echo 2023 > myfile

mv -f moveme mydir/

cp -f copyme mydir/copied

cat readme

gcc bad.c 2>err.txt

rm -rf gen
mkdir gen
n=10
if (($# > 0))
then
	let n=$1
fi
while (($n > 0))
do
	echo "" >gen/"$n.txt"
	let n=n-1
done
