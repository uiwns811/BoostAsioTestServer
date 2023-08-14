#pragma once
#include "stdafx.h"

class ServerSession;
class Client
{
private:
	tcp::socket m_socket;
	tcp::endpoint endpoint;
	unique_ptr<boost::thread> m_thread;

	bool m_bConnected = false;
public:
	shared_ptr<ServerSession> m_session;

public:
	Client(boost::asio::io_context& io_context, const tcp::endpoint& endpoints) : m_socket(io_context), endpoint(endpoints)
	{
		RegisterConnect();
	}

	void RegisterConnect();

	void Run(boost::asio::io_context* io_context);
	void Shutdown();

	bool IsConnected() { return m_bConnected; }
};