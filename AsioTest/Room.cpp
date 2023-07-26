#include "Room.h"
#include "ClientSession.h"
#include "RoomManager.h"

void Room::EnterPlayer(const shared_ptr<ClientSession>& session)
{
	cout << session->m_name << "이 방[" << m_id << "]에 입장하심" << endl;
	m_lock.lock();
	clients.emplace_back(make_pair(session->m_id, session));
	session->m_room = shared_from_this();
	sort(clients.begin(), clients.end());
	m_lock.unlock();
}

void Room::LeavePlayer(const shared_ptr<ClientSession>& session)
{
	int id = session->m_id;
	m_lock.lock();
	if (binary_search(clients.begin(), clients.end(), id, Compare()) == false) {
		m_lock.unlock();
		cout << "player[" << id << "]는 원래 없던 사람임" << endl;
		return;
	}

	SC_LEAVE_PLAYER_PACKET p;
	p.size = sizeof(SC_LEAVE_PLAYER_PACKET);
	p.type = SC_LEAVE_PLAYER;
	p.id = id;
	strcpy_s(p.name, session->m_name.c_str());

	for (auto& client : clients) {
		if (client.first == id) continue;
		client.second->RegisterSend(&p, p.size);
	}

	//clients.erase(clients.begin() + id);
	clients.erase(remove_if(clients.begin(), clients.end(),
		[id](const pair<int, shared_ptr<ClientSession>>& pairSession) {return pairSession.first == id; }));
	if (clients.size() == 0) {
		m_lock.unlock();
		RoomManager::GetInstance()->RemoveRoom(m_id);
	}
	else
		m_lock.unlock();
	return;
}

void Room::BroadcastPacket(void* packet, size_t length, int sender)
{
	for (auto& c : clients) {
		if (c.first == sender) continue;
		c.second->RegisterSend(&packet, length);
	}
}

void Room::SendEnterRoomPacket(int id)
{
	SC_ENTER_ROOM_PACKET roomPacket;
	roomPacket.size = sizeof(SC_ENTER_ROOM_PACKET);
	roomPacket.type = SC_ENTER_ROOM;
	roomPacket.room_id = m_id;
	roomPacket.client_id = id;
	// 본인 입장 알림
	//auto newClient = find(clients.begin(), clients.end(), id);
	auto newClient = find_if(clients.begin(), clients.end(),
		[id](const pair<int, shared_ptr<ClientSession>>& client) {return client.first == id; });

	newClient->second->RegisterSend(&roomPacket, roomPacket.size);
	//clients[id]->RegisterSend(&roomPacket, roomPacket.size);

	// 이미 있는 애들한테 얘들어왓다고
	for (auto& client : clients) {
		if (client.first == id) continue;
		client.second->RegisterSend(&roomPacket, roomPacket.size);
	}

	// 새로 들어온 애한테 기존애들정보
	for (auto& client : clients) {
		if (client.first == id) continue;
		roomPacket.client_id = client.first;
		newClient->second->RegisterSend(&roomPacket, roomPacket.size);
	}
}

void Room::SendChatPacket(int sender, char* chat)
{
	// auto sendClient = find(clients.begin(), clients.end(), sender);
	auto sendClient = find_if(clients.begin(), clients.end(),
		[sender](const pair<int, shared_ptr<ClientSession>>& client) {return client.first == sender; });
	SC_CHAT_PACKET p;
	p.size = sizeof(SC_CHAT_PACKET);
	p.type = SC_CHAT;
	p.id = sender;
	strcpy_s(p.name, sendClient->second->m_name.c_str());
	strcpy_s(p.chat, chat);

	for (auto& client : clients) {
		client.second->RegisterSend(&p, p.size);
	}
}