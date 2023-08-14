#pragma once

const int SERVER_PORT = 8011;
const int MAX_USER = 1000;
const int BUF_SIZE = 1024;
const int NAME_SIZE = 50;
const int CHAT_SIZE = 50;
const int MAX_ROOM_USER = 4;
const int MAX_ROOM_SIZE = 10;

enum class PacketType : char
{
	CS_LOGIN = 11,
	CS_LEAVE_PLAYER = 12,
	CS_SELECT_ROOM = 13,
	CS_CHAT = 14,

	SC_LOGIN_OK = 51,
	SC_ENTER_LOBBY = 52,
	SC_LEAVE_PLAYER = 53,
	SC_ROOM_INFO = 54,
	SC_ENTER_ROOM = 55,
	SC_CHAT = 56,
};

#pragma pack(push, 1)

struct PacketHeader {
	char size;
	char type;
};

struct RoomInfo {
	int room_id;
	int cur_user_cnt;
};

//////////////////////////////

struct CS_LOGIN_PACKET {
	char size;
	PacketType type;
	wchar_t name[NAME_SIZE];
};

struct CS_LEAVE_PLAYER_PACKET {
	char size;
	PacketType type;
};

struct CS_SELECT_ROOM_PACKET {
	char size;
	PacketType type;
	int room_id;
};

struct CS_CHAT_PACKET {
	char size;
	PacketType type;
	wchar_t chat[CHAT_SIZE];
};

//////////////////////////////////////////////

struct SC_LOGIN_OK_PACKET {
	char size;
	PacketType type;
	int id;
};

struct SC_ENTER_LOBBY_PACKET {
	char size;
	PacketType type;
	int id;
	wchar_t name[NAME_SIZE];
};

struct SC_LEAVE_PLAYER_PACKET {
	char size;
	PacketType type;
	int id;
	wchar_t name[NAME_SIZE];
};

struct SC_ROOM_INFO_PACKET {
	char size;
	PacketType type;
	RoomInfo roomList[MAX_ROOM_SIZE];
};

struct SC_ENTER_ROOM_PACKET {
	char size;
	PacketType type;
	int client_id;
	int room_id;
};

struct SC_CHAT_PACKET {
	char size;
	PacketType type;
	wchar_t chat[CHAT_SIZE];
	wchar_t name[NAME_SIZE];
	int id;
};

#pragma pack (pop)
