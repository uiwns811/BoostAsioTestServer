#include "ClientSession.h"
#include "SessionManager.h"
#include "RoomManager.h"
#include "Room.h"

void ClientSession::ProcessConnect()
{
	wcout << "New Client Connected" << endl;
	RegisterRecv();
}

void ClientSession::ProcessDisconnect()
{
	SessionManager::GetInstance()->RemoveClient(m_id);
	shared_ptr<Room> room = m_room.lock();
	if (room != nullptr) {
		room->LeavePlayer(shared_from_this());
		room = nullptr;
	}
	m_socket.shutdown(m_socket.shutdown_both);
	m_socket.close();
}

void ClientSession::RegisterRecv()
{
	m_socket.async_read_some(boost::asio::buffer(data, BUF_SIZE), 
		boost::bind(&ClientSession::ProcessRecv, shared_from_this(), 
			boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
}

void ClientSession::ProcessRecv(const boost::system::error_code& ec, std::size_t length)
{
	if (ec)
	{
		if (ec.value() == boost::asio::error::operation_aborted) return;
		//if (nullptr == SessionManager::GetInstance()->FindClient(m_id)) return;
		cout << "Receive Error on Session[" << m_id << "] EC[" << ec << "]\n";	// error. 
		ProcessDisconnect();
		return;
	}
	ConstructData(data, length);
	RegisterRecv();
}

void ClientSession::ConstructData(unsigned char* buf, size_t io_byte)
{
	int data_to_process = static_cast<int>(io_byte);
	while (0 < data_to_process) {
		if (0 == cur_packet_size) {
			cur_packet_size = buf[0];
		}
		int need_to_build = cur_packet_size - prev_data_size;
		if (need_to_build <= data_to_process) {
			memcpy(packet + prev_data_size, buf, need_to_build);
			ProcessPacket(packet, m_id);
			cur_packet_size = 0;
			prev_data_size = 0;
			data_to_process -= need_to_build;
			buf += need_to_build;
		}
		else {
			memcpy(packet + prev_data_size, buf, data_to_process);
			prev_data_size += data_to_process;
			data_to_process = 0;
			buf += data_to_process;
		}
	}
}

void ClientSession::ProcessPacket(unsigned char* packet, int id)
{
	switch (packet[1]) {
	case CS_LOGIN:
	{
		CS_LOGIN_PACKET* pkt = reinterpret_cast<CS_LOGIN_PACKET*>(packet);
		wcout << pkt->name << "가 로그인 " << endl;
		m_name = pkt->name;

		SC_LOGIN_OK_PACKET loginPacket;
		loginPacket.size = sizeof(SC_LOGIN_OK_PACKET);
		loginPacket.type = SC_LOGIN_OK;

		SC_ENTER_LOBBY_PACKET enterPacket;
		enterPacket.size = sizeof(SC_ENTER_LOBBY_PACKET);
		enterPacket.type = SC_ENTER_LOBBY;
		enterPacket.id = id;
		wcscpy_s(enterPacket.name, m_name.c_str());

		// 들어온 애한테 로그인ㅇㅋ
		//SendPacket(&loginPacket, m_id);
		RegisterSend(&loginPacket, loginPacket.size);

		// Room Info
		SC_ROOM_INFO_PACKET roomPacket;
		memset(&roomPacket, NULL, sizeof(SC_ROOM_INFO_PACKET));
		roomPacket.size = sizeof(SC_ROOM_INFO_PACKET);
		roomPacket.type = SC_ROOM_INFO;
		vector<RoomInfo> curRoomInfo = RoomManager::GetInstance()->GetRoomInfo();
		for (int i = 0; i < curRoomInfo.size(); i++) {
			roomPacket.roomList[i] = curRoomInfo[i];
		}
		RegisterSend(&roomPacket, roomPacket.size);
		// 포인터로  안해도 되나?
	}
	break;   
	case CS_SELECT_ROOM :
	{
		CS_SELECT_ROOM_PACKET* p = reinterpret_cast<CS_SELECT_ROOM_PACKET*>(packet);
		int roomid = p->room_id;
		wcout << m_name<< "이 "<< roomid << "번 방을 선택함" << endl;
		if (roomid < 0 || roomid > MAX_ROOM_SIZE) {
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
			RegisterSend(&roomPacket, roomPacket.size);
		}
		else {
			RoomManager::GetInstance()->GetRoom(roomid)->EnterPlayer(shared_from_this());
			shared_ptr<Room> room = m_room.lock();
			if(room.use_count() != 0)
				room->SendEnterRoomPacket(id);
		}
	}
	break;
	case CS_CHAT:
	{
		CS_CHAT_PACKET* p = reinterpret_cast<CS_CHAT_PACKET*>(packet);
		shared_ptr<Room> room = m_room.lock();
		if(room.use_count() != 0)
			room->SendChatPacket(id, m_name.c_str(), p->chat);
	}
	break;
	}
}


void ClientSession::RegisterSend(void* packet, std::size_t length)
{
	auto self(shared_from_this());
	m_socket.async_write_some(boost::asio::buffer(packet, length),
		[this, self, packet, length](boost::system::error_code ec, std::size_t bytes_transferred)
		{
			if (!ec)
			{
				if (length != bytes_transferred) {
					wcout << "Incomplete Send occured on session[" << m_id << "]" << endl;
				}
				cout << "Send Complete" << endl;
			}
		});
}

void ClientSession::SendPacket(void* packet, unsigned id)
{
	int packet_size = reinterpret_cast<unsigned char*>(packet)[0];
	unsigned char* buff = new unsigned char[packet_size];
	memcpy(buff, packet, packet_size);
	SessionManager::GetInstance()->FindClient(id)->RegisterSend(buff, packet_size);
	//SessionManager::GetInstance()->FindClient(id)->RegisterSend(packet, packet_size);
}
