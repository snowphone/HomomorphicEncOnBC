#include <vector>
#include <exception>
#include <cstdlib>
#include <ctime>
#include <thread>
#include <chrono>


#include <helib/FHE.h>
#include <helib/EncryptedArray.h>
#include <helib/binaryCompare.h>
#include <helib/binaryArith.h>
#include <NTL/BasicThreadPool.h>

#include "bitencryption.h"
#include "operator.h"
#include "common.h"

using namespace std;

int main(int argc, const char* argv[]) {
	long threadNum = thread::hardware_concurrency();
	NTL::SetNumThreads(threadNum);

#ifdef VERBOSE
	cout << "Threads, " << threadNum << endl;
#else
	Log("Threads: " << threadNum );
#endif
	

	// reading context from file
	ifstream infile2; 
	infile2.open("Context.bin", ios::binary); 
	unique_ptr<FHEcontext> context = buildContextFromBinary(infile2);
	readContextBinary(infile2, *context);
	infile2.close();
	Log("Successfully loaded Context.bin");

	FHEPubKey publicKey(*context);
	EncryptedArray ea(*context, context->alMod.getFactorsOverZZ()[0]);

	// reading public key from file
	ifstream infile; 
	infile.open("public_key.bin", ios::binary); 
	readPubKeyBinary(infile, publicKey);
	infile.close();
	Log("Successfully loaded public key");


	// reading clients encrypted data from file
	vector<Ctxt> cipher1(1, Ctxt(publicKey));
	ifstream infile1; 
	infile1.open("client_data.bin", ios::binary); 
	infile1 >> cipher1;
	infile1.close();
	Log("Successfully loaded binary data");

	//connect to decode server

	//test 10 times
	long loops = 10;
	srand(time(0));
	for(long i=0, plain; i < loops; ++i)
	{
		plain = rand() % (1ull << BITSIZE);
		vector<Ctxt> cipher2 = Encrypt_bitwise(plain, publicKey);

		auto beg = chrono::system_clock::now();
		bool ret = (cipher1 < cipher2);
		auto end = chrono::system_clock::now();
		auto time = chrono::duration_cast<chrono::milliseconds>(end - beg).count();
#ifdef VERBOSE
		cout << plain << ", " << boolalpha << ret << ", " << time << endl;
#else
		cout << "#" << (i + 1) << ": client's cipher < " << plain << " ? " << boolalpha << (cipher1 < cipher2) << endl;
#endif
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
