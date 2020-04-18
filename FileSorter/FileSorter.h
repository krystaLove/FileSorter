#pragma once

#include "ReadThread.h"
#include "WriteThread.h"

#include "dependencies.h"

#include <memory>

#include "threadsafe_queue.h"

const int WAITING_CYCLES_TO_FINISH = 30;

#define CHECKING_BUFFER_PERIOD 1000ms

class FileSorter
{
private:
	std::string m_Path;
	std::unique_ptr<ReadThread> m_ReadThread;
	std::vector<std::unique_ptr<WriteThread> > wtVector;

	std::shared_ptr<threadsafe_queue<File>> SharedBuffer;

	unsigned int m_AvailableThreads = 2;

	int notFoundFilesCycles = 0; // Сколько периодов чтения подряд буфер оказывался пустой

	void _createWriteThreads();
	void _stopAllWriteThreads();

public:

	FileSorter(const std::string& path);

	void Start();
	void Stop();
};


