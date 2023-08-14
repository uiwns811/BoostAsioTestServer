#include "Client.h"
#include "ServerSession.h"

void Client::RegisterConnect()
{
	m_socket.async_connect(endpoint, [this](boost::system::error_code ec) {
		if (!ec) {
			m_session = make_shared<ServerSession>(move(m_socket));
			m_bConnected = true;
			m_session->ProcessConnect();
		}
		else
			std::cout << "connect failed : " << ec.message() << endl;
		});
}

void Client::Run(boost::asio::io_context* io_context)
{
	m_thread = make_unique<boost::thread>(boost::thread([this, &io_context]() {io_context->run(); }));
}

void Client::Shutdown()
{
	m_thread->join();
}