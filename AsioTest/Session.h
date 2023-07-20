#pragma once
#include "stdafx.h"

class Session abstract : public std::enable_shared_from_this<Session>
{
protected:
	tcp::socket m_socket;
	unsigned char data[BUF_SIZE];
	unsigned char packet[BUF_SIZE];
	int cur_packet_size;
	int prev_data_size;

	std::shared_ptr<Session> get_shared_ptr() { return shared_from_this(); }

	// 아래 4개는 PacketManager로 이동

	virtual void ProcessPacket(unsigned char* packet, int id)=0;
	virtual void ConstructData(unsigned char* buf, size_t io_byte)=0;
	//
	virtual void Send(void* packet, std::size_t length)=0;
	
	virtual void ProcessRecv() = 0;
	virtual void ProcessSend() = 0;
	virtual void ProcessConnect() = 0;
	virtual void ProcessDisconnect() = 0;

public:
	Session(tcp::socket socket) : m_socket(std::move(socket)) {};
	virtual ~Session() {};
};