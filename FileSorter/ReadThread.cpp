#include "ReadThread.h"

ReadThread::ReadThread(std::string& path, std::shared_ptr<threadsafe_queue<File>> q)
	: m_Path(path), SharedBuffer(q)
{}

void ReadThread::Start()
{
	t = std::move(std::thread(&ReadThread::_read, this));
}

void ReadThread::Join()
{
	t.join();
}

bool ReadThread::Joinable()
{
	return t.joinable();
}

void ReadThread::Stop()
{
	m_Work = false;
	Join();
}

void ReadThread::_read()
{
	std::ifstream sourceStream;
	std::queue<fs::path> catchedFiles;

	while (m_Work)
	{
		std::this_thread::sleep_for(READING_PERIOD);

		// Получение всех файлов в директории
		for (const auto& entry : fs::directory_iterator(m_Path))
		{

			if (!entry.path().has_extension())
				continue;

			std::cout << "Read Thread: Some file catched" << std::endl;
			catchedFiles.push(entry.path());
		}

		if (catchedFiles.empty())
		{
			std::cout << "Read Thread:  Waiting for files to read!" << std::endl;
			continue;
		}

		while (!catchedFiles.empty())
		{
			std::cout << "Read Thread: Writing to buffer" << std::endl;
			// Обработка файла по пути
			_writeToBuffer(catchedFiles.front(), sourceStream);
			catchedFiles.pop();
		}
	}
}

/*
* Вспомогательная функция для загрузки в буфер
*/
void ReadThread::_writeToBuffer(fs::path path, std::ifstream& sourceStream)
{
	std::cout << "Read Thread: Working with: " <<
		std::endl << path << std::endl;

	// Открытие файла по пути в бинарном виде
	sourceStream.open(path, std::ios::binary | std::ios::in);
	std::filebuf* pbuf = sourceStream.rdbuf();

	// Размер бинарного файла
	long long size = pbuf->pubseekoff(0, std::ios::end, std::ios::in);
	std::cout << "Read Thread: File size in bytes: " << size << std::endl;

	// Установление указателя на начало
	pbuf->pubseekpos(0, std::ios::in);

	// Память по указателю чистится после записи
	char* buffer = new char[size];
	pbuf->sgetn(buffer, size);

	SharedBuffer->push(File(path, size, buffer));

	sourceStream.close();

	// Удаление считанного в буфер файла из директории
	fs::remove(path);

	std::cout << std::endl;
}
