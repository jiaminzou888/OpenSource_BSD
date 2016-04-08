#include "MdManager.h"

#include <algorithm>

void CMdManager::controll_function(void* data)
{
	CppThread*	handle_th = static_cast<CppThread*>(data);
	CMdManager* handle_ma = static_cast<CMdManager*>(handle_th->get_data());

	// Redis Operation String
	char redis_operation[1024] = { 0 };
	for (auto ins : handle_ma->intrusts_)
	{
		sprintf_s(redis_operation, sizeof(redis_operation)-1, "DEL %s", ins.c_str());
		handle_ma->redis_.execute_one_operation(redis_operation);
	}

	// Verify Insert Operation One By One At First, And Then Optimize Into Transaction By 'execute_mul_operation'.
	while (!handle_th->is_stop())
	{
		if (handle_ma->md_.get_mtk_size())
		{
			std::deque<mtk_type>&& temp_mtk = handle_ma->md_.get_mtk_queue();
			for (auto mtk : temp_mtk)
			{
				sprintf_s(redis_operation, "RPUSH %s %s %lf %d %lf %d", mtk.InstrumentID, mtk.TradingDay, mtk.AskPrice1, mtk.AskVolume1, mtk.BidPrice1, mtk.BidVolume1);
				handle_ma->redis_.execute_one_operation(redis_operation);
			}

			// test std::cout mtk hq
			for (auto ins : handle_ma->intrusts_)
			{
				sprintf_s(redis_operation, sizeof(redis_operation)-1, "LRANGE %s 0 -1", ins.c_str());
				handle_ma->redis_.execute_one_operation(redis_operation);
			}
		}
	}
}

bool CMdManager::initial_md_manager()
{
	// initial capacity of vector
	intrusts_.reserve(100);

	bool md_ret = true;
	
	md_ret &= redis_.connect_redis_instance();
	md_ret &= md_.initial_md_broadcast();

	return md_ret;
}

void CMdManager::release_md_manager()
{
	md_.release_md_broadcast();
	redis_.free_redis_instance();
}

bool CMdManager::open_consumer_thread()
{
	// Start One Consumer Thread
	thread_.set_data(this);
	return thread_.create_thread(controll_function);
}

void CMdManager::close_consumer_thread()
{
	thread_.set_stop(true);
	thread_.close_thread();
}

void CMdManager::subscribe_market(std::vector<std::string> ins)
{
	size_t ins_size = ins.size();

	if (0 == ins_size)
	{
		return;
	}

	intrusts_			= ins;
	char** instr	= new char*[ins_size];
	
	for (size_t i = 0; i < ins_size; ++i)
	{
		instr[i] = const_cast<char*>(intrusts_[i].c_str());
	}

	// Waiting For Login Successfully At First Forever, I'll Optimize It Sooner Or Later.
	while (true)
	{
		if (md_.get_ready_subscribe())
		{
			md_.subscribe_instruments(instr, (int)ins_size);
			break;
		}
	}
	
	delete[] instr;
	instr = nullptr;
}