#pragma once

#include <thread>
#include <mutex>
#include <condition_variable>

class CRWLock
{
public:
	inline void read_lock()
	{
		std::unique_lock<std::mutex> lck{ _mtx };
		while (rw_cnt < 0)
		{
			_cv.wait(lck);
		}
		++rw_cnt;
		lck.unlock();
	}

	inline void read_unlock()
	{
		std::unique_lock<std::mutex> lck{ _mtx };
		--rw_cnt;
		if (0 == rw_cnt)
		{
			_cv.notify_one();
		}
		lck.unlock();
	}

	inline void write_lock()
	{
		std::unique_lock<std::mutex> lck{ _mtx };
		while (rw_cnt != 0)
		{
			_cv.wait(lck);
		}
		--rw_cnt;
		lck.unlock();
	}

	inline void write_unlock()
	{
		std::unique_lock<std::mutex> lck{ _mtx };
		rw_cnt = 0;
		_cv.notify_all();
		lck.unlock();
	}

private:
	std::mutex _mtx;
	std::condition_variable _cv;
	int rw_cnt{ 0 }; // 0: no lock; <0: write; >0 read
};