#include "ServerSession.h"

void ServerSession::ProcessConnect()
{
	wcout << "Connected to server" << endl;

	wcout << "ID 입력 : ";
	wcin >> m_name;

	SendLoginPacket(m_name);

	RegisterRecv();
}

void ServerSession::RegisterSend(void* packet, std::size_t length)
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

void ServerSession::RegisterRecv()
{
	m_socket.async_read_some(boost::asio::buffer(recvbuff, BUF_SIZE),
		[this](boost::system::error_code ec, std::size_t length) {
			ProcessRecv(ec, length);
		});
}

void ServerSession::ProcessRecv(const boost::system::error_code& ec, std::size_t length)
{
	ConstructData(recvbuff, length);
	RegisterRecv();
}

void ServerSession::ConstructData(unsigned char* buf, size_t io_byte)
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

void ServerSession::ProcessPacket(unsigned char* packet)
{
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
		m_bChat = true;
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

void ServerSession::SendLoginPacket(wstring name)
{
	CS_LOGIN_PACKET p;
	p.size = sizeof(CS_LOGIN_PACKET);
	p.type = PacketType::CS_LOGIN;
	wcscpy_s(p.name, name.c_str());
	RegisterSend(&p, p.size);
}

void ServerSession::SendChatPacket(wstring chat)
{
	CS_CHAT_PACKET p;
	p.size = sizeof(CS_CHAT_PACKET);
	p.type = PacketType::CS_CHAT;
	wcscpy_s(p.chat, chat.c_str());
	RegisterSend(&p, p.size);
}