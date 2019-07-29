#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>

#include "bitencryption.h"
#include "common.h"

using namespace std;

int main(){
  // reading context from file
  ifstream infile2; 
  infile2.open("Context.bin", ios::binary); 
  unique_ptr<FHEcontext> context = buildContextFromBinary(infile2);
  readContextBinary(infile2, *context);
  infile2.close();

  EncryptedArray ea(*context, context->alMod.getFactorsOverZZ().front());

  
  
  // reading secret key from file  
  FHESecKey secretKey(*context);
  ifstream infile1; 
  infile1.open("secret_key.bin", ios::binary); 
  readSecKeyBinary(infile1, secretKey);
  infile1.close();
  
 // reading add results from file    
  vector<Ctxt> cipher(1, Ctxt(FHEPubKey(*context)));
  ifstream myfile1; 
  myfile1.open("add_results.bin", ios::binary); 
  myfile1 >> cipher;
  myfile1.close();

 //decrypting  
  cout << "add results " << Decrypt_bitwise(cipher, secretKey) << endl;
}
