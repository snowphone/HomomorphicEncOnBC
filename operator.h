#pragma once
#include "common.hpp"

//#define _debug


using namespace std;
using namespace boost::asio;
enum class OP{ GT, LT };

static unique_ptr<ip::tcp::socket> sock;


static void connect()
{

	static boost::asio::io_service io_serv;
	ip::tcp::endpoint endpoint(ip::address::from_string("127.0.0.1"), PORT_NUM);
	sock = make_unique<ip::tcp::socket>(io_serv);

	boost::system::error_code connect_error;

	sock->connect(endpoint, connect_error);

	if(connect_error)
	{
		stringstream ss;
		ss << "error No: " << connect_error.value() << endl
			<< connect_error.message() << endl;
		throw std::runtime_error(ss.str());
	}

	Log("Connection established");

}


static bool operator_base(vector<Ctxt>& lhs, vector<Ctxt>& rhs, OP op )
{
	vector<Ctxt> maxi, mini;
	CtPtrs_vectorCt lhs_iter(lhs), rhs_iter(rhs),
				 max_iter(maxi), min_iter(mini);
	Ctxt mu(lhs.front().getPubKey()), ni = mu;

	compareTwoNumbers(max_iter, min_iter, mu, ni, lhs_iter, rhs_iter);
	


	auto func = [](const Ctxt& ct)
	{
		boost::system::error_code err, ignored_error;
		stringstream ss;
		vector<char> data(128);
		size_t len = 0;

		ct.write(ss);
		assert("stream doesn't exist" && ss.str().size());
		sock->write_some(buffer(ss.str().data(), ss.str().size()), ignored_error);
		len = sock->read_some(buffer(data), err);

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

	switch (op)
	{
		case OP::GT:
			return func(mu);
		case OP::LT:
			return func(ni);
		default:
			throw std::runtime_error("undefined input");
	}
}

inline bool operator>(vector<Ctxt>& lhs, vector<Ctxt>& rhs)
{
	return operator_base(lhs, rhs, OP::GT);
}


inline bool operator<(vector<Ctxt>& lhs, vector<Ctxt>& rhs)
{
	return operator_base(lhs, rhs, OP::LT);
}


