#pragma once
#include <iostream>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include "protocol.h"

#include <vector>
#include <string>
#include <unordered_map>
#include <algorithm>
#include <mutex>

using namespace std;
using boost::asio::ip::tcp;