#include "RoomManager.h"
#include "Room.h"

void RoomManager::CreateRoom()
{
	if (rooms.size() >= MAX_ROOM_SIZE) {
		cout << "더이상 Room을 생성할 수 없음" << endl;
	}
	m_lock.lock();
	int roomId = m_room_id++;
	while (binary_search(rooms.begin(), rooms.end(), roomId, Compare())) {
		roomId++;
	}
	pair<int, shared_ptr<Room>> room = make_pair(roomId, make_shared<Room>());
	rooms.emplace_back(room);
	room.second->m_id = roomId;
	sort(rooms.begin(), rooms.end());
	m_lock.unlock();
}

void RoomManager::CreateRoom(int id)
{
	if (rooms.size() >= MAX_ROOM_SIZE) {
		cout << "더이상 Room을 생성할 수 없음" << endl;
	}
	m_lock.lock();
	if (binary_search(rooms.begin(), rooms.end(), id, Compare())) {
		m_lock.unlock();
		return;
	}
	pair<int, shared_ptr<Room>> room = make_pair(id, make_shared<Room>());
	rooms.emplace_back(room);
	room.second->m_id = id;
	m_lock.unlock();
}

void RoomManager::RemoveRoom(int id)
{
	m_lock.lock();
	//roomList.erase(roomList.begin() + id);
	rooms.erase(remove_if(rooms.begin(), rooms.end(), 
		[id](const pair<int, shared_ptr<Room>>& room) {return room.first == id; }));
	cout << "Room[" << id << "]가 삭제되었습니다";
	m_lock.unlock();
}

vector<RoomInfo> RoomManager::GetRoomInfo()
{
	vector<RoomInfo> roomInfo;
	m_lock.lock();
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
	CreateRoom(id);
	m_lock.lock();
	auto room = find_if(rooms.begin(), rooms.end(),
		[id](const pair<int, shared_ptr<Room>>& pair) {return pair.first == id; });
	m_lock.unlock();
	return room->second;
}