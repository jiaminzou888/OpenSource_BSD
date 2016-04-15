#pragma once

#include "RedisWrapper.h"
#include "MdBroadCast.h"
#include "CppThread.hpp"
#include "CandleBar.h"

#include <vector>
#include <string>

class CMdManager
{
public:
	CMdManager();

	static void controll_function(void* data);

	bool initial_md_manager();
	void release_md_manager();

	bool open_consumer_thread();
	void close_consumer_thread();

	bool subscribe_market(std::vector<std::string> ins);

	void attach_md_strategy(CStrategy* stg);

private:
	std::string		root_log_;

	CRedisWrapper	redis_;
	CMdBroadCast	md_;
	CppThread		thread_;
	
	std::vector<std::string>		intrusts_;
};