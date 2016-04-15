#pragma once

#include "SolveReDefinition.h"
// Has To Be Behind 'SolveReDefinition.h'

#define GOOGLE_GLOG_DLL_DECL           // be used by static glog lib
#define GLOG_NO_ABBREVIATED_SEVERITIES // it will cause re-definition because of <windows.h>
#include "windows/glog/logging.h"


class CGLog
{
public:
	inline static CGLog* get_glog()
	{
		return const_cast<CGLog*>(google_log);
	}

	inline void init_log(const char* log_dir)
	{
		google::InitGoogleLogging("ctp_trade");

		FLAGS_log_dir = log_dir; 
	}

	inline void release_log()
	{
		google::ShutdownGoogleLogging();
	}

	inline void print_log(char* out_msg)
	{
		LOG(INFO) << out_msg;
	}

private:
	CGLog()		= default;
	~CGLog()	= default;
	
	CGLog(const CGLog&)				= delete;
	CGLog& operator =(const CGLog&) = delete;

private:
	class GcGlog
	{
	public:
		~GcGlog()
		{
			if (google_log != nullptr)
			{
				delete google_log;
				google_log = nullptr;
			}
		}
	};

	std::string  log_directory;
	static const CGLog* google_log;
	static const GcGlog gc_log;
};