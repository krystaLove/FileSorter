#pragma once

#include "dependencies.h"

#include "threadsafe_queue.h"

#define CHECKING_PERIOD 500ms

class WriteThread
{
private:
	std::string m_Path;
	std::thread t;

	std::shared_ptr<threadsafe_queue<File>> SharedBuffer;

	// Индекс потока
	int m_Index = 0;

	bool m_Work = true;

	std::ofstream log_stream;

	/*
	* Выгрузка данных из буфера с сортировкой по каталогам
	*/
	void _sort();

public:
	WriteThread(std::string& path, int index, std::shared_ptr<threadsafe_queue<File>>);

	void Start();
	void Stop();
	bool Joinable();
	void Join();
};

