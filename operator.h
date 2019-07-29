#pragma once
#include "common.h"

using namespace std;
using namespace boost::asio;


class Operator {
	friend bool operator>(vector<Ctxt>& lhs, vector<Ctxt>& rhs);
	friend bool operator<(vector<Ctxt>& lhs, vector<Ctxt>& rhs);

	static Operator* var;

	boost::asio::io_service io_serv;
	ip::tcp::socket sock;
	vector<zzX> unpackEncoding;

	Operator(string ipAddress="127.0.0.1", int port=PORT_NUM);
public:
	static Operator& getInstance();

	enum class OP{ GT, LT };

	bool operator_base(vector<Ctxt>& lhs, vector<Ctxt>& rhs, OP op);
private:
	bool compare(const Ctxt& ct);
};


bool operator>(vector<Ctxt>& lhs, vector<Ctxt>& rhs);

bool operator<(vector<Ctxt>& lhs, vector<Ctxt>& rhs);



