#include <iostream>
#include <string>
#include "../AsioTest/protocol.h"

#include <boost/asio.hpp>

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

public:
	Client(boost::asio::io_context& io_context, const tcp::endpoint& endpoints) : socket(io_context), endpoint(endpoints)
	{
		
	}

	void Connect()
	{
		socket.async_connect(endpoint, [this](boost::system::error_code ec) {
			if (!ec) {
				cout << "Connected to server." << endl;

				CS_LOGIN_PACKET p;
				p.size = sizeof(CS_LOGIN_PACKET);
				p.type = CS_LOGIN;
				strcpy_s(p.name, "TEST");
				Write(&p, p.size);

				ProcessRead();
			}
			else
				cout << "connect failed : " << ec.message() << endl;
		});
	}

	void Write(void* packet, std::size_t length)
	{
		socket.async_write_some(boost::asio::buffer(packet, length),
			[this, packet, length](boost::system::error_code ec, std::size_t bytes_transferred)
			{
				if (!ec)
				{
					cout << "Send Complete" << endl;
					//delete packet;
				}
			});
	}

	void ProcessRead()
	{
		cout << "Process Read" << endl;
		socket.async_read_some(boost::asio::buffer(data, BUF_SIZE),
			[this](boost::system::error_code ec, std::size_t length) {
				cout << "Recv Callback" << endl;
				//int data_to_process = static_cast<int>(length);
				//if (data_to_process > 0)
				//	ProcessData(data, data_to_process);

				//cout << "Completed Read" << endl;
				//ProcessRead();
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
						// �ʳ��� ���
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
		cout << "ProcessPakcet" << endl;
		switch (packet[1])
		{
		case SC_LOGIN_OK:
		{
			cout << "Login OK" << endl;
		}
		break;
		}
	}
};

int main()
{
	boost::asio::io_context io_context;
	boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::address::from_string("127.0.0.1"), SERVER_PORT);

	Client client(io_context, endpoint);

	client.Connect();

	io_context.run();
}