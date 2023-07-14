#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include "protocol.h"

using namespace std;
using boost::asio::ip::tcp;

atomic_int g_user_ID;

int GetNewClientID()
{
	if (g_user_ID >= MAX_USER) {
		cout << "MAX USER FULL\n";
		exit(-1);
	}
	return g_user_ID++;
}

struct Player {
	bool connected;
	chrono::system_clock::time_point start_time;
	class Session* session;
	string name;
};

Player players[MAX_USER];

class Session 
	: public std::enable_shared_from_this<Session>
{
private:
	tcp::socket socket_;
	unsigned char data[BUF_SIZE];
	unsigned char packet[BUF_SIZE];
	int cur_packet_size;
	int prev_data_size;
	int my_id;

	std::shared_ptr<Session> get_shared_ptr() { return shared_from_this(); }

	void SendPacket(void* packet, unsigned id)
	{
		int packet_size = reinterpret_cast<unsigned char*>(packet)[0];
		unsigned char* buff = new unsigned char[packet_size];
		memcpy(buff, packet, packet_size);
		players[id].session->Write(buff, packet_size);
	}

	void ProcessPacket(unsigned char* packet, int id)
	{
		switch (packet[1]) {
		case CS_LOGIN: 
		{
			cout << "Recv Login Packet" << endl;
			CS_LOGIN_PACKET* pkt = reinterpret_cast<CS_LOGIN_PACKET*>(packet);
			cout << pkt->name << "- login " << endl;
			players[id].name = pkt->name;

			SC_LOGIN_OK_PACKET loginPacket;
			loginPacket.size = sizeof(SC_LOGIN_OK_PACKET);
			loginPacket.type = SC_LOGIN_OK;

			SC_ENTER_PLAYER_PACKET enterPacket;
			enterPacket.size = sizeof(SC_ENTER_PLAYER_PACKET);
			enterPacket.type = SC_ENTER_PLAYER;
			enterPacket.id = id;

			// 들어온 애한테 로그인ㅇㅋ
			players[id].session->Write(&loginPacket, loginPacket.size);

			// 기존 애들한테 새로 드러온 애 정보
			for (auto& player : players) {
				if (!player.connected)  continue;
				if (player.session->my_id == id) continue;
				enterPacket.id = id;
				player.session->Write(&enterPacket, enterPacket.size);
				enterPacket.id = player.session->my_id;
				players[id].session->Write(&enterPacket, enterPacket.size);
			}
		}
		break;
		case CS_CHAT:
		{
			cout << "Recv Chat Packet" << endl;
			CS_CHAT_PACKET* pkt = reinterpret_cast<CS_CHAT_PACKET*>(packet);
			
			SC_CHAT_PACKET p;
			p.size = sizeof(SC_CHAT_PACKET);
			p.type = SC_CHAT;
			p.id = id;
			strcpy_s(p.name, players[id].name.c_str());
			
			for (auto& player : players) {
				if (!player.connected) continue;
				player.session->Write(&p, p.size);
			}
		}
		}
	}

	void ProcessRead()
	{
		auto self(shared_from_this());

		socket_.async_read_some(boost::asio::buffer(data, BUF_SIZE),
		[this, self](boost::system::error_code ec, std::size_t length)
		{
			if (ec)
			{
				if (ec.value() == boost::asio::error::operation_aborted) return;		
				if (false == players[my_id].connected) return;			
				cout << "Receive Error on Session[" << my_id << "] EC[" << ec << "]\n";	// error. 
				socket_.shutdown(socket_.shutdown_both);
				socket_.close();
				// socket_ = boost::asio::ip::tcp::socket(*io_service_);
				return;
			}
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
					// 패킷 조립
					memcpy(packet + prev_data_size, buf, need_to_build);
					ProcessPacket(packet, my_id);
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
		/*unsigned char* ptr = buf;
		static size_t in_packet_size = 0;
		static size_t saved_packet_size = 0;
		static unsigned char packet_buffer[BUF_SIZE];

		while (0 < io_byte) {
			if (0 == in_packet_size) in_packet_size = ptr[0];
			if (io_byte + saved_packet_size >= in_packet_size) {
				memcpy(packet_buffer + saved_packet_size, ptr, in_packet_size - saved_packet_size);
				ProcessPacket(packet_buffer, my_id);
				ptr += in_packet_size - saved_packet_size;
				io_byte -= in_packet_size - saved_packet_size;
				in_packet_size = 0;
				saved_packet_size = 0;
			}
			else {
				memcpy(packet_buffer + saved_packet_size, ptr, io_byte);
				saved_packet_size += io_byte;
				io_byte = 0;
			}
		}*/

		unsigned char* ptr = buf;
		while (io_byte > 0) {
			if (0 == cur_packet_size) cur_packet_size = ptr[0];
			int need_to_build_size = cur_packet_size - prev_data_size;
			if (need_to_build_size >= io_byte) {
				memcpy(packet + prev_data_size, ptr, need_to_build_size);
				ProcessPacket(packet, my_id);
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

	void Write(void* packet, std::size_t length)
	{
		auto self(shared_from_this());

		socket_.async_write_some(boost::asio::buffer(packet, length),
			[this, self, packet, length](boost::system::error_code ec, std::size_t bytes_transferred)
			{
				if (!ec)
				{
					if (length != bytes_transferred)
						cout << "Incomplete Send occured on session[" << my_id << "]. This session should be closed.\n";
					//delete packet;
				}
			});
	}

public:
	Session(tcp::socket socket)
		: socket_(std::move(socket))
	{
		cur_packet_size = 0;
		prev_data_size = 0;
	}

	void start()
	{
		my_id = GetNewClientID();
		cout << "Client[" << my_id + 1 << "] Connected\n";

		players[my_id].session = this;
		players[my_id].connected = true;

		ProcessRead();
	}
};

class GameServer
{
public:
	GameServer(boost::asio::io_context& io_context, int port) : acceptor_(io_context, tcp::endpoint(tcp::v4(), port)),
		socket_(io_context)
	{
		RegisterAccept();
	}

private:
	tcp::acceptor acceptor_;
	tcp::socket socket_;

	void RegisterAccept()
	{
		acceptor_.async_accept(socket_, [this](boost::system::error_code ec) 
			{
				if (!ec) {
					std::make_shared<Session>(std::move(socket_))->start();
				}
				RegisterAccept();
			});
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
	vector<thread*> worker_threads;

	GameServer gameServer(io_context, SERVER_PORT);

	_wsetlocale(LC_ALL, L"korean");

	for (int i = 0; i < MAX_USER; ++i) {
		players[i].connected = false;
	}

	std::size_t numThreads = std::thread::hardware_concurrency();
	for (int i = 0; i < numThreads; i++) {
		worker_threads.emplace_back(new thread{ worker_thread, &io_context });
	}

	for (auto& wth : worker_threads) {
		wth->join();
		delete wth;
	}
}