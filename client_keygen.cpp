#include <ctime>
#include <sstream>
#include <string>
#include <exception>
#include <thread>
#include <unistd.h>


#include "bitencryption.h"
#include "common.hpp"

using namespace std;
using namespace NTL;
using namespace boost::asio;
static void initialize_decode_server();
static bool DecodeBool(const Ctxt& cipher, const FHESecKey& key);

int main(int argc, const char* argv[]) 
{


	long m =0, p = 2, r = 1; // Native plaintext space
	// Computations will be 'modulo p'
	long L = 15;			// Levels 
	long c = 3;				// Columns in key switching matrix
	long w = 80;			// Hamming weight of secret key
	long d = 0;
	long slb=0;				//Slots Lower Bound : minimum number of slots 
	long security = 62;		// security parameter , number is directly proportional to the security.  
	/* Get a random number from whatever random source you have: /dev/random, 
	   /dev/urandom, input by the user from the command line, or whatever*/
	ZZ seed = to_ZZ(time(0));
	NTL::SetSeed(seed);

	m = FindM(security, L, c, p, d, slb, 0);
	FHEcontext context(m, p, r);
	buildModChain(context, L, c);
	ZZX G = context.alMod.getFactorsOverZZ()[0];

	//key generation
	FHESecKey secretKey(context);
	const FHEPubKey& publicKey = secretKey;
	secretKey.GenSecKey(w);
	EncryptedArray ea(context, G);

	
	//storing public key in file
	ofstream myfile;
	myfile.open ("public_key.bin");
	writePubKeyBinary(myfile, publicKey);
	myfile.close();

	//storing secret key in file	
	ofstream myfile1;
	myfile1.open ("secret_key.bin");
	writeSecKeyBinary(myfile1, secretKey); 
	myfile1.close();


	//storing context in file	
	ofstream myfile3;
	myfile3.open ("Context.bin");
	writeContextBaseBinary(myfile3, context);
	writeContextBinary(myfile3, context);
	myfile3.close();

	//client data	
	srand(time(0));
	long plain = rand() % (1ull << BITSIZE);
	cout << "Encrypting " << plain << endl;
	vector<Ctxt> cipher = Encrypt_bitwise(plain, publicKey);

	//storing encrypted client data in file
	ofstream myfile2;
	myfile2.open ("client_data.bin");
	myfile2 << cipher;
	myfile2.close();


	//make decode process
	pid_t pid = fork();
	if(pid == 0)
	{
		initialize_decode_server();
		return 0;
	}

}


static void initialize_decode_server()
{
	boost::asio::io_service io_service;
	ip::tcp::endpoint endpoint(ip::tcp::v4(), PORT_NUM);
	ip::tcp::acceptor acceptor(io_service, endpoint);

	ip::tcp::socket socket(io_service);
	acceptor.accept(socket);

	while(true)
	{
		vector<char> data, buf(4096);

		boost::system::error_code err;
		size_t len = 0;
		do {
			len = socket.read_some(buffer(buf), err);
			copy(buf.begin(), buf.begin() + len, back_inserter(data));
		} while(len == buf.size());



		if(err)
		{
			if(err == error::eof)
			{
				cerr << "Decode Server: Connection Disconnected" << endl;
			}
			else
			{
				cerr << __func__ << '\t' << "line:  " << __LINE__ << endl
					<< "error No: " << err.value() << endl
					<< err.message() << endl;
			}
			break;
		}

		//reading context from the file
		ifstream contextFile("Context.bin");
		unique_ptr<FHEcontext> context = buildContextFromBinary(contextFile);
		readContextBinary(contextFile, *context);
		contextFile.close();

		//reading secret key from the file
		FHESecKey secretKey(*context);
		ifstream secretFile;
		secretFile.open("secret_key.bin");
		readSecKeyBinary(secretFile, secretKey);
		secretFile.close();


		//reading public key from the secret key
		FHESecKey publicKey(*context);
		ifstream publicFile("public_key.bin");
		readPubKeyBinary(publicFile, publicKey);
		publicFile.close();

		//reading Ctxt data
		stringstream ss(string(data.begin(), data.end()));

		Ctxt cipher(publicKey);
		cipher.read(ss);

		bool msg = DecodeBool(cipher, secretKey);

		boost::system::error_code ignored_error;
		socket.write_some(buffer(reinterpret_cast<char*>(&msg), sizeof(msg)), ignored_error);

	}
	cerr << "server is terminated" << endl;
}

static bool DecodeBool(const Ctxt& cipher, const FHESecKey& key)
{
	vector<long> buf;
	key.getContext().ea->decrypt(cipher, key, buf);
	return static_cast<bool>(buf.front());
}
