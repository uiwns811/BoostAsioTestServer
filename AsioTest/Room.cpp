#include "Room.h"
#include "ClientSession.h"
#include "RoomManager.h"

void Room::EnterPlayer(const shared_ptr<ClientSession>& session)
{
	wcout << session->m_name << "�� ��[" << m_id << "]�� �����Ͻ�" << endl;
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
		cout << "player[" << id << "]�� ���� ���� �����" << endl;
		return;
	}

	SC_LEAVE_PLAYER_PACKET p;
	p.size = sizeof(SC_LEAVE_PLAYER_PACKET);
	p.type = PacketType::SC_LEAVE_PLAYER;
	p.id = id;
	wcscpy_s(p.name, session->m_name.c_str());

	for (auto& client : clients) {
		if (client.first == id) continue;
		client.second->SendPacket(&p);
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

void Room::SendEnterRoomPacket(int id)
{
	SC_ENTER_ROOM_PACKET roomPacket;
	roomPacket.size = sizeof(SC_ENTER_ROOM_PACKET);
	roomPacket.type = PacketType::SC_ENTER_ROOM;
	roomPacket.room_id = m_id;
	roomPacket.client_id = id;
	// ���� ���� �˸�

	auto newClient = find_if(clients.begin(), clients.end(),
		[id](const pair<int, shared_ptr<ClientSession>>& client) {return client.first == id; });

	newClient->second->SendPacket(&roomPacket);
	
	// �̹� �ִ� �ֵ����� ����Ӵٰ�
	for (auto& client : clients) {
		if (client.first == id) continue;
		client.second->SendPacket(&roomPacket);
	}

	// ���� ���� ������ �����ֵ�����
	for (auto& client : clients) {
		if (client.first == id) continue;
		roomPacket.client_id = client.first;
		newClient->second->SendPacket(&roomPacket);
	}
}

void Room::SendChatPacket(int sender, const wchar_t* name, const wchar_t* chat)
{
	//auto sendClient = find_if(clients.begin(), clients.end(),
	//	[sender](const pair<int, shared_ptr<ClientSession>>& client) {return client.first == sender; });
	SC_CHAT_PACKET chatPacket;
	chatPacket.size = sizeof(SC_CHAT_PACKET);
	chatPacket.type = PacketType::SC_CHAT;
	chatPacket.id = sender;
	wcscpy_s(chatPacket.name, name);
	wcscpy_s(chatPacket.chat, chat);

	for (auto& client : clients) {
		client.second->SendPacket(&chatPacket);
	}
}