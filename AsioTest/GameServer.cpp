#include "GameServer.h"
#include "ClientSession.h"
#include "SessionManager.h"
#include "RoomManager.h"

void GameServer::Init()
{
	//m_sessionManager = make_unique<SessionManager>();
	//m_roomManager = make_unique<RoomManager>();
}

void GameServer::Shutdown()
{

}

void GameServer::Run(boost::asio::io_context* io_context)
{
	RegisterAccept();

	std::size_t numThreads = std::thread::hardware_concurrency();
	for (int i = 0; i < numThreads; i++) {
		//worker_threads.emplace_back(new boost::thread(worker_thread, &io_context));
		//worker_threads.emplace_back(new boost::thread([this, &io_context]() {io_context->run(); }));
		worker_threads.emplace_back(new boost::thread(boost::bind(&boost::asio::io_context::run, io_context)));
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
	if (!ec) {
		//make_shared<ClientSession>(move(m_socket))->ProcessConnect();
		shared_ptr<ClientSession> newSession = make_shared<ClientSession>(move(m_socket));
		//m_sessionManager->AddClient(newSession);
		SessionManager::GetInstance()->AddClient(newSession);
		newSession->RegisterRecv();
	}
	RegisterAccept();
}