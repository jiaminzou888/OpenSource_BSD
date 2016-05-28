#pragma once

#include "RedisWrapper.h"
#include "MdBroadCast.h"
#include "CppThread.hpp"

class CMdManager
{
public:
	CMdManager();

	bool initial_md_manager(std::vector<std::string>& ins);
	void release_md_manager();

	bool get_md_conncet_status();
	bool subscribe_market();

	void attach_md_strategy(CStrategy* stg);

private:
	std::string		root_log_;

	CRedisWrapper	redis_;
	CMdBroadCast	md_;
	
};