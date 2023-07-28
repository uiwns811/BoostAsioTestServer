#include "stdafx.h"
#include "Client.h"

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
		if (client.isChatting) {
			wstring msg;
			getline(wcin, msg);
			if (0 != wcscmp(msg.c_str(), L"\0")) {
				client.SendChatPacket(msg);
			}
		}
	}

	th->join();
	delete th;

	return 0;
}