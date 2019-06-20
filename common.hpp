#pragma once
#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <ctime>
#include <exception>
#include <iostream>
#include <iterator>
#include <sstream>
#include <string>
#include <vector>

#include <boost/asio.hpp>

#include <helib/EncryptedArray.h>
#include <helib/FHE.h>
#include <helib/binaryArith.h>
#include <helib/binaryCompare.h>

#define _debug

#ifdef _debug
#define Log(X) (cerr << X << endl)
#else
#define Log(X) ;
#endif

constexpr size_t BITSIZE = 32;
constexpr unsigned short PORT_NUM = 31400;
