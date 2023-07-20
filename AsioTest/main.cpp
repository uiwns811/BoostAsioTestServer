#include "stdafx.h"
#include "GameServer.h"


void worker_thread(boost::asio::io_context* io_context)
{
	io_context->run();
}

int main()
{
	_wsetlocale(LC_ALL, L"korean");
	boost::asio::io_context io_context;

	try {
		GameServer server(io_context, SERVER_PORT);
		server.Run(&io_context);
		return 0;
	}
	catch (exception& ex)
	{
		std::cout << "Exception : " << ex.what() << std::endl;
		return -1;
	}
}