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
LIB_OBJ = $(addprefix ../src/,binaryArith.o binaryCompare.o NumbTh.o timing.o bluestein.o PAlgebra.o  CModulus.o FHEContext.o IndexSet.o DoubleCRT.o FHE.o KeySwitching.o Ctxt.o EncryptedArray.o replicate.o hypercube.o matching.o powerful.o BenesNetwork.o permutations.o PermNetwork.o OptimizePermutations.o eqtesting.o polyEval.o extractDigits.o EvalMap.o recryption.o debugging.o matmul.o intraSlot.o tableLookup.o binio.o)

LDLIBS = $(NTL) $(GMP) -lm -lboost_system -pthread  -lhelib
LIBS = bitencryption.o 
HEADER = operator.h common.hpp

%.o: %.cpp $(HEADER) 
	$(CC) $(CFLAGS) -c $< -o $@



all: $(OBJS)
	$(MAKE) client_keygen_x
	$(MAKE) server_x
	$(MAKE) client_decrypt_x
	$(MAKE) brokers_attack_x
	export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/lib

test:
	$(MAKE) all
	./tester.sh

#fhe.a: $(LIB_OBJ)
	#$(AR) $(ARFLAGS) fhe.a $(LIB_OBJ)

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

