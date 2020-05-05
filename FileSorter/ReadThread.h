#pragma once

#include "dependencies.h"

#include "threadsafe_queue.h"

#define READING_PERIOD 1000ms

class ReadThread
{
private:
	std::string m_Path;
	std::thread _t;

	std::shared_ptr<threadsafe_queue<File>> SharedBuffer;

	bool m_Work = true;

	/*
	* Чтение путей всех файлов, находящихся по пути m_Path и последующая их загрузка в буфер
	* Основная точка входа потока чтения
	*/
	void _read();
	
	/*
	* Вспомогательная функция для загрузки в буфер
	*/
	void _writeToBuffer(fs::path path, std::ifstream& sourceStream);

public:
	ReadThread(std::string&, std::shared_ptr<threadsafe_queue<File>>);

	void Start();
	void Stop();
	void Join();
	bool Joinable();
};


