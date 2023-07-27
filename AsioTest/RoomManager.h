#pragma once
#include "stdafx.h"
#include "TSingleton.h"

class Room;
class RoomManager : public TSingleton<RoomManager>
{
	vector<pair<int, shared_ptr<Room>>> rooms;
	mutex m_lock;
	atomic_int m_room_id = 0;

public:
	void CreateRoom();
	void CreateRoom(int id);
	void RemoveRoom(int id);

	vector<RoomInfo> GetRoomInfo();
	shared_ptr<Room> GetRoom(int id);
};

