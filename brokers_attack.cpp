#include <ctime>
#include <helib/FHE.h>
#include <helib/EncryptedArray.h>
#include "bitencryption.h"
#include "common.h"

#include <fstream>
using namespace std;

/*
 * 클라이언트의 키가 아닌 서버가 직접 만든 키를 이용했을 때, 
 * 손상된 상태로 복호화가 됨을 알 수 있었다.
 */

int main() {
	// reading context from file
	ifstream infile2; 
	infile2.open("Context.bin"); 
	auto context = buildContextFromBinary(infile2);
	readContextBinary(infile2, *context);
	infile2.close();
	srand(clock());

	FHEPubKey publicKey(*context);
	FHESecKey secretKey(*context);
	long w = rand();
	secretKey.GenSecKey(w); /* <---- brute force attack : Trying different hamming weight will gives us secret key. At w=80 , the attacker finds the secret key*/

	// reading public key from file
	ifstream infile; 
	infile.open("public_key.bin"); 
	readPubKeyBinary(infile, publicKey);
	infile.close();

	vector<Ctxt> ct1(1, Ctxt(publicKey));

	// reading add results from file
	ifstream infile1; 
	infile1.open("add_results.bin"); 
	infile1>>ct1;
	infile1.close();

	//decrypting
	cout << "Attacker's decryption results: " << Decrypt_bitwise(ct1, secretKey) << endl;
}
