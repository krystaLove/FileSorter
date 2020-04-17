#pragma once

#include <stack>
#include <queue>

#include <chrono>
#include <thread>
#include <mutex>

#include <filesystem>
#include <fstream>

#include <cstring>
#include <string>
#include <iostream>

namespace fs = std::filesystem;
using namespace std::literals::chrono_literals;

namespace Paths
{
	// Директория, в котором работает программа
	const std::string MAIN_PATH_TO_WORK = "toSort";

	const std::string MUSIC_EXTENSION = ".mp3";
	const std::string PICTURE_EXTENSION = ".jpg";

	// Каталог для сортировки mp3 файлов
	const std::string MUSIC_DESTINATION = "mp3";
	// Каталог для сортировки jpg файлов
	const std::string PICTURE_DESTINATION = "jpg";

	// Каталог для вывода логов
	const std::string LOG_DIRECTORY = "logs";
	// Название файлов для вывода логов потоков записи
	const std::string WRITE_THREAD_LOG_PATH = "write.thread";
}


