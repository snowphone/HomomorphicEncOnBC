
for bsz in 8 16 20 24 28 32
do
	pattern='s/(?<=BITSIZE = )\d+/' 
	pattern+=$bsz
	pattern+='/'
	cmd='perl -pi -e "'
	cmd+=$pattern
	cmd+='" common.h '
	eval $cmd || exit
	make verbose -j16 &> /dev/null && ./client_keygen
	./work
done

