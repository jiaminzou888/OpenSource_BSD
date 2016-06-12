#pragma once

#include "MdBroadCast.h"
#include "CppThread.hpp"
#include "CandleBar.h"

class CTradeManager; // Forward Declaration For Management Pointer

class CMdDataSet
{
public:
	void initial_dataset(int type, std::vector<std::string>& ins);
	void push_one_data(std::string ins, candle_bar& bar);

	bool calculate_specified_bar(std::string ins, size_t strat_pos, int compose_mutiple, candle_bar& des_bar);
private:
	int dataset_type{ 0 };
	std::map<std::string, CCandleBar> dataset;
};


// All Period-Circle KData's Management For All Interested Instruments
// Note. Maintaining All Data A Day.
class CMdManager
{
public:
	enum
	{
		ONE			= 0x00,
		FIVE		= 0x01,
		FIVETEEN	= 0x02,
		THIRTY		= 0x03,
		SIXTY		= 0x04
	};

	CMdManager(CTradeManager* trade_ptr);

	static void calculate_md_base_function(void* data);

	bool initial_md_manager(std::vector<std::string>& ins);
	void release_md_manager();

	bool get_md_conncet_status();
	bool subscribe_market();

	void push_min_one_data(std::string ins, candle_bar& bar);

	bool calculate_decision_kdata(int type);

private:
	bool initial_checkup_table();
	int  convert_datano_to_ktype(size_t thread_id);

	bool open_calculate_thread(std::vector<std::string>& ins);
	void close_calculate_thread();

private:
	std::map<size_t, int>	checkup_table;			// Based On One Minute

	CppThread	 calculate_thread_[5];
	CMdDataSet	 basic_minute_kdata_[5];

	CMdBroadCast md_broadcast_;

	std::shared_ptr<CTradeManager>	md_trade_pointer;	// Trade Management
};