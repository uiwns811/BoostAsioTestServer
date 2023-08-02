#pragma once
#include "stdafx.h"
#include "TSingleton.h"

class PacketManager : public TSingleton<PacketManager>
{
private:
	concurrent_queue<unsigned char*> m_queue;

	void ParseData();

public:
	void Enqueue(unsigned char* data);
	void Dequeue();

	void Run();
};

