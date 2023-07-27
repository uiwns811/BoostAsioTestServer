#pragma once

const int SERVER_PORT = 8011;
const int MAX_USER = 1000;
const int BUF_SIZE = 1024;
const int NAME_SIZE = 50;
const int CHAT_SIZE = 50;
const int MAX_ROOM_USER = 4;
const int MAX_ROOM_SIZE = 10;

const char CS_LOGIN = 1;
const char CS_LEAVE_PLAYER = 2;
const char CS_SELECT_ROOM = 3;
const char CS_CHAT = 4;

const char SC_LOGIN_OK = 1;
const char SC_ENTER_LOBBY = 2;
const char SC_LEAVE_PLAYER = 3;
const char SC_ROOM_INFO = 4;
const char SC_ENTER_ROOM = 5;
const char SC_CHAT = 6;

struct RoomInfo {
	int room_id;
	int cur_user_cnt;
};

//////////////////////////////

struct CS_LOGIN_PACKET {
	char size;
	char type;
	wchar_t name[NAME_SIZE];
};

struct CS_LEAVE_PLAYER_PACKET {
	char size;
	char type;
};

struct CS_SELECT_ROOM_PACKET {
	char size;
	char type;
	int room_id;
};

struct CS_CHAT_PACKET {
	char size;
	char type;
	wchar_t chat[CHAT_SIZE];
};

//////////////////////////////////////////////

struct SC_LOGIN_OK_PACKET {
	char size;
	char type;
	int id;
};

struct SC_ENTER_LOBBY_PACKET {
	char size;
	char type;
	int id;
	wchar_t name[NAME_SIZE];
};

struct SC_LEAVE_PLAYER_PACKET {
	char size;
	char type;
	int id;
	wchar_t name[NAME_SIZE];
};

struct SC_ROOM_INFO_PACKET {
	char size;
	char type;
	RoomInfo roomList[MAX_ROOM_SIZE];
};

struct SC_ENTER_ROOM_PACKET {
	char size;
	char type;
	int client_id;
	int room_id;
};

struct SC_CHAT_PACKET {
	char size;
	char type;
	wchar_t chat[CHAT_SIZE];
	wchar_t name[NAME_SIZE];
	int id;
};
