#pragma once
#include "stdafx.h"

class ClientSession;
class Room : public enable_shared_from_this<Room>
{
private:
	mutex m_lock;

public:
	vector<pair<int, shared_ptr<ClientSession>>> clients;
	//unordered_map<int, shared_ptr<ClientSession>> clients;
	int m_id;

public:
	void EnterPlayer(const shared_ptr<ClientSession>& session);
	void LeavePlayer(const shared_ptr<ClientSession>& session);

	void SendEnterRoomPacket(int id);
	void SendChatPacket(int sender, const wchar_t* name, const wchar_t* chat);
};

