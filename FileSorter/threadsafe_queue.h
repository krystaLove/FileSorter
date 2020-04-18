#pragma once
#ifndef THREADSAFE_QUEUE
#define THREADSAFE_QUEUE

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


template <typename T>
class threadsafe_queue
{
private:
	mutable std::mutex mut;
	std::queue<std::shared_ptr<T> > data_queue;
	std::condition_variable data_cond;

public:
	threadsafe_queue()
	{}

	std::shared_ptr<T> wait_and_pop()
	{
		std::unique_lock<std::mutex> lk(mut);
		data_cond.wait(lk, [this] { return !data_queue.empty(); });
		std::shared_ptr<T> res = data_queue.front();
		data_queue.pop();
		return res;
	}

	void push(const T& value)
	{
		std::shared_ptr data = std::make_shared<T>(std::move(value));
		std::lock_guard<std::mutex> lk(mut);
		data_queue.push(data);
		data_cond.notify_one();
	}

	bool empty() const
	{
		std::lock_guard<std::mutex> lk(mut);
		return data_queue.empty();
	}
};

#endif
