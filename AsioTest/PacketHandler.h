#pragma once
#include "stdafx.h"

class ClientSession;
class PacketHandler
{
private:

public:
	void CSLoginHandler(shared_ptr<ClientSession> session, unsigned char* packet);
	void CSLeaveHandler(shared_ptr<ClientSession> session, unsigned char* packet);
	void CSSelectRoomHandler(shared_ptr<ClientSession> session, unsigned char* packet);
	void CSChatHandler(shared_ptr<ClientSession> session, unsigned char* packet);
};

