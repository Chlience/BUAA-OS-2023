mkdir mydir
chmod 777 mydir

echo "2023" > myfile

mv moveme mydir/
cp copyme mydir/copied
cat readme

gcc bad.c 2>err.txt

echo "test"

mkdir gen

var=10

if (($# == 1))
then
	let var=$1
fi

echo $var
