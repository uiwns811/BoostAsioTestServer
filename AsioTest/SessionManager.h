#pragma once
#include "stdafx.h"
#include "TSingleton.h"

class ClientSession;
class SessionManager : public TSingleton<SessionManager>
{
private:
	unordered_map<int, shared_ptr<ClientSession>> clients;
	mutex m_lock;
	atomic_int m_client_id = 0;

public:
	int AddClient(const shared_ptr<ClientSession>& sessionz);
	shared_ptr<ClientSession> FindClient(int id);
	void RemoveClient(int id);

	unordered_map<int, shared_ptr<ClientSession>> GetClients() { return clients; }
};

