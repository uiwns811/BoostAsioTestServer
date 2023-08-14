#pragma once
#include "stdafx.h"

class PacketManager
{/*
private:
	queue<unsigned char*> m_queue;
	unordered_map<PacketType, function<void(shared_ptr<ClientSession>, unsigned char*)>> m_handlers;

	unsigned char remainData[BUF_SIZE];
	int prev_data_size;

private:
	void ParseData();

	void BindHandler(PacketType, void(PacketManager::* handler)(shared_ptr<ClientSession>, unsigned char*));

	void CSLoginHandler(shared_ptr<ClientSession> session, unsigned char* packet);
	void CSLeaveHandler(shared_ptr<ClientSession> session, unsigned char* packet);
	void CSSelectRoomHandler(shared_ptr<ClientSession> session, unsigned char* packet);
	void CSChatHandler(shared_ptr<ClientSession> session, unsigned char* packet);

public:
	void Init();

	void Enqueue(shared_ptr<ClientSession> session, unsigned char* data, size_t length);
	void Dequeue();

	void Run();*/
};