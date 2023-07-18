#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include "../AsioTest/protocol.h"

using namespace std;
using boost::asio::ip::tcp;

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

	string m_name;

public:
	Client(boost::asio::io_context& io_context, const tcp::endpoint& endpoints) : socket(io_context), endpoint(endpoints)
	{
		cout << "ID 입력 : ";
		cin >> m_name;
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
				cout << "connect failed : " << ec.message() << endl;
		});
	}

	void OnConnected()
	{
		cout << "Connected to server" << endl;

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
						if (buf[0] > 200) {   
							cout << "Invalid Packet Size [ << buf[0] << ] Terminating Server!\n";
							exit(-1);
						}
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
			cout << "로그인 완료!" << endl;
		}
		break;
		case SC_ENTER_PLAYER:
		{
			SC_ENTER_PLAYER_PACKET* p = reinterpret_cast<SC_ENTER_PLAYER_PACKET*>(packet);
			cout << "New Client [" << p->name << "] enter!" << endl;
		}
		break;
		case SC_CHAT:
		{
			SC_CHAT_PACKET* p = reinterpret_cast<SC_CHAT_PACKET*>(packet);
			cout << "[CHAT] " << p->name << " : " << p->chat << endl;
		}
		break;
		}
	}

	void SendLoginPacket(string name)
	{
		CS_LOGIN_PACKET p;
		p.size = sizeof(CS_LOGIN_PACKET);
		p.type = CS_LOGIN;
		strcpy_s(p.name, name.c_str());
		Write(&p, p.size);
	}

	void SendChatPacket(string chat)
	{
		CS_CHAT_PACKET p;
		p.size = sizeof(CS_CHAT_PACKET);
		p.type = CS_CHAT;
		strcpy_s(p.chat, chat.c_str());
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
		string msg;
		getline(cin, msg);
		client.SendChatPacket(msg);
	}

	th->join();
	delete th;

	cout << "종료" << endl;
	return 0;
}