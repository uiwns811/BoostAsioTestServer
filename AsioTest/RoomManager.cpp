#include "RoomManager.h"
#include "Room.h"

int RoomManager::CreateRoom(const shared_ptr<Room>& room)
{
	if (roomList.size() >= MAX_ROOM_SIZE) {
		cout << "더이상 Room을 생성할 수 없음" << endl;
	}
	m_lock.lock();
	roomList.emplace_back(room);
	m_lock.unlock();
	int roomId = find(roomList.begin(), roomList.end(), room) - roomList.begin();
	return roomId;
}

void RoomManager::CreateRoom()
{
	if (roomList.size() >= MAX_ROOM_SIZE) {
		cout << "더이상 Room을 생성할 수 없음" << endl;
	}
	m_lock.lock();
	int roomId = m_room_id++;
	rooms[roomId] = make_shared<Room>();
	rooms[roomId]->m_id = roomId;
	m_lock.unlock();
}

void RoomManager::RemoveRoom(int id)
{
	m_lock.lock();
	//roomList.erase(roomList.begin() + id);
	rooms.erase(id);
	m_lock.unlock();
}

vector<RoomInfo> RoomManager::GetRoomInfo()
{
	vector<RoomInfo> roomInfo;
	m_lock.lock();
	//for (int i = 0; i < roomList.size(); i++) {
	//	roomInfo[i].room_id = i;
	//	roomInfo[i].cur_user_cnt = roomList[i]->clients.size();
	//}
	for (auto& room : rooms) {
		RoomInfo info;
		info.room_id = room.first;
		info.cur_user_cnt = room.second->clients.size();
		roomInfo.emplace_back(info);
	}
	m_lock.unlock();
	return roomInfo;
}

shared_ptr<Room> RoomManager::GetRoom(int id)
{
	m_lock.lock();
	shared_ptr<Room> room = rooms[id]->shared_from_this();
	m_lock.unlock();
	return room;
}