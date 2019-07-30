#include <ctime>
#include <sstream>
#include <string>
#include <exception>
#include <thread>
#include <chrono>
#include <array>

#include <unistd.h>


#include "bitencryption.h"
#include "common.h"

using namespace std;
using namespace NTL;
using namespace boost::asio;

using Sock_ptr = shared_ptr<ip::tcp::socket>;

static bool decode_bool(const Ctxt& cipher, const FHESecKey& key);
void process_sesseion(Sock_ptr socket_ptr) ;

int main(int argc, const char* argv[]) 
{
 	boost::asio::io_service io_service;
	ip::tcp::endpoint endpoint(ip::tcp::v4(), PORT_NUM);
	ip::tcp::acceptor acceptor(io_service, endpoint);

	while (true) {
		Sock_ptr socket_ptr = make_shared<ip::tcp::socket>(io_service);
		acceptor.accept(*socket_ptr);
		auto t1 = make_shared<thread>(process_sesseion, std::move(socket_ptr));
		t1->detach();
	}
}

void process_sesseion(Sock_ptr socket_ptr) {
	while(true)
	{
		vector<char> data;
		array<char, 4096> buf;

		boost::system::error_code err;
		size_t len = 0;
		do {
			len = socket_ptr->read_some(boost::asio::buffer(buf), err);
			copy(buf.begin(), buf.begin() + len, back_inserter(data));
		} while(len == buf.size());


		if(err)
		{
			if(err == error::eof) {
				/* pass */
			} else {
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

		bool msg = decode_bool(cipher, secretKey);

		boost::system::error_code ignored_error;
		socket_ptr->write_some(buffer(reinterpret_cast<char*>(&msg), sizeof(msg)), ignored_error);
	}
}

static bool decode_bool(const Ctxt& cipher, const FHESecKey& key)
{
	vector<long> buf;
	key.getContext().ea->decrypt(cipher, key, buf);
	return static_cast<bool>(buf.front());
}
