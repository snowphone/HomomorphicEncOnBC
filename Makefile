CC = g++
#
#CFLAGS = -g -O2 -std=c++11 -DDEBUG_PRINTOUT -Wfatal-errors -Wshadow -Wall -I/usr/local/include 
CFLAGS = -g -O2 -std=c++14 -DFHE_THREADS -DFHE_BOOT_THREADS -fmax-errors=2 

# useful flags:
#   -std=c++11
#   -DNO_HALF_SIZE_PRIME  tells helib to not use the half size prime
#                         in the prime chain
#
#   -DFHE_THREADS  tells helib to enable generic multithreading capabilities;
#                  must be used with a thread-enabled NTL and the -pthread
#                  flag should be passed to gcc
#
#   -DFHE_BOOT_THREADS  tells helib to use a multithreading strategy for
#                       bootstrapping; requires -DFHE_THREADS (see above)

#  If you get compilation errors, you may need to add -std=c++11 or -std=c++0x

LD = g++
AR = ar
ARFLAGS=ruv
GMP=-lgmp
NTL=-lntl


# NOTE: NTL and GMP are distributed under LGPL (v2.1), so you can link
#       against them as dynamic libraries.
#LDLIBS = -L/usr/local/lib $(NTL) $(GMP) -lm -lboost_system
SRCS = $(wildcard *.cpp)
OBJS = $(SRCS:.cpp=.o)
TARGETS = client_keygen work client_decrypt brokers_attack decode_server
#LIB_OBJ = $(addprefix ../src/,binaryArith.o binaryCompare.o NumbTh.o timing.o bluestein.o PAlgebra.o  CModulus.o FHEContext.o IndexSet.o DoubleCRT.o FHE.o KeySwitching.o Ctxt.o EncryptedArray.o replicate.o hypercube.o matching.o powerful.o BenesNetwork.o permutations.o PermNetwork.o OptimizePermutations.o eqtesting.o polyEval.o extractDigits.o EvalMap.o recryption.o debugging.o matmul.o intraSlot.o tableLookup.o binio.o)

LDLIBS = $(NTL) $(GMP) -lm -lboost_system -pthread  -lhelib
LIBS = bitencryption.o  operator.o
HEADER = operator.h common.h

%.o: %.cpp $(HEADER) 
	$(CC) $(CFLAGS) -c $< -o $@



all: $(TARGETS)
	export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/lib

verbose: CFLAGS += -D VERBOSE
verbose: $(TARGETS)

test:
	$(MAKE) all
	./tester.sh

client_keygen: client_keygen.o $(LIBS)
	$(CC) -o $@ $^ $(LDLIBS)

work: work.o $(LIBS)
	$(CC) -o $@ $^ $(LDLIBS)

client_decrypt: client_decrypt.o $(LIBS)
	$(CC) -o $@ $^ $(LDLIBS)

brokers_attack: brokers_attack.o $(LIBS)
	$(CC) -o $@ $^ $(LDLIBS)

decode_server: decode_server.o $(LIBS)
	$(CC) -o $@ $^ $(LDLIBS)

#링크는 종속적 obj를 왼쪽에, 일반적인 obj를 오른쪽에 두자!
./%_x: %.o $(LIBS)
	$(CC) -o $@ $^ $(LDLIBS)


check_Bin_IO_x: Test_Bin_IO_x
	./Test_Bin_IO_x p=257 m=11008 r=1


DoubleCRT.o: DoubleCRT.cpp $(HEADER)
	$(CC) $(CFLAGS) -c DoubleCRT.cpp




clean:
	rm -f *.o *_x *_x.exe core.*
	rm -rf *.dSYM
	rm -f *.bin
	rm -f $(TARGETS)
	rm -f *.bin

