#include <vector>
#include <exception>
#include <cstdlib>
#include <ctime>


#include <helib/FHE.h>
#include <helib/EncryptedArray.h>
#include <helib/binaryCompare.h>
#include <helib/binaryArith.h>
#include <NTL/BasicThreadPool.h>

#include "bitencryption.h"
#include "operator.h"
#include "common.hpp"

using namespace std;

int main(int argc, const char* argv[]) {
	long threadNum = 16;
	NTL::SetNumThreads(threadNum);

	Log("Threads: " << threadNum );
	

	// reading context from file
	ifstream infile2; 
	infile2.open("Context.bin", ios::binary); 
	unique_ptr<FHEcontext> context = buildContextFromBinary(infile2);
	readContextBinary(infile2, *context);
	infile2.close();

	FHEPubKey publicKey(*context);
	EncryptedArray ea(*context, context->alMod.getFactorsOverZZ()[0]);

	// reading public key from file
	ifstream infile; 
	infile.open("public_key.bin", ios::binary); 
	readPubKeyBinary(infile, publicKey);
	infile.close();


	// reading clients encrypted data from file
	vector<Ctxt> cipher1(1, Ctxt(publicKey));
	ifstream infile1; 
	infile1.open("client_data.bin", ios::binary); 
	infile1 >> cipher1;
	infile1.close();

	//connect to decode server
	connect();

	//test 10 times
	long loops = 10;
	Log("Comparison test");
	srand(time(0));
	for(long i=0, plain; i < loops; ++i)
	{
		plain = rand() % (1ull << BITSIZE);
		vector<Ctxt> cipher2 = Encrypt_bitwise(plain, publicKey);

		cout << "#" << i << ": client's cipher < " << plain << " ? " << boolalpha << (cipher1 < cipher2) << endl;
	}


#if 0
	//add computation
	vector<Ctxt> ret;
	CtPtrs_vectorCt ptr(ret);
	addTwoNumbers(ptr, CtPtrs_vectorCt(cipher1), CtPtrs_vectorCt(cipher2), BITSIZE);

	//storing addition results in file  
	ofstream myfile;
	myfile.open("add_results.bin", ios::binary);
	myfile << ret;
	myfile.close();
#endif

}
