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
#include <cstring>
#include <concurrent_queue.h>

using namespace std;
using namespace Concurrency;
using boost::asio::ip::tcp;

struct Compare
{
	template <typename T>
	bool operator() (const pair<int, shared_ptr<T>>& cur, const int& key) {
		return (cur.first < key);
	}

	template <typename T>
	bool operator() (const int& key, const pair<int, shared_ptr<T>>& cur) {
		return (cur.first > key);
	}
};