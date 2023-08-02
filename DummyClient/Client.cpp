#include "Client.h"

void Client::Connect()
{
	m_socket.async_connect(endpoint, [this](boost::system::error_code ec) {
		if (!ec) {
			OnConnected();
		}
		else
			std::cout << "connect failed : " << ec.message() << endl;
		});
}

void Client::OnConnected()
{
	wcout << "Connected to server" << endl;

	SendLoginPacket(m_name);

	RegisterRecv();
}

void Client::RegisterSend(void* packet, std::size_t length)
{
	unsigned char* buff = new unsigned char[length];
	memcpy(buff, packet, length);

	m_socket.async_write_some(boost::asio::buffer(buff, length),
		[this, buff, length](boost::system::error_code ec, std::size_t bytes_transferred)
		{
			if (!ec)
			{
				delete buff;
			}
		});
}

void Client::RegisterRecv()
{
	m_socket.async_read_some(boost::asio::buffer(recvbuff, BUF_SIZE),
		[this](boost::system::error_code ec, std::size_t length) {
			ProcessRecv(ec, length);
		});
}

void Client::ProcessRecv(const boost::system::error_code& ec, std::size_t length)
{
	//m_socket.async_read_some(boost::asio::buffer(data, BUF_SIZE),
	//	[this](boost::system::error_code ec, std::size_t length) {
	//		int data_to_process = static_cast<int>(length);
	//		unsigned char* buf = data;
	//		while (0 < data_to_process) {
	//			if (0 == cur_packet_size) {
	//				cur_packet_size = buf[0];
	//			}
	//			int need_to_build = cur_packet_size - prev_data_size;
	//			if (need_to_build <= data_to_process) {
	//				memcpy(packet + prev_data_size, buf, need_to_build);
	//				ProcessPacket(packet);
	//				cur_packet_size = 0;
	//				prev_data_size = 0;
	//				data_to_process -= need_to_build;
	//				buf += need_to_build;
	//			}
	//			else {
	//				// 훗날을 기약
	//				memcpy(packet + prev_data_size, buf, data_to_process);
	//				prev_data_size += data_to_process;
	//				data_to_process = 0;
	//				buf += data_to_process;
	//			}
	//		}
	//		ProcessRecv();
	//	});

	ConstructData(recvbuff, length);
	RegisterRecv();
}

void Client::ConstructData(unsigned char* buf, size_t io_byte)
{
	int data_to_process = static_cast<int>(io_byte) + prev_data_size;
	while (data_to_process > 0) {
		int packet_size = buf[0];
		if (packet_size > data_to_process) break;

		//PacketHeader* header = reinterpret_cast<PacketHeader*>(buf);
		//if (header->size > data_to_process) break;
		
		//memcpy(remainData, buf, packet_size);
		ProcessPacket(buf);
		buf += packet_size;
		data_to_process -= packet_size;
	}
	prev_data_size = data_to_process;
	if (prev_data_size > 0) {
		memcpy(remainData, buf, prev_data_size);
	}
}

void Client::ProcessPacket(unsigned char* packet)
{
	//PacketHeader* header = reinterpret_cast<PacketHeader*>(packet);
	//packet += header->size;
	PacketType type = (PacketType)packet[1];
	switch (type)
	{
	case PacketType::SC_LOGIN_OK:
	{
		wcout << "로그인 완료!" << endl;
	}
	break;
	case PacketType::SC_ENTER_LOBBY:
	{
		SC_ENTER_LOBBY_PACKET* p = reinterpret_cast<SC_ENTER_LOBBY_PACKET*>(packet);
		wcout << "New Client [" << p->name << "] Connected!" << endl;
	}
	break;
	case PacketType::SC_LEAVE_PLAYER:
	{
		SC_LEAVE_PLAYER_PACKET* p = reinterpret_cast<SC_LEAVE_PLAYER_PACKET*>(packet);
		wcout << "player [" << p->name << "] 접속 종료" << endl;
	}
	break;
	case PacketType::SC_ROOM_INFO:
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
		RegisterSend(&pkt, pkt.size);
	}
	break;
	case PacketType::SC_ENTER_ROOM:
	{
		SC_ENTER_ROOM_PACKET* p = reinterpret_cast<SC_ENTER_ROOM_PACKET*>(packet);
		wcout << "player [" << p->client_id << "]가 Room[" << p->room_id << "]에 입장하였습니다" << endl;
		isChatting = true;
	}
	break;
	case PacketType::SC_CHAT:
	{
		SC_CHAT_PACKET* p = reinterpret_cast<SC_CHAT_PACKET*>(packet);
		wcout << "[" << p->name << "] : " << p->chat << endl;
	}
	break;
	}
}

void Client::SendLoginPacket(wstring name)
{
	CS_LOGIN_PACKET p;
	p.size = sizeof(CS_LOGIN_PACKET);
	p.type = PacketType::CS_LOGIN;
	wcscpy_s(p.name, name.c_str());
	RegisterSend(&p, p.size);
}

void Client::SendChatPacket(wstring chat)
{
	CS_CHAT_PACKET p;
	p.size = sizeof(CS_CHAT_PACKET);
	p.type = PacketType::CS_CHAT;
	wcscpy_s(p.chat, chat.c_str());
	RegisterSend(&p, p.size);
}