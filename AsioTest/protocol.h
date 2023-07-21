#pragma once

constexpr int SERVER_PORT = 8011;
constexpr int MAX_USER = 1000;
constexpr int BUF_SIZE = 1024;
constexpr int NAME_SIZE = 50;
constexpr int CHAT_SIZE = 50;
constexpr int MAX_ROOM_USER = 4;
constexpr int MAX_ROOM_SIZE = 10;

#define CS_LOGIN 1
#define CS_LEAVE_PLAYER 2
#define CS_SELECT_ROOM 3
#define CS_CHAT 4

#define SC_LOGIN_OK 1
#define SC_ENTER_LOBBY 2
#define SC_LEAVE_PLAYER 3
#define SC_ROOM_INFO 4
#define SC_ENTER_ROOM 5
#define SC_CHAT 6

struct RoomInfo {
	int room_id;
	int cur_user_cnt;
};

struct CS_LOGIN_PACKET {
	char size;
	char type;
	char name[NAME_SIZE];
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
	char chat[CHAT_SIZE];
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
	char name[NAME_SIZE];
};

struct SC_LEAVE_PLAYER_PACKET {
	char size;
	char type;
	int id;
	char name[NAME_SIZE];
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
	int id;
	char chat[CHAT_SIZE];
	char name[NAME_SIZE];
};
