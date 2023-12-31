#pragma once
#include "stdafx.h"

class ServerSession : public enable_shared_from_this<ServerSession>
{
private:	
	tcp::socket m_socket;
	unsigned char recvbuff[BUF_SIZE];
	unsigned char remainData[BUF_SIZE];
	int prev_data_size;

	bool m_bChat = false;

public:
	wstring m_name;

public:
	ServerSession(tcp::socket socket) : m_socket(std::move(socket))
	{
		prev_data_size = 0;
	}

	void ProcessConnect();

	void RegisterSend(void* packet, std::size_t length);
	void RegisterRecv();

	void ProcessRecv(const boost::system::error_code& ec, std::size_t length);

	void SendLoginPacket(wstring name);
	void SendChatPacket(wstring chat);

	bool ChatCheck() { return m_bChat; }
	void SetChat(bool _bTrue) { m_bChat = _bTrue; }
};

