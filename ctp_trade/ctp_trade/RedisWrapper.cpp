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


/*
#define REDIS_REPLY_STRING 1
#define REDIS_REPLY_ARRAY 2
#define REDIS_REPLY_INTEGER 3
#define REDIS_REPLY_NIL 4
#define REDIS_REPLY_STATUS 5
#define REDIS_REPLY_ERROR 6
*/
bool CRedisWrapper::execute_one_operation(const char* command, size_t need_num, std::vector<std::string>* reply_strs)
{
	bool execute_ret = true;

	redisReply* reply = (redisReply *)redisCommand(connect_, command);

	size_t reply_flag = reply->type;
	switch (reply_flag)
	{
	case REDIS_REPLY_ERROR:
	{
		execute_ret = false;
	}
		break;
	case REDIS_REPLY_STRING:
	{
		reply_strs->push_back(reply->str);
	}
		break;
	case REDIS_REPLY_INTEGER:
	{
		if (need_num > (size_t)reply->integer)
		{
			execute_ret = false;
		}
	}
		break;
	case REDIS_REPLY_ARRAY:
	{
		for (int i = 0; i < reply->elements; ++i)
		{
			reply_strs->push_back(reply->element[i]->str);
		}
	}
		break;
	default:
		break;
		
	}
	
	freeReplyObject(reply);

	return execute_ret;
}

// transaction
bool CRedisWrapper::execute_mul_operation(const char** commands, int counts)
{
	bool execute_ret = true;

	redisReply* reply = (redisReply *)redisCommand(connect_, "MULTI");

	for (int i = 0; i < counts; ++i)
	{
		reply = (redisReply *)redisCommand(connect_, commands[i]);
	}

	reply = (redisReply *)redisCommand(connect_, "EXEC");

	freeReplyObject(reply);

	return execute_ret;
}