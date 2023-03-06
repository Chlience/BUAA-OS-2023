mkdir mydir
chmod 777 mydir

echo "2023" > myfile

mv moveme mydir/
cp copyme mydir/copied
cat readme

gcc bad.c 2>err.txt

mkdir gen

n=10
if (($# > 0))
then
    n=$1
fi
while (($n > 0))
do
    echo "" > gen/"$n.txt"
    n=$((n-1))
done
