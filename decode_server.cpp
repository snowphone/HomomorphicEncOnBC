#include <ctime>
#include <sstream>
#include <string>
#include <exception>
#include <thread>
#include <unistd.h>
#include <chrono>


#include "bitencryption.h"
#include "common.h"

using namespace std;
using namespace NTL;
using namespace boost::asio;

static void initialize_decode_server();
static bool DecodeBool(const Ctxt& cipher, const FHESecKey& key);

int main(int argc, const char* argv[]) 
{
    initialize_decode_server();
}


static void initialize_decode_server()
{
	boost::asio::io_service io_service;
	ip::tcp::endpoint endpoint(ip::tcp::v4(), PORT_NUM);
	ip::tcp::acceptor acceptor(io_service, endpoint);

	ip::tcp::socket socket(io_service);

	while(true)
	{
		acceptor.accept(socket);

		vector<char> data, buf(4096);

		boost::system::error_code err;
		size_t len = 0;
		do {
			len = socket.read_some(buffer(buf), err);
			copy(buf.begin(), buf.begin() + len, back_inserter(data));
		} while(len == buf.size());



		if(err)
		{
			if(err == error::eof) {
				cerr << "Decode Server: Connection Disconnected" << endl;
				continue;
			}
			else {
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
