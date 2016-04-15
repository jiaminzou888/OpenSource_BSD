#include "MdManager.h"
#include "EfficientMap.h"
#include "GLogWrapper.h"

#include <algorithm>


CMdManager::CMdManager()
{
	char module_dir[MAX_PATH + 1] = { 0 };
	::GetModuleFileName(nullptr, module_dir, MAX_PATH);
	root_log_ = module_dir;

	root_log_ = root_log_.substr(0, root_log_.find_last_of("\\")) + "\\glog_log";
}

void CMdManager::controll_function(void* data)
{
	CppThread*	handle_th = static_cast<CppThread*>(data);
	CMdManager* handle_ma = static_cast<CMdManager*>(handle_th->get_data());

	while (!handle_th->is_stop())
	{
		const strategy_list& stg_list = handle_ma->md_.get_strategy_list();
		for (auto stg : stg_list)
		{
			const instr_container& ins_vec = stg->get_focused_instr();
			for (auto ins : ins_vec)
			{
				stg->calculate_min_bar(ins);
			}
		}
	}
}

bool CMdManager::initial_md_manager()
{
	// initial capacity of vector
	intrusts_.reserve(64);

	bool md_ret = true;
	
	CGLog::get_glog()->init_log(root_log_.c_str());
	md_ret &= redis_.connect_redis_instance();
	md_ret &= md_.initial_md_broadcast();

	return md_ret;
}

void CMdManager::release_md_manager()
{
	md_.release_md_broadcast();
	redis_.free_redis_instance();
	CGLog::get_glog()->release_log();
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

bool CMdManager::subscribe_market(std::vector<std::string> ins)
{
	bool sub_ret = false;

	size_t ins_size = ins.size();

	if (0 == ins_size)
	{
		return sub_ret;
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
			sub_ret = md_.subscribe_instruments(instr, (int)ins_size);
			break;
		}
	}
	
	delete[] instr;
	instr = nullptr;

	return sub_ret;
}

void CMdManager::attach_md_strategy(CStrategy* stg)
{
	md_.attach_observer(stg);
}