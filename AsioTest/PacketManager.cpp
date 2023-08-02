#include "PacketManager.h"

void PacketManager::Enqueue(unsigned char* data)
{
	m_queue.push(data);
}

void PacketManager::ParseData()
{
	//int data_to_process = static_cast<int>(io_byte) + prev_data_size;
	//while (data_to_process > 0) {
	//	int cur_packet_size = buf[0];
	//	if (data_to_process >= cur_packet_size) {
	//		memcpy(remainData, buf, cur_packet_size);
	//		ProcessPacket(remainData, m_id);
	//		buf += cur_packet_size;
	//		data_to_process -= cur_packet_size;
	//	}
	//	else
	//		break;
	//}
	//prev_data_size = data_to_process;
	//if (prev_data_size > 0) {
	//	memcpy(remainData, buf, prev_data_size);
	//}
}

void PacketManager::Run()
{
	
}