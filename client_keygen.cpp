#include <ctime>
#include <sstream>
#include <string>
#include <exception>
#include <thread>
#include <unistd.h>
#include <thread>


#include "bitencryption.h"
#include "common.h"

using namespace std;
using namespace NTL;
using namespace boost::asio;

static long mValues[][15] = { 
// { p, phi(m),   m,   d, m1, m2, m3,    g1,   g2,   g3, ord1,ord2,ord3, B,c}
  {  2,    48,   105, 12,  3, 35,  0,    71,    76,    0,   2,  2,   0, 25, 2},
  {  2 ,  600,  1023, 10, 11, 93,  0,   838,   584,    0,  10,  6,   0, 25, 2},
  {  2,  2304,  4641, 24,  7,  3,221,  3979,  3095, 3760,   6,  2,  -8, 25, 3},
  {  2, 15004, 15709, 22, 23,683,  0,  4099, 13663,    0,  22, 31,   0, 25, 3},
  {  2, 27000, 32767, 15, 31,  7, 151, 11628, 28087,25824, 30,  6, -10, 28, 4}
};

int main(int argc, const char* argv[]) 
{
	bool bBootstrap = true;
	long* vals = mValues[0];

	long m = vals[2], p = vals[0], r = 1; // Native plaintext space
	// Computations will be 'modulo p'
	long L = bBootstrap ? 900 : 30 * (7 + NTL::NumBits(BITSIZE + 2));	// Levels 
	long B = vals[13];
	long c = vals[14];													// Columns in key switching matrix
	/* Get a random number from whatever random source you have: /dev/random, 
	   /dev/urandom, input by the user from the command line, or whatever*/
	ZZ seed = to_ZZ(time(nullptr));
	NTL::SetSeed(seed);

	NTL::Vec<long> mvec;
	append(mvec, vals[4]);
	if (vals[5]>1) append(mvec, vals[5]);
	if (vals[6]>1) append(mvec, vals[6]);

	std::vector<long> gens;
	gens.push_back(vals[7]);
	if (vals[8]>1) gens.push_back(vals[8]);
	if (vals[9]>1) gens.push_back(vals[9]);

	std::vector<long> ords;
	ords.push_back(vals[10]);
	if (abs(vals[11])>1) ords.push_back(vals[11]);
	if (abs(vals[12])>1) ords.push_back(vals[12]);

	FHEcontext context(m, p, r, gens, ords);
	buildModChain(context, L, c, bBootstrap);

	if(bBootstrap)
		context.makeBootstrappable(mvec, /*t=*/0);

	//key generation
	FHESecKey secretKey(context);
	const FHEPubKey& publicKey = secretKey;
	secretKey.GenSecKey();

	
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
}

