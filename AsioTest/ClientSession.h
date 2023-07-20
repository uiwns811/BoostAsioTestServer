#pragma once
#include "stdafx.h"

class ClientSession : public std::enable_shared_from_this<ClientSession>
{
private:
	tcp::socket m_socket;
	unsigned char data[BUF_SIZE];
	unsigned char packet[BUF_SIZE];
	int cur_packet_size;
	int prev_data_size;
	mutex m_lock;

public:
	int m_id;
	string m_name;

private:
	void ProcessPacket(unsigned char* packet, int id);
	void ConstructData(unsigned char* buf, size_t io_byte);

public:
	ClientSession(tcp::socket socket) : m_socket(std::move(socket))
	{
		cur_packet_size = 0;
		prev_data_size = 0;
	}
	virtual ~ClientSession() {};

	void SendPacket(void* packet, unsigned id);

	void RegisterSend(void* packet, std::size_t length);
	void RegisterRecv();
	
	void ProcessRecv(const boost::system::error_code& ec, std::size_t length);
	void ProcessSend(const boost::system::error_code& ec, std::size_t length);
	void ProcessConnect();
	void ProcessDisconnect();
};