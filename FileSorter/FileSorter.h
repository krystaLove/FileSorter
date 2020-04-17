#pragma once

#include "ReadThread.h"
#include "WriteThread.h"

#include "dependencies.h"

#include <memory>

using namespace std::literals::chrono_literals;

const int WAITING_CYCLES_TO_FINISH = 30;

#define CHECKING_BUFFER_PERIOD 1000ms

class FileSorter
{
private:
	std::string m_Path;
	std::unique_ptr<ReadThread> m_ReadThread;
	std::vector<std::unique_ptr<WriteThread> > wtVector;

	unsigned int m_AvailableThreads = 2;

	int notFoundFilesCycles = 0; // Сколько периодов чтения подряд буфер оказывался пустой

	void _createWriteThreads()
	{
		for (int i = 0; i < m_AvailableThreads; ++i)
		{
			wtVector.push_back(std::make_unique<WriteThread>(m_Path, i + 1));
			wtVector[i]->Start();
		}
	}

	void _stopAllWriteThreads()
	{
		for (int i = 0; i < wtVector.size(); ++i)
		{
			std::cout << "[ Terminating Write Thread " << i + 1 << " ]" << std::endl;
			if (wtVector[i]->Joinable())
				wtVector[i]->Stop();
		}
	}

public:

	FileSorter(const std::string& path) : m_Path(path)
	{
		m_ReadThread = std::make_unique<ReadThread>(m_Path);
		
		m_AvailableThreads = std::thread::hardware_concurrency();
		if (m_AvailableThreads == 0)
			m_AvailableThreads = 2;
	}

	void Stop()
	{
		std::cout << "[ Terminating Read Thread ]" << std::endl;
		m_ReadThread->Stop();

		_stopAllWriteThreads();
	}

	void Start()
	{
		std::cout << "[ Available threads: " << m_AvailableThreads << " ] "<< std::endl;
		m_ReadThread->Start();
		m_AvailableThreads--;

		_createWriteThreads();

		// Пока поток чтения запущен
		while (m_ReadThread->Joinable())
		{
			std::this_thread::sleep_for(CHECKING_BUFFER_PERIOD);
		
			// Если нечего записывать
			if (SharedBuffer.buf.empty())
			{
				// Если долго никаких файлов не было найдено, то завершаем потоки
				if (++notFoundFilesCycles == WAITING_CYCLES_TO_FINISH)
				{
					int duration = static_cast<int> ((float)(CHECKING_BUFFER_PERIOD).count() / 1000.f * WAITING_CYCLES_TO_FINISH);

					std::cout << "\n[ NO FILE CATCHED IN " << duration << " SECONDS! ]" << std::endl;

					// Остановка всех потоков (чтения и записи)
					Stop();
					return;
				}
				continue;
			}
			else
			{
				notFoundFilesCycles = 0;
			}
		}
	}

};


