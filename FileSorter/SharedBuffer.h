#pragma once

#include "dependencies.h"

struct File
{
	fs::path path;
	int size;
	char* buf;

	File(fs::path path, long long size, char* buf)
	{
		this->path = path;
		this->size = size;
		this->buf = buf;
	}
};

//Очередь файлов
struct
{
	std::mutex m;
	std::queue<File> buf;
} SharedBuffer;

