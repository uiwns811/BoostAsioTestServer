#pragma once
#include "stdafx.h"

template <typename T>
class TSingleton
{
protected:
	TSingleton() {};
	virtual ~TSingleton() {};

private:
	static T* m_pInstance;

public:
	static T* GetInstance() {
		if (m_pInstance == nullptr)
			m_pInstance = new T();
		return m_pInstance;
	}

	static void DestroyInstance() {
		if (m_pInstance) {
			delete m_pInstance;
			m_pInstance = nullptr;
		}
	}
};

template <typename T>
T* TSingleton<T>::m_pInstance = nullptr;