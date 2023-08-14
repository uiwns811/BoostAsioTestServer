#include "ServerSession.h"
#include "PacketManager.h"

void ServerSession::ProcessConnect()
{
	wcout << "Connected to server" << endl;

	wcout << "ID ют╥б : ";
	wcin >> m_name;

	SendLoginPacket(m_name);

	RegisterRecv();
}

void ServerSession::RegisterSend(void* packet, std::size_t length)
{
	unsigned char* buff = new unsigned char[length];
	memcpy(buff, packet, length);

	m_socket.async_write_some(boost::asio::buffer(buff, length),
		[this, buff, length](boost::system::error_code ec, std::size_t bytes_transferred)
		{
			if (!ec)
			{
				delete buff;
			}
		});
}

void ServerSession::RegisterRecv()
{
	m_socket.async_read_some(boost::asio::buffer(recvbuff, BUF_SIZE),
		[this](boost::system::error_code ec, std::size_t length) {
			ProcessRecv(ec, length);
		});
}

void ServerSession::ProcessRecv(const boost::system::error_code& ec, std::size_t length)
{
	if (ec) {
		cout << "Receive Error EC[" << ec << "]\n";	// error. 
		return;
	}
	PacketManager::GetInstance()->Enqueue(recvbuff, length);
	RegisterRecv();
}

void ServerSession::SendLoginPacket(wstring name)
{
	CS_LOGIN_PACKET p;
	p.size = sizeof(CS_LOGIN_PACKET);
	p.type = PacketType::CS_LOGIN;
	wcscpy_s(p.name, name.c_str());
	RegisterSend(&p, p.size);
}

void ServerSession::SendChatPacket(wstring chat)
{
	CS_CHAT_PACKET p;
	p.size = sizeof(CS_CHAT_PACKET);
	p.type = PacketType::CS_CHAT;
	wcscpy_s(p.chat, chat.c_str());
	RegisterSend(&p, p.size);
}