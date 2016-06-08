#include "MdManager.h"

#include "EfficientMap.h"
#include "TradeManager.h"

#include <algorithm>


void CMdDataSet::initial_dataset(int type, std::vector<std::string>& ins)
{
	dataset_type	= type;

	for (std::string in_ : ins)
	{
		CCandleBar data;
		data.set_candle_type(dataset_type);
		efficient_map_operation(dataset, in_, data);
	}
}

void CMdDataSet::push_one_data(std::string ins, candle_bar& bar)
{
	dataset[ins].push_bar(bar);
}

bool CMdDataSet::calculate_specified_bar(std::string ins, size_t strat_pos, int compose_mutiple, candle_bar& des_bar)
{
	CCandleBar& original_bars = dataset[ins];

	return original_bars.calculate_specified_period(strat_pos, compose_mutiple, des_bar);
}

CMdManager::CMdManager(CTradeManager* trade_ptr)
: md_trade_pointer(trade_ptr)
, md_broadcast_(this)
{

}

// Calculate Decision Data That Based On One Minute KData
void CMdManager::calculate_md_base_function(void* data)
{
	CppThread*		handle_th	= static_cast<CppThread*>(data);
	CMdManager*		handle_md	= static_cast<CMdManager*>(handle_th->get_data());
	
	int	thread_id				= handle_th->get_thread_index();
	CMdDataSet&	base_one_minute = handle_md->basic_minute_kdata_[ONE];
	std::vector<std::string> focus_instr = handle_md->md_broadcast_.get_instruments();

	size_t retrieve_start_pos	= 0;
	int	   compose_multipile	= handle_md->checkup_table[thread_id];
	while (!handle_th->is_stop())
	{
		for_each(focus_instr.begin(), focus_instr.end(), [&](std::string& ins)
		{
			candle_bar compose_bar;
			bool calcu_ret = base_one_minute.calculate_specified_bar(ins, retrieve_start_pos, compose_multipile, compose_bar);
			if (calcu_ret)
			{
				// Calculation
				handle_md->basic_minute_kdata_[thread_id].push_one_data(ins, compose_bar);
				// Notification
				int k_type = handle_md->convert_datano_to_ktype(thread_id);
				handle_md->md_trade_pointer->notify_decision_data(k_type, compose_bar);
				// Position
				retrieve_start_pos += compose_multipile;
			}
		});
	}
}

bool CMdManager::initial_md_manager(std::vector<std::string>& ins)
{
	// initial capacity of vector
	bool md_ret = true;
	
	// Initialize Md Module
	md_ret &= initial_checkup_table();
	md_ret &= open_calculate_thread(ins);
	md_ret &= md_broadcast_.initial_md_broadcast(ins);

	return md_ret;
}

void CMdManager::release_md_manager()
{
	md_broadcast_.release_md_broadcast();
	close_calculate_thread();
}

bool CMdManager::get_md_conncet_status()
{
	return md_broadcast_.get_md_connect_flag();
}

bool CMdManager::subscribe_market()
{
	bool sub_ret = false;

	size_t ins_size = md_broadcast_.get_instruments_size();

	if (0 == ins_size)
	{
		return sub_ret;
	}

	// Subscribe MD Data
	char** instr	= new char*[ins_size];
	md_broadcast_.get_instrument_name(instr, ins_size);

	// Waiting For Login Successfully At First Forever, I'll Optimize It Sooner Or Later.
	while (true)
	{
		if (md_broadcast_.get_ready_subscribe())
		{
			sub_ret = md_broadcast_.subscribe_instruments(instr, (int)ins_size);
			break;
		}
	}
	
	delete[] instr;
	instr = nullptr;

	return sub_ret;
}

void CMdManager::push_min_one_data(std::string ins, candle_bar& bar)
{
	basic_minute_kdata_[ONE].push_one_data(ins, bar);

	// Notify The Strategy Which Focuses On One_Minute Directly Without An Extra Thread
	md_trade_pointer->notify_decision_data(CCandleBar::MIN_ONE, bar);
}

bool CMdManager::calculate_decision_kdata(int type)
{
	bool cal_ret = true;

	switch (type)
	{
	case CCandleBar::MIN_ONE:
		break;
	}

	return cal_ret;
}

bool CMdManager::initial_checkup_table()
{
	checkup_table[ONE]		= 1;
	checkup_table[FIVE]		= 5;
	checkup_table[FIVETEEN] = 15;
	checkup_table[THIRTY]	= 30;
	checkup_table[SIXTY]	= 60;

	return true;
}

int  CMdManager::convert_datano_to_ktype(int thread_id)
{
	int convert_type = -1;

	switch (thread_id)
	{
	case ONE:
		convert_type = CCandleBar::MIN_ONE;
		break;
	case FIVE:
		convert_type = CCandleBar::MIN_FIVE;
		break;
	case FIVETEEN:
		convert_type = CCandleBar::MIN_FIVETEEN;
		break;
	case THIRTY:
		convert_type = CCandleBar::MIN_THIRTY;
		break;
	case SIXTY:
		convert_type = CCandleBar::MIN_SIXTY;
		break;
	}

	return convert_type;
}

bool CMdManager::open_calculate_thread(std::vector<std::string>& ins)
{
	basic_minute_kdata_[ONE].initial_dataset(CCandleBar::MIN_ONE, ins);
	//calculate_thread_[ONE].set_data(this);
	//calculate_thread_[ONE].set_thread_index(ONE);
	//calculate_thread_[ONE].create_thread(calculate_md_base_function);

	basic_minute_kdata_[FIVE].initial_dataset(CCandleBar::MIN_FIVE, ins);
	calculate_thread_[FIVE].set_data(this);
	calculate_thread_[FIVE].set_thread_index(FIVE);
	calculate_thread_[FIVE].create_thread(calculate_md_base_function);

	basic_minute_kdata_[FIVETEEN].initial_dataset(CCandleBar::MIN_FIVETEEN, ins);
	calculate_thread_[FIVETEEN].set_data(this);
	calculate_thread_[FIVETEEN].set_thread_index(FIVETEEN);
	calculate_thread_[FIVETEEN].create_thread(calculate_md_base_function);

	basic_minute_kdata_[THIRTY].initial_dataset(CCandleBar::MIN_THIRTY, ins);
	calculate_thread_[THIRTY].set_data(this);
	calculate_thread_[THIRTY].set_thread_index(THIRTY);
	calculate_thread_[THIRTY].create_thread(calculate_md_base_function);

	basic_minute_kdata_[SIXTY].initial_dataset(CCandleBar::MIN_SIXTY, ins);
	calculate_thread_[SIXTY].set_data(this);
	calculate_thread_[SIXTY].set_thread_index(SIXTY);
	calculate_thread_[SIXTY].create_thread(calculate_md_base_function);

	return true;
}

void CMdManager::close_calculate_thread()
{
	//calculate_thread_[ONE].close_thread();
	calculate_thread_[FIVE].close_thread();
	calculate_thread_[FIVETEEN].close_thread();
	calculate_thread_[THIRTY].close_thread();
	calculate_thread_[SIXTY].close_thread();
}