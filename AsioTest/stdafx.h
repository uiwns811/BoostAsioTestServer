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

template <typename T>
bool compare(const pair<int, shared_ptr<T>>& a, const pair<int, shared_ptr<T>>& b);

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