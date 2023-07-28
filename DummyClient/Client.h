#pragma once
#include "stdafx.h"

class Client
{
private:
	tcp::socket m_socket;
	boost::asio::io_context io_context;
	tcp::endpoint endpoint;
	unsigned char data[BUF_SIZE];
	unsigned char packet[BUF_SIZE];
	int cur_packet_size;
	int prev_data_size;

	wstring m_name;

public:
	bool isChatting = false;

public:
	Client(boost::asio::io_context& io_context, const tcp::endpoint& endpoints) : m_socket(io_context), endpoint(endpoints)
	{
		wcout << "ID ют╥б : ";
		wcin >> m_name;
		Connect();
	}

	Client(boost::asio::io_context& io_context) : m_socket(io_context)
	{

	}

	void Connect();

	void OnConnected();

	void RegisterSend(void* packet, std::size_t length);
	void RegisterRecv();

	void ProcessRecv(const boost::system::error_code& ec, std::size_t length);
	void ConstructData(unsigned char* buf, size_t io_byte);
	void ProcessPacket(unsigned char* packet);

	void SendLoginPacket(wstring name);

	void SendChatPacket(wstring chat);
};