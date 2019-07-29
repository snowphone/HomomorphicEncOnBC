#include "operator.h"
#include <helib/intraSlot.h>

using namespace std;
using namespace boost::asio;

Operator* Operator::var = nullptr;

Operator& Operator::getInstance() {
	if(!var)
		var = new Operator();
	return *var;
}

Operator::Operator(string ipAddress, int port) : sock(ip::tcp::socket(io_serv))
{
	ip::tcp::endpoint endpoint(ip::address::from_string(ipAddress), PORT_NUM);
	boost::system::error_code connect_error;
	sock.connect(endpoint, connect_error);

	if(connect_error)
	{
		stringstream ss;
		ss << "error No: " << connect_error.value() << endl
			<< connect_error.message() << endl;
		throw std::runtime_error(ss.str());
	}
}
bool Operator::operator_base(vector<Ctxt>& lhs, vector<Ctxt>& rhs, OP op )
{
	vector<Ctxt> maxi, mini;
	CtPtrs_vectorCt lhs_iter(lhs), rhs_iter(rhs);
	Ctxt mu(lhs.front().getPubKey()), ni = mu;

	compareTwoNumbers(mu, ni, lhs_iter, rhs_iter);


	switch (op)
	{
		case OP::GT:
			return compare(mu);
		case OP::LT:
			return compare(ni);
		default:
			throw std::runtime_error("undefined input");
	}
}
bool Operator::compare(const Ctxt& ct)
{
	boost::system::error_code err, ignored_error;
	stringstream ss;
	vector<char> data(128);
	size_t len = 0;

	ct.write(ss);
	assert("stream doesn't exist" && ss.str().size());
	sock.write_some(buffer(ss.str().data(), ss.str().size()), ignored_error);
	len = sock.read_some(buffer(data), err);

	if (err)
	{
		if(err == error::eof)
			throw runtime_error("Connection disconnected");
		else
		{
			stringstream ss;
			ss << "function: " << __func__ << " line:  " << __LINE__ << endl
				<< "error No: " << err.value() << endl
				<< err.message() << endl;;
			throw runtime_error(ss.str());
		}
	}
	return static_cast<bool>(data[0]);
};


bool operator>(vector<Ctxt>& lhs, vector<Ctxt>& rhs)
{
	return Operator::getInstance().operator_base(lhs, rhs, Operator::OP::GT);
}


bool operator<(vector<Ctxt>& lhs, vector<Ctxt>& rhs)
{
	return Operator::getInstance().operator_base(lhs, rhs, Operator::OP::LT);
}

