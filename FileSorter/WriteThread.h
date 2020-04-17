#pragma once

#include "dependencies.h"
#include "SharedBuffer.h"

#define CHECKING_PERIOD 500ms

class WriteThread
{
private:
	std::string m_Path;
	std::thread t;

	// Индекс потока
	int m_Index = 0;

	bool m_Work = true;

	std::ofstream log_stream;

	/*
	* Выгрузка данных из буфера с сортировкой по каталогам
	*/
	void _sort()
	{
		while (m_Work)
		{
			// Ждём остальные потоки
			SharedBuffer.m.lock();
			if (SharedBuffer.buf.empty())
			{
				SharedBuffer.m.unlock();
				
				// Задержка между попытками получить что-то из буфера
				std::this_thread::sleep_for(CHECKING_PERIOD);
				continue;
			}

			std::ofstream destStream;

			// Получение файла из общей для всех потоков очереди
			File file = std::move(SharedBuffer.buf.front());
			SharedBuffer.buf.pop();

			SharedBuffer.m.unlock();

			log_stream << "Write Thread " << m_Index << " : " << "File: " << file.path << "  " << file.size << std::endl;
			log_stream << "Write Thread " << m_Index << " : " << "Writing file from queue" << std::endl;

			std::string path = m_Path + "\\"
				+ ((file.path.extension().string() == Paths::MUSIC_EXTENSION) ? Paths::MUSIC_DESTINATION : Paths::PICTURE_DESTINATION);
			std::string filename = file.path.filename().string();
			std::string fullpath = path + "\\" + filename;

			log_stream << "Write Thread " << m_Index << " : " << "File sorted to " << fullpath << std::endl;

			fs::create_directory(path);

			destStream.open(fullpath, std::ios::binary);
			destStream.write(file.buf, file.size);

			delete[] file.buf;

			log_stream << std::endl;
		}
	}

public:
	WriteThread(std::string& path, int index) : m_Path(path), m_Index(index)
	{
		log_stream.open(Paths::LOG_DIRECTORY + "\\" + Paths::WRITE_THREAD_LOG_PATH + std::to_string(m_Index));
	}

	void Start()
	{
		t = std::move(std::thread(&WriteThread::_sort, this));
	}

	void Stop()
	{
		m_Work = false;
		Join();
	}

	bool Joinable()
	{
		return t.joinable();
	}

	void Join()
	{
		t.join();
	}
};

