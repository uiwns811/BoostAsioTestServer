#include <iostream>
#include <string>
#include <cstring>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include "../AsioTest/protocol.h"

using namespace std;
using boost::asio::ip::tcp;

bool isChatting = false;

class Client
{
private:
	tcp::socket socket;
	boost::asio::io_context io_context;
	tcp::endpoint endpoint;
	unsigned char data[BUF_SIZE];
	unsigned char packet[BUF_SIZE];
	int cur_packet_size;
	int prev_data_size;

	wstring m_name;

public:
	Client(boost::asio::io_context& io_context, const tcp::endpoint& endpoints) : socket(io_context), endpoint(endpoints)
	{
		wcout << "ID 입력 : ";
		wcin >> m_name;
		Connect();
	}

	Client(boost::asio::io_context& io_context) : socket(io_context) 
	{

	}

	void Connect()
	{
		socket.async_connect(endpoint, [this](boost::system::error_code ec) {
			if (!ec) {
				OnConnected();
			}
			else
				std::cout << "connect failed : " << ec.message() << endl;
		});
	}

	void OnConnected()
	{
		wcout << "Connected to server" << endl;

		SendLoginPacket(m_name);

		ProcessRead();
	}

	void Write(void* packet, std::size_t length)
	{
		socket.async_write_some(boost::asio::buffer(packet, length),
			[this, packet, length](boost::system::error_code ec, std::size_t bytes_transferred)
			{
				if (!ec)
				{
					//delete packet;
				}
			});
	}

	void ProcessRead()
	{
		socket.async_read_some(boost::asio::buffer(data, BUF_SIZE),
			[this](boost::system::error_code ec, std::size_t length) {
				int data_to_process = static_cast<int>(length);
				unsigned char* buf = data;
				while (0 < data_to_process) {
					if (0 == cur_packet_size) {
						cur_packet_size = buf[0];
					}
					int need_to_build = cur_packet_size - prev_data_size;
					if (need_to_build <= data_to_process) {
						memcpy(packet + prev_data_size, buf, need_to_build);
						ProcessPacket(packet);
						cur_packet_size = 0;
						prev_data_size = 0;
						data_to_process -= need_to_build;
						buf += need_to_build;
					}
					else {
						// 훗날을 기약
						memcpy(packet + prev_data_size, buf, data_to_process);
						prev_data_size += data_to_process;
						data_to_process = 0;
						buf += data_to_process;
					}
				}
				ProcessRead();
			});
	}

	void ProcessData(unsigned char* buf, size_t io_byte) 
	{
		unsigned char* ptr = buf;
		while (0 < io_byte) {
			if (0 == cur_packet_size) cur_packet_size = ptr[0];
			int need_to_build_size = cur_packet_size - prev_data_size;
			if (need_to_build_size <= io_byte) {
				memcpy(packet + prev_data_size, ptr, need_to_build_size);
				ProcessPacket(packet);
				cur_packet_size = 0;
				prev_data_size = 0;
				io_byte -= need_to_build_size;
				ptr += need_to_build_size;
			}
			else {
				memcpy(packet + prev_data_size, ptr, io_byte);
				prev_data_size += io_byte;
				io_byte = 0;
				ptr += io_byte;
			}
		}
	}
	
	void ProcessPacket(unsigned char* packet)
	{
		switch (packet[1])
		{
		case SC_LOGIN_OK:
		{
			wcout << "로그인 완료!" << endl;
		}
		break;
		case SC_ENTER_LOBBY:
		{
			SC_ENTER_LOBBY_PACKET* p = reinterpret_cast<SC_ENTER_LOBBY_PACKET*>(packet);
			wcout << "New Client [" << p->name << "] Connected!" << endl;
		}
		break;
		case SC_LEAVE_PLAYER:
		{
			SC_LEAVE_PLAYER_PACKET* p = reinterpret_cast<SC_LEAVE_PLAYER_PACKET*>(packet);
			wcout << "player [" << p->name << "] 접속 종료" << endl;
		}
		break;
		case SC_ROOM_INFO:
		{
			SC_ROOM_INFO_PACKET* p = reinterpret_cast<SC_ROOM_INFO_PACKET*>(packet);
			for (auto& info : p->roomList) {
				if (info.cur_user_cnt == 0) continue;
				wcout << "Room[" << info.room_id << "] - 현재 " << info.cur_user_cnt << "명 접속 중" << endl;
			}
			wcout << "입장을 원하는 방 번호를 입력 (방 없으면 생성함) : ";
			int input;
			cin >> input;
			CS_SELECT_ROOM_PACKET pkt;
			pkt.size = sizeof(CS_SELECT_ROOM_PACKET);
			pkt.type = CS_SELECT_ROOM;
			pkt.room_id = input;
			Write(&pkt, pkt.size);
		}
		break;
		case SC_ENTER_ROOM:
		{
			SC_ENTER_ROOM_PACKET* p = reinterpret_cast<SC_ENTER_ROOM_PACKET*>(packet);
			wcout << "player [" << p->client_id << "]가 Room[" << p->room_id << "]에 입장하였습니다" << endl;
			isChatting = true;
		}
		break;
		case SC_CHAT:
		{
			SC_CHAT_PACKET* p = reinterpret_cast<SC_CHAT_PACKET*>(packet);
			wcout << "[" << p->name << "] : " << p->chat << endl;
		}
		break;
		}
	}

	void SendLoginPacket(wstring name)
	{
		CS_LOGIN_PACKET p;
		p.size = sizeof(CS_LOGIN_PACKET);
		p.type = CS_LOGIN;
		wcscpy_s(p.name, name.c_str());
		Write(&p, p.size);
	}

	void SendChatPacket(wstring chat)
	{
		CS_CHAT_PACKET p;
		p.size = sizeof(CS_CHAT_PACKET);
		p.type = CS_CHAT;
		wcscpy_s(p.chat, chat.c_str());
		Write(&p, p.size);
	}
};

void worker_thread(boost::asio::io_context* io_context)
{
	try {
		io_context->run();
	}
	catch (std::exception& e) {
		std::cerr << "Exception: " << e.what() << std::endl;
	}
}

int main()
{
	boost::asio::io_context io_context;
	boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::address::from_string("127.0.0.1"), SERVER_PORT);

	Client client(io_context, endpoint);

	boost::thread* th = new boost::thread(worker_thread, &io_context);

	while (true) {
		if (isChatting) {
			wstring msg;
			getline(wcin, msg);
			if (0 != wcscmp(msg.c_str(), L"\0")) {
				client.SendChatPacket(msg);
			}
		}
	}

	th->join();
	delete th;

	wcout << "종료" << endl;
	return 0;
}