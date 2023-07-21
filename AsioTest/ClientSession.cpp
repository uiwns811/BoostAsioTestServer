#include "ClientSession.h"
#include "SessionManager.h"
#include "RoomManager.h"
#include "Room.h"

void ClientSession::ProcessConnect()
{
	cout << "New Client Connected" << endl;
	m_id = SessionManager::GetInstance()->AddClient(shared_from_this());
	RegisterRecv();

	// 룸 로직 처리
	//shared_ptr<Room> room = make_shared<Room>();
	//room->m_id = RoomManager::GetInstance()->CreateRoom(room);
	//room->EnterPlayer(shared_from_this());
	RoomManager::GetInstance()->CreateRoom();
}

void ClientSession::ProcessDisconnect()
{
	SessionManager::GetInstance()->RemoveClient(m_id);
	if (m_room != nullptr) {
		m_room->LeavePlayer(shared_from_this());
	}
	m_socket.shutdown(m_socket.shutdown_both);
	m_socket.close();
}

void ClientSession::ProcessSend(const boost::system::error_code& ec, std::size_t length)
{
	//if (length != bytes_transferred)
	//	cout << "Incomplete Send occured on session[" << m_id << "]. This session should be closed.\n";
	//delete packet;
}

void ClientSession::ProcessRecv(const boost::system::error_code& ec, std::size_t length)
{
	if (ec)
	{
		if (ec.value() == boost::asio::error::operation_aborted) return;
		if (nullptr == SessionManager::GetInstance()->FindClient(m_id)) return;
		cout << "Receive Error on Session[" << m_id << "] EC[" << ec << "]\n";	// error. 
		ProcessDisconnect();
		return;
	}
	ConstructData(data, length);
	RegisterRecv();
}

void ClientSession::RegisterRecv()
{
	m_socket.async_read_some(boost::asio::buffer(data, BUF_SIZE), 
		boost::bind(&ClientSession::ProcessRecv, shared_from_this(), 
			boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
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
		cout << pkt->name << "가 로그인 " << endl;
		m_name = pkt->name;

		SC_LOGIN_OK_PACKET loginPacket;
		loginPacket.size = sizeof(SC_LOGIN_OK_PACKET);
		loginPacket.type = SC_LOGIN_OK;

		SC_ENTER_LOBBY_PACKET enterPacket;
		enterPacket.size = sizeof(SC_ENTER_LOBBY_PACKET);
		enterPacket.type = SC_ENTER_LOBBY;
		enterPacket.id = id;
		strcpy_s(enterPacket.name, m_name.c_str());

		// 들어온 애한테 로그인ㅇㅋ
		//SendPacket(&loginPacket, m_id);
		RegisterSend(&loginPacket, loginPacket.size);

		m_lock.lock();
		unordered_map<int, shared_ptr<ClientSession>> curClient = SessionManager::GetInstance()->GetClients();
		m_lock.unlock();

		// 기존 애들한테 새로 드러온 애 정보
		for (auto& client : curClient) {
			if (client.first == id) continue;
			enterPacket.id = id;
			strcpy_s(enterPacket.name, m_name.c_str());
			//SendPacket(&enterPacket, client.first);
			client.second->RegisterSend(&enterPacket, enterPacket.size);
			enterPacket.id = client.first;
			strcpy_s(enterPacket.name, client.second->m_name.c_str());
			//SendPacket(&enterPacket, id);
			RegisterSend(&enterPacket, enterPacket.size);
		}

		// Room Info
		SC_ROOM_INFO_PACKET roomPacket;
		roomPacket.size = sizeof(SC_ROOM_INFO_PACKET);
		roomPacket.type = SC_ROOM_INFO;
		vector<RoomInfo> curRoomInfo = RoomManager::GetInstance()->GetRoomInfo();
		for (int i = 0; i < curRoomInfo.size(); i++) {
			roomPacket.roomList[i] = curRoomInfo[i];
		}
		RegisterSend(&roomPacket, roomPacket.size);
	}
	break;   
	case CS_SELECT_ROOM :
	{
		CS_SELECT_ROOM_PACKET* p = reinterpret_cast<CS_SELECT_ROOM_PACKET*>(packet);
		cout << m_name<< "이 "<< p->room_id << "번 방을 선택함" << endl;

		RoomManager::GetInstance()->GetRoom(p->room_id)->EnterPlayer(shared_from_this());
		
		m_room->SendEnterRoomPacket(id);
	}
	break;
	case CS_CHAT:
	{
		CS_CHAT_PACKET* p = reinterpret_cast<CS_CHAT_PACKET*>(packet);
		//m_lock.lock();
		//unordered_map<int, shared_ptr<ClientSession>> curClient = SessionManager::GetInstance()->GetClients();
		//m_lock.unlock();
		//for (auto& client : curClient) {
		//	//SendPacket(&p, client.first);
		//	client.second->RegisterSend(&p, p.size);
		//}

		m_room->SendChatPacket(id, p->chat);
	}
	break;
	}
}


void ClientSession::RegisterSend(void* packet, std::size_t length)
{
	m_socket.async_write_some(boost::asio::buffer(packet, length), 
		boost::bind(&ClientSession::ProcessSend, shared_from_this(), 
			boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
}

void ClientSession::SendPacket(void* packet, unsigned id)
{
	int packet_size = reinterpret_cast<unsigned char*>(packet)[0];
	unsigned char* buff = new unsigned char[packet_size];
	memcpy(buff, packet, packet_size);
	SessionManager::GetInstance()->FindClient(id)->RegisterSend(buff, packet_size);
	//SessionManager::GetInstance()->FindClient(id)->RegisterSend(packet, packet_size);
}
