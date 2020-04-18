#include "FileSorter.h"

FileSorter::FileSorter(const std::string& path) : m_Path(path)
{
	SharedBuffer = std::make_shared<threadsafe_queue<File>>();
	m_ReadThread = std::make_unique<ReadThread>(m_Path, SharedBuffer);

	m_AvailableThreads = std::thread::hardware_concurrency();
	if (m_AvailableThreads == 0)
		m_AvailableThreads = 2;
}

void FileSorter::Stop()
{
	std::cout << "[ Terminating Read Thread ]" << std::endl;
	m_ReadThread->Stop();

	_stopAllWriteThreads();
}

void FileSorter::Start()
{
	std::cout << "[ Available threads: " << m_AvailableThreads << " ] " << std::endl;
	m_ReadThread->Start();
	m_AvailableThreads--;

	_createWriteThreads();

	// ���� ����� ������ �������
	while (m_ReadThread->Joinable())
	{
		std::this_thread::sleep_for(CHECKING_BUFFER_PERIOD);

		// ���� ������ ����������
		// ��������� ���������� (������ ������ ����� ������� ������ �� ����, ��� ���������� ��������� ��������)
		if (SharedBuffer->empty())
		{
			// ���� ����� ������� ������ �� ���� �������, �� ��������� ������
			if (++notFoundFilesCycles == WAITING_CYCLES_TO_FINISH)
			{
				int duration = static_cast<int> ((float)(CHECKING_BUFFER_PERIOD).count() / 1000.f * WAITING_CYCLES_TO_FINISH);

				std::cout << "\n[ NO FILE CATCHED IN " << duration << " SECONDS! ]" << std::endl;

				// ��������� ���� ������� (������ � ������)
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

void FileSorter::_createWriteThreads()
{
	for (int i = 0; i < m_AvailableThreads; ++i)
	{
		wtVector.push_back(std::make_unique<WriteThread>(m_Path, i + 1, SharedBuffer));
		wtVector[i]->Start();
	}
}

void FileSorter::_stopAllWriteThreads()
{
	for (int i = 0; i < wtVector.size(); ++i)
	{
		std::cout << "[ Terminating Write Thread " << i + 1 << " ]" << std::endl;
		if (wtVector[i]->Joinable())
			wtVector[i]->Stop();
	}
}