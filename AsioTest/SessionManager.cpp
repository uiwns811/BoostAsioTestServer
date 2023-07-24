#include "SessionManager.h"
#include "ClientSession.h"

int SessionManager::AddClient(const shared_ptr<ClientSession>& session)
{
	m_lock.lock();
	int new_id = m_client_id++;
	clients[new_id] = session;
	clients[new_id]->m_id = new_id;
	m_lock.unlock();
	return new_id;
}

shared_ptr<ClientSession> SessionManager::FindClient(int id)
{
	m_lock.lock();
	auto iter = clients.find(id);
	m_lock.unlock();
	if (iter != clients.end()) {
		return iter->second;
	}
	else {
		exit(1);
	}
}

void SessionManager::RemoveClient(int id)
{
	cout << "Client [" << id << "] is Disconnceted!" << endl;
	m_lock.lock();
	clients.erase(id);
	m_lock.unlock();
}
