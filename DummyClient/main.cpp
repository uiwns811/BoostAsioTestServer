#include "stdafx.h"
#include "Client.h"
#include "ServerSession.h"

int main()
{
	boost::asio::io_context io_context;
	boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::address::from_string("127.0.0.1"), SERVER_PORT);

	try {
		Client client(io_context, endpoint);
		client.Run(&io_context);

		while (client.IsConnected() == false)
			continue;

		while (true) {
			if (client.m_session->ChatCheck() == false) continue;
			wstring msg;
			getline(wcin, msg);
			if (0 != wcscmp(msg.c_str(), L"\0")) {
				client.m_session->SendChatPacket(msg);
			}
		}

		client.Shutdown();
		return 0;
	}
	catch (exception& ex) {
		cout << "Exception : " << ex.what() << endl;
		return -1;
	}
}