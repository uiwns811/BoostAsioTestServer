#pragma once
#include "stdafx.h"

class ClientSession;
class Room
{
public:
	vector<ClientSession> clients;

public:
	void Enter();
};

