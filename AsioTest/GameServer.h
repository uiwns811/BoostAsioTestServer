#pragma once
#include "IServer.h"

class SessionManager;
class RoomManager;

class GameServer : public IServer
{
private:
	boost::asio::io_context m_io_context;
	tcp::acceptor m_acceptor;
	tcp::socket m_socket;
	vector<boost::thread*> worker_threads;

private:
	void RegisterAccept();
	void ProcessAccept(const boost::system::error_code& ec);

public:
	GameServer(boost::asio::io_context& io_context, int port) : m_acceptor(io_context, tcp::endpoint(tcp::v4(), port)),
		m_socket(io_context) {};
	void Init();
	void Run(boost::asio::io_context* io_context);
	void Shutdown();
};