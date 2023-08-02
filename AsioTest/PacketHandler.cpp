#include "PacketHandler.h"
#include "ClientSession.h"
#include "RoomManager.h"
#include "Room.h"

void PacketHandler::CSLoginHandler(shared_ptr<ClientSession> session, unsigned char* packet)
{
	CS_LOGIN_PACKET* p = reinterpret_cast<CS_LOGIN_PACKET*>(packet);
	session->m_name = p->name;
	wcout << session->m_name << "가 로그인 " << endl;

	SC_LOGIN_OK_PACKET loginPacket;
	loginPacket.size = sizeof(SC_LOGIN_OK_PACKET);
	loginPacket.type = SC_LOGIN_OK;

	SC_ENTER_LOBBY_PACKET enterPacket;
	enterPacket.size = sizeof(SC_ENTER_LOBBY_PACKET);
	enterPacket.type = SC_ENTER_LOBBY;
	enterPacket.id = session->m_id;
	wcscpy_s(enterPacket.name, session->m_name.c_str());

	// 들어온 애한테 로그인ㅇㅋ
	session->SendPacket(&loginPacket);

	// Room Info
	SC_ROOM_INFO_PACKET roomPacket;
	memset(&roomPacket, NULL, sizeof(SC_ROOM_INFO_PACKET));
	roomPacket.size = sizeof(SC_ROOM_INFO_PACKET);
	roomPacket.type = SC_ROOM_INFO;
	vector<RoomInfo> curRoomInfo = RoomManager::GetInstance()->GetRoomInfo();
	for (int i = 0; i < curRoomInfo.size(); i++) {
		roomPacket.roomList[i] = curRoomInfo[i];
	}
	session->SendPacket(&roomPacket);
}

void PacketHandler::CSLeaveHandler(shared_ptr<ClientSession> session, CS_LEAVE_PLAYER_PACKET& packet)
{
	cout << "Leave Handler" << endl;
}

void PacketHandler::CSSelectRoomHandler(shared_ptr<ClientSession> session, CS_SELECT_ROOM_PACKET& packet)
{
	int roomid = packet.room_id;
	
	wcout << session->m_name << "이 " << roomid << "번 방을 선택함" << endl;
	if (roomid < 1 || roomid > MAX_ROOM_SIZE) {
		cout << "유효하지 않은 방 - 다시 선택하십숑" << endl;
		// Room Info
		SC_ROOM_INFO_PACKET roomPacket;
		memset(&roomPacket, NULL, sizeof(SC_ROOM_INFO_PACKET));
		roomPacket.size = sizeof(SC_ROOM_INFO_PACKET);
		roomPacket.type = SC_ROOM_INFO;
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

void PacketHandler::CSChatHandler(shared_ptr<ClientSession> session, CS_CHAT_PACKET& packet)
{
	shared_ptr<Room> room = session->m_room.lock();
	if (room.use_count() != 0)
		room->SendChatPacket(session->m_id, session->m_name.c_str(), packet.chat);
}