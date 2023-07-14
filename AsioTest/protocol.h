#pragma once

constexpr int SERVER_PORT = 8011;
constexpr int MAX_USER = 1000;
constexpr int BUF_SIZE = 1024;
constexpr int NAME_SIZE = 50;
constexpr int CHAT_SIZE = 50;

#define CS_LOGIN 1
#define CS_CHAT 2

#define SC_LOGIN_OK 1
#define SC_ENTER_PLAYER 2
#define SC_CHAT 3

struct CS_LOGIN_PACKET {
	char size;
	char type;
	char name[NAME_SIZE];
};

struct CS_CHAT_PACKET {
	char size;
	char type;
	char chat[CHAT_SIZE];
};

struct SC_LOGIN_OK_PACKET {
	char size;
	char type;
	int id;
};

struct SC_ENTER_PLAYER_PACKET {
	char size;
	char type;
	int id;
	char name[NAME_SIZE];
};

struct SC_CHAT_PACKET {
	char size;
	char type;
	int id;
	char chat[CHAT_SIZE];
	char name[NAME_SIZE];
};