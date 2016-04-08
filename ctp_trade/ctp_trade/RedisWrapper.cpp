#include "RedisWrapper.h"

#include <iostream>

bool CRedisWrapper::connect_redis_instance()
{
	bool connect_ret = true;

	struct timeval timeout = { 1, 500000 }; // 1.5 seconds  
	connect_ = redisConnectWithTimeout((char*)"127.0.0.1", 6379, timeout);

	if (connect_->err)
	{
		connect_ret = false;
	}

	return connect_ret;

}

void CRedisWrapper::free_redis_instance()
{
	if (connect_)
	{
		redisFree(connect_);
		connect_ = nullptr;
	}
}

bool CRedisWrapper::execute_one_operation(const char* command)
{
	bool execute_ret = true;

	reply_ = (redisReply *)redisCommand(connect_, command);

	if (strncmp(reply_.load()->str, "OK", reply_.load()->len))
	{
		execute_ret = false;
	}
	else if (reply_.load()->type == REDIS_REPLY_ARRAY)
	{
		redisReply* c = reply_.load();
		for (int i = 0; i < reply_.load()->elements; i += 5)
		{
			for (int j = 0; j < 5; ++j)
			{
				std::cout << reply_.load()->element[i+j]->str << "\t";
			}
			std::cout << std::endl;
		}
	}
	
	freeReplyObject(reply_);

	return execute_ret;
}

// transaction
bool CRedisWrapper::execute_mul_operation(const char** commands, int counts)
{
	bool execute_ret = true;

	reply_ = (redisReply *)redisCommand(connect_, "MULTI");

	for (int i = 0; i < counts; ++i)
	{
		reply_ = (redisReply *)redisCommand(connect_, commands[i]);
	}

	reply_ = (redisReply *)redisCommand(connect_, "EXEC");

	freeReplyObject(reply_);

	return execute_ret;
}