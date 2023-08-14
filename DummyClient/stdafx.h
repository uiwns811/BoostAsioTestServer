#pragma once
#include <iostream>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include "../Common/protocol.h"
#include "../Common/TSingleton.h"

#include <string>
#include <cstring>
#include <queue>
#include <unordered_map>

using namespace std;
using boost::asio::ip::tcp;