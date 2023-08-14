#include "ClientSession.h"
#include "SessionManager.h"
#include "RoomManager.h"
#include "Room.h"
#include "PacketManager.h"

void ClientSession::ProcessConnect()
{
	wcout << "New Client Connected" << endl;
	SessionManager::GetInstance()->AddClient(shared_from_this());
	RegisterRecv();
}

void ClientSession::ProcessDisconnect()
{
	SessionManager::GetInstance()->RemoveClient(m_id);
	shared_ptr<Room> room = m_room.lock();
	if (room != nullptr) {
		room->LeavePlayer(shared_from_this());
		room = nullptr;
	}
	m_socket.shutdown(m_socket.shutdown_both);
	m_socket.close();
}

void ClientSession::RegisterRecv()
{
	auto self(shared_from_this());
	m_socket.async_read_some(boost::asio::buffer(recvbuff, BUF_SIZE),
		[this, self](boost::system::error_code ec, std::size_t length) {
			ProcessRecv(ec, length);
		});
}

void ClientSession::ProcessRecv(const boost::system::error_code& ec, std::size_t length)
{
	if (ec)
	{
		if (ec.value() == boost::asio::error::operation_aborted) return;
		//if (nullptr == SessionManager::GetInstance()->FindClient(m_id)) return;
		cout << "Receive Error on Session[" << m_id << "] EC[" << ec << "]\n";	// error. 
		ProcessDisconnect();
		return;
	}
	PacketManager::GetInstance()->Enqueue(shared_from_this(), recvbuff, length);
	RegisterRecv();
}

void ClientSession::RegisterSend(void* packet, std::size_t length)
{
	auto self(shared_from_this());
	m_socket.async_write_some(boost::asio::buffer(packet, length),
		[this, self, packet, length](boost::system::error_code ec, std::size_t bytes_transferred)
		{
			if (!ec)
			{
				if (length != bytes_transferred) {
					wcout << "Incomplete Send occured on session[" << m_id << "]" << endl;
				}					
				delete packet;
			}
		});
}

void ClientSession::SendPacket(void* packet)
{
	int packet_size = reinterpret_cast<unsigned char*>(packet)[0];
	unsigned char* buff = new unsigned char[packet_size];
	memcpy(buff, packet, packet_size);
	RegisterSend(buff, packet_size);
}
