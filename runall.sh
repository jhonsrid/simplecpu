cd scripts
files=`ls *.asm`
for f in $files 
do
	../simplecpu $f 1
done
cd ..
