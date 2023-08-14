#include "PacketManager.h"
#include "ServerSession.h"

void PacketManager::Init(shared_ptr<ServerSession> session)
{
	m_session = session->shared_from_this();
	BindHandler(PacketType::SC_LOGIN_OK, &PacketManager::SCLoginHandler);
	BindHandler(PacketType::SC_ENTER_LOBBY, &PacketManager::SCEnterLobbyHandler);
	BindHandler(PacketType::SC_LEAVE_PLAYER, &PacketManager::SCLeavePlayerHandler);
	BindHandler(PacketType::SC_ROOM_INFO, &PacketManager::SCRoomInfoHandler);
	BindHandler(PacketType::SC_ENTER_ROOM, &PacketManager::SCEnterRoomHandler);
	BindHandler(PacketType::SC_CHAT, &PacketManager::SCChatHandler);
}

void PacketManager::BindHandler(PacketType type, void(PacketManager::* handler)(unsigned char*))
{
	m_handlers[type] = [this, handler](unsigned char* packet)
	{
		(this->*handler)(packet);
	};
}

void PacketManager::Enqueue(unsigned char* packet, size_t length)
{
	int data_to_process = static_cast<int>(length) + prev_data_size;
	while (data_to_process > 0) {
		int packet_size = packet[0];
		if (packet_size > data_to_process) break;

		PacketType packet_type = (PacketType)packet[1];
		m_handlers[packet_type](packet);

		packet += packet_size;
		data_to_process -= packet_size;
	}
	prev_data_size = data_to_process;
	if (prev_data_size > 0) {
		memcpy(remainData, packet, prev_data_size);
	}
}

void PacketManager::SCLoginHandler(unsigned char* packet)
{
	wcout << "로그인 완료!" << endl;
}

void PacketManager::SCEnterLobbyHandler(unsigned char* packet)
{
	SC_ENTER_LOBBY_PACKET* p = reinterpret_cast<SC_ENTER_LOBBY_PACKET*>(packet);
	wcout << "New Client [" << p->name << "] Connected!" << endl;
}

void PacketManager::SCLeavePlayerHandler(unsigned char* packet)
{
	SC_LEAVE_PLAYER_PACKET* p = reinterpret_cast<SC_LEAVE_PLAYER_PACKET*>(packet);
	wcout << "player [" << p->name << "] 접속 종료" << endl;
}

void PacketManager::SCRoomInfoHandler(unsigned char* packet)
{
	SC_ROOM_INFO_PACKET* p = reinterpret_cast<SC_ROOM_INFO_PACKET*>(packet);
	for (auto& info : p->roomList) {
		if (info.cur_user_cnt == 0) continue;
		wcout << "Room[" << info.room_id << "] - 현재 " << info.cur_user_cnt << "명 접속 중" << endl;
	}
	int input = 0;
	wcout << "입장을 원하는 방 번호(1 ~ 10)를 입력 (방 없으면 생성함) : ";
	while (!(cin >> input)) {
		cout << "숫자로 다시 입력하세요.." << endl;
		cin.clear();
		cin.ignore(1000, '\n');
	}

	CS_SELECT_ROOM_PACKET pkt;
	pkt.size = sizeof(CS_SELECT_ROOM_PACKET);
	pkt.type = PacketType::CS_SELECT_ROOM;
	pkt.room_id = input;
	m_session->RegisterSend(&pkt, pkt.size);
}

void PacketManager::SCEnterRoomHandler(unsigned char* packet)
{
	SC_ENTER_ROOM_PACKET* p = reinterpret_cast<SC_ENTER_ROOM_PACKET*>(packet);
	wcout << "player [" << p->client_id << "]가 Room[" << p->room_id << "]에 입장하였습니다" << endl;
	m_session->SetChat(true);
}

void PacketManager::SCChatHandler(unsigned char* packet)
{
	SC_CHAT_PACKET* p = reinterpret_cast<SC_CHAT_PACKET*>(packet);
	wcout << "[" << p->name << "] : " << p->chat << endl;
}