#include "WriteThread.h"

WriteThread::WriteThread(std::string& path, int index, std::shared_ptr<threadsafe_queue<File>> q)
	: m_Path(path), m_Index(index), SharedBuffer(q)
{
	log_stream.open(Paths::LOG_DIRECTORY + "\\" + Paths::WRITE_THREAD_LOG_PATH + std::to_string(m_Index));
}

void WriteThread::Start()
{
	t = std::move(std::thread(&WriteThread::_sort, this));
}

void WriteThread::Stop()
{
	m_Work = false;
	Join();
}

void WriteThread::Join()
{
	t.join();
}

bool WriteThread::Joinable()
{
	return t.joinable();
}

void WriteThread::_sort()
{
	while (m_Work)
	{
		// Ждём остальные потоки
		if (SharedBuffer->empty())
		{
			// Задержка между попытками получить что-то из буфера
			std::this_thread::sleep_for(CHECKING_PERIOD);
			continue;
		}

		std::ofstream destStream;

		// Получение файла из общей для всех потоков очереди
		File file = *SharedBuffer->wait_and_pop();

		log_stream << "[ File Processing ]" << std::endl << 
					  "File: " << file.path.filename() << std::endl << 
					  "Size: " << (file.size) << "b" << std::endl;

		std::string path = m_Path + "\\"
			+ ((file.path.extension().string() == Paths::MUSIC_EXTENSION) ? Paths::MUSIC_DESTINATION : Paths::PICTURE_DESTINATION);
		std::string filename = file.path.filename().string();
		std::string fullpath = path + "\\" + filename;

		fs::create_directory(path);

		destStream.open(fullpath, std::ios::binary);
		destStream.write(file.buf, file.size);

		log_stream << "File sorted to " << fullpath << std::endl;

		delete[] file.buf;

		log_stream << std::endl;
	}
}
