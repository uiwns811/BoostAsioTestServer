#include "GameServer.h"
#include "ClientSession.h"
#include "SessionManager.h"

void GameServer::Run(boost::asio::io_context* io_context)
{
	RegisterAccept();

	std::size_t numThreads = std::thread::hardware_concurrency();
	for (int i = 0; i < numThreads; i++) {
		//worker_threads.emplace_back(new boost::thread(worker_thread, &io_context));
		worker_threads.emplace_back(new boost::thread([this, &io_context]() {io_context->run(); }));
	}

	for (auto& wth : worker_threads) {
		wth->join();
		delete wth;
	}
}
	
void GameServer::RegisterAccept()
{
	m_acceptor.async_accept(m_socket, boost::bind(&GameServer::ProcessAccept, this, boost::asio::placeholders::error));
}

void GameServer::ProcessAccept(const boost::system::error_code& ec)
{
	if (!ec)
		make_shared<ClientSession>(move(m_socket))->ProcessConnect();
	RegisterAccept();
}