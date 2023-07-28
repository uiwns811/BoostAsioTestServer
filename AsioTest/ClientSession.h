#pragma once
#include "stdafx.h"

class Room;
class ClientSession : public enable_shared_from_this<ClientSession>
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
	wstring m_name;
	weak_ptr<Room> m_room;

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

	void SendPacket(void* packet);

	void RegisterSend(void* packet, std::size_t length);
	void RegisterRecv();
	
	void ProcessRecv(const boost::system::error_code& ec, std::size_t length);
	void ProcessConnect();
	void ProcessDisconnect();
};