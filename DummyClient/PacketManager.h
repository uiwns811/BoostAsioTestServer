#pragma once
#include "stdafx.h"

class ServerSession;
class PacketManager : public TSingleton<PacketManager>
{
private:
	shared_ptr<ServerSession> m_session;
	queue<unsigned char*> m_queue;
	unordered_map<PacketType, function<void(unsigned char*)>> m_handlers;

	unsigned char remainData[BUF_SIZE];
	int prev_data_size;

private:
	void BindHandler(PacketType, void(PacketManager::* handler)(unsigned char*));

	void SCLoginHandler(unsigned char* packet);
	void SCEnterLobbyHandler(unsigned char* packet);
	void SCLeavePlayerHandler(unsigned char* packet);
	void SCRoomInfoHandler(unsigned char* packet);
	void SCEnterRoomHandler(unsigned char* packet);
	void SCChatHandler( unsigned char* packet);

public:
	void Init(shared_ptr<ServerSession> session);

	void Enqueue(unsigned char* data, size_t length);
	void Dequeue();
};