#include "PacketManager.h"
#include "SessionManager.h"
#include "PacketHandler.h"
#include "ClientSession.h"
#include "RoomManager.h"
#include "Room.h"

void PacketManager::Init()
{
	BindHandler(PacketType::CS_LOGIN, &PacketManager::CSLoginHandler);
	BindHandler(PacketType::CS_LEAVE_PLAYER, &PacketManager::CSLeaveHandler);
	BindHandler(PacketType::CS_SELECT_ROOM, &PacketManager::CSSelectRoomHandler);
	BindHandler(PacketType::CS_CHAT, &PacketManager::CSChatHandler);
}

void PacketManager::BindHandler(PacketType type, void(PacketManager::* handler)(shared_ptr<ClientSession>, unsigned char*))
{
	m_handlers[type] = [this, handler](shared_ptr<ClientSession> session, unsigned char* packet)
	{
		(this->*handler)(session, packet);
	};
}

void PacketManager::Enqueue(shared_ptr<ClientSession> session, unsigned char* packet, size_t length)
{
	int data_to_process = static_cast<int>(length) + prev_data_size;
	while (data_to_process > 0) {
		int packet_size = packet[0];
		if (packet_size > data_to_process) break;

		PacketType packet_type = (PacketType)packet[1];
		m_handlers[packet_type](session, packet);

		packet += packet_size;
		data_to_process -= packet_size;
	}
	prev_data_size = data_to_process;
	if (prev_data_size > 0) {
		memcpy(remainData, packet, prev_data_size);
	}
}

void PacketManager::CSLoginHandler(shared_ptr<ClientSession> session, unsigned char* packet)
{
	CS_LOGIN_PACKET* p = reinterpret_cast<CS_LOGIN_PACKET*>(packet);
	session->m_name = p->name;
	wcout << session->m_name << "가 로그인 " << endl;

	SC_LOGIN_OK_PACKET loginPacket;
	loginPacket.size = sizeof(SC_LOGIN_OK_PACKET);
	loginPacket.type = PacketType::SC_LOGIN_OK;

	SC_ENTER_LOBBY_PACKET enterPacket;
	enterPacket.size = sizeof(SC_ENTER_LOBBY_PACKET);
	enterPacket.type = PacketType::SC_ENTER_LOBBY;
	enterPacket.id = session->m_id;
	wcscpy_s(enterPacket.name, session->m_name.c_str());

	// 들어온 애한테 로그인ㅇㅋ
	session->SendPacket(&loginPacket);

	// Room Info
	SC_ROOM_INFO_PACKET roomPacket;
	memset(&roomPacket, NULL, sizeof(SC_ROOM_INFO_PACKET));
	roomPacket.size = sizeof(SC_ROOM_INFO_PACKET);
	roomPacket.type = PacketType::SC_ROOM_INFO;
	vector<RoomInfo> curRoomInfo = RoomManager::GetInstance()->GetRoomInfo();
	for (int i = 0; i < curRoomInfo.size(); i++) {
		roomPacket.roomList[i] = curRoomInfo[i];
	}
	session->SendPacket(&roomPacket);
}


void PacketManager::CSLeaveHandler(shared_ptr<ClientSession> session, unsigned char* packet)
{
	cout << "Leave Handler" << endl;
}

void PacketManager::CSSelectRoomHandler(shared_ptr<ClientSession> session, unsigned char* packet)
{
	CS_SELECT_ROOM_PACKET* p = reinterpret_cast<CS_SELECT_ROOM_PACKET*>(packet);
	int roomid = p->room_id;

	wcout << session->m_name << "이 " << roomid << "번 방을 선택함" << endl;
	if (roomid < 1 || roomid > MAX_ROOM_SIZE) {
		cout << "유효하지 않은 방 - 다시 선택하십숑" << endl;
		// Room Info
		SC_ROOM_INFO_PACKET roomPacket;
		memset(&roomPacket, NULL, sizeof(SC_ROOM_INFO_PACKET));
		roomPacket.size = sizeof(SC_ROOM_INFO_PACKET);
		roomPacket.type = PacketType::SC_ROOM_INFO;
		vector<RoomInfo> curRoomInfo = RoomManager::GetInstance()->GetRoomInfo();
		for (int i = 0; i < curRoomInfo.size(); i++) {
			roomPacket.roomList[i] = curRoomInfo[i];
		}
		session->SendPacket(&roomPacket);
	}
	else {
		RoomManager::GetInstance()->GetRoom(roomid)->EnterPlayer(session->shared_from_this());
		shared_ptr<Room> room = session->m_room.lock();
		if (room.use_count() != 0)
			room->SendEnterRoomPacket(session->m_id);
	}
}

void PacketManager::CSChatHandler(shared_ptr<ClientSession> session, unsigned char* packet)
{
	CS_CHAT_PACKET* p = reinterpret_cast<CS_CHAT_PACKET*>(packet);
	shared_ptr<Room> room = session->m_room.lock();
	if (room.use_count() != 0)
		room->SendChatPacket(session->m_id, session->m_name.c_str(), p->chat);
}