#pragma once

#include "SolveReDefinition.h"
// Has To Be Behind 'SolveReDefinition.h'
#include <atomic>
#include <vector>

#include "hiredis.h" 
#include "Win32_Interop\win32fixes.h"

class CRedisWrapper
{
public:
	bool connect_redis_instance();
	void free_redis_instance();

	bool execute_one_operation(const char* command, size_t need_num = 0, std::vector<std::string>* reply_strs = nullptr);
	bool execute_mul_operation(const char** commands, int counts);

private:
	redisContext*	connect_{ nullptr };
};