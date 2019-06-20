
log=experiment.csv
touch $log

date >> $log
echo "bitsize, time" >> $log

for bsz in 8 16 20 24 28 32
do
	printf "bitsize: %s \n" $bsz
	pattern='s/(?<=BITSIZE = )\d+/' 
	pattern+=$bsz
	pattern+='/'
	cmd='perl -pi -e "'
	cmd+=$pattern
	cmd+='" common.hpp '
	eval $cmd || exit
	make -j16 &> /dev/null && ./client_keygen_x 
	ret=$(	(time ./server_x) 2>&1 | grep 'real') || exit
	
	t=$(echo $ret | grep -Po '(?<=real\s).*')
	m=$(echo $t | grep -Po '\d+(?=m)')
	s=$(echo $t | grep -Po '(?<=m).*(?=s)')
	t=$(printf "print (%s * 60 + %s) / 10." $m $s | python)
	printf "%s, %s \n" $bsz $t 
	printf "%s, %s \n" $bsz $t >> $log
done

