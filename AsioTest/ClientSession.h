#pragma once
#include "stdafx.h"

class Room;
class ClientSession : public enable_shared_from_this<ClientSession>
{
private:
	tcp::socket m_socket;
	unsigned char recvbuff[BUF_SIZE];
	unsigned char remainData[BUF_SIZE];
	int prev_data_size;
	mutex m_lock;

public:
	int m_id;
	wstring m_name;
	weak_ptr<Room> m_room;

public:
	ClientSession(tcp::socket socket) : m_socket(std::move(socket))
	{
		prev_data_size = 0;
	}
	virtual ~ClientSession() { cout << "ClientSession �Ҹ�" << endl; };

	void SendPacket(void* packet);

	void RegisterSend(void* packet, std::size_t length);
	void RegisterRecv();
	
	void ProcessRecv(const boost::system::error_code& ec, std::size_t length);
	void ProcessConnect();
	void ProcessDisconnect();
};