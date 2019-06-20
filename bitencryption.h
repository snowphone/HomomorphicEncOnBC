#pragma once
#include "common.hpp"
using std::vector;
using std::istream;
using std::ostream;

std::vector<Ctxt> Encrypt_bitwise(const long plain, const FHEPubKey& key);

long Decrypt_bitwise(vector<Ctxt>& ct, const FHESecKey& key);

istream& operator>>(istream& is, vector<Ctxt>& ct);

ostream& operator<<(ostream& os, const vector<Ctxt>& ct);
