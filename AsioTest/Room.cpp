#include "Room.h"
#include "ClientSession.h"

void Room::EnterPlayer(const shared_ptr<ClientSession>& session)
{
	cout << session->m_name << "이 방[" << m_id << "]에 입장하심" << endl;
	m_lock.lock();
	clients[session->m_id] = session;
	session->m_room = shared_from_this();
	m_lock.unlock();

	for (auto& cl : clients) {
		cout << cl.first << endl;
	}
}

void Room::LeavePlayer(const shared_ptr<ClientSession>& session)
{
	m_lock.lock();
	for (auto& client : clients) {
		if (client.first == session->m_id) {
			clients.erase(client.first);
			m_lock.unlock();
			return;
		}
	}
	m_lock.unlock();
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
	clients[id]->RegisterSend(&roomPacket, roomPacket.size);

	// 이미 있는 애들한테 얘들어왓다고
	for (auto& client : clients) {
		if (client.first == id) continue;
		client.second->RegisterSend(&roomPacket, roomPacket.size);
	}

	// 새로 들어온 애한테 기존애들정보
	for (auto& client : clients) {
		if (client.first == id) continue;
		roomPacket.client_id = client.first;
		clients[id]->RegisterSend(&roomPacket, roomPacket.size);
	}
}

void Room::SendChatPacket(int sender, char* chat)
{
	SC_CHAT_PACKET p;
	p.size = sizeof(SC_CHAT_PACKET);
	p.type = SC_CHAT;
	p.id = sender;
	strcpy_s(p.name, clients[sender]->m_name.c_str());
	strcpy_s(p.chat, chat);

	for (auto& client : clients) {
		client.second->RegisterSend(&p, p.size);
	}
}