#include "bitencryption.h"

using namespace NTL;

vector<Ctxt> Encrypt_bitwise(const long plain, const FHEPubKey& key)
{
	vector<Ctxt> ret;
	for (size_t i=0; i < BITSIZE; i++) 
	{
		Ctxt ct(key);
		key.Encrypt(ct, ZZX((plain >> i) & 1u));
		//ct.modDownToLevel(5);
		ret.push_back(ct);
	}
	return ret;
}

long Decrypt_bitwise(vector<Ctxt>& ct, const FHESecKey& key)
{
	long ret=0;
	std::reverse(ct.begin(), ct.end());

	for(auto& c : ct)
	{
		ret <<= 1;

		vector<long> buf;
		key.getContext().ea->decrypt(c, key, buf);
		ret |= static_cast<bool>(buf.front());
	}
	return ret;
}

ostream& operator<<(ostream& os, const vector<Ctxt>& ct)
{
	int32_t len = ct.size();
	os.write(reinterpret_cast<char*>(&len), sizeof(len));
	for_each(ct.begin(), ct.end(), [&os](const Ctxt& c){ c.write(os); });
	return os;
}

//To get public key, ct vector's size must be bigger than zero!
istream& operator>>(istream& is, vector<Ctxt>& ct)
{
	assert("Ctxt vector's size must be bigger than zero to contain the public key!" && ct.size());

	int32_t len = 0;
	is.read(reinterpret_cast<char*>(&len), sizeof(len));
	Ctxt c(ct.front());
	ct.clear();
	while(len--)
	{
		c.read(is);
		ct.push_back(c);
	}
	return is;
}


