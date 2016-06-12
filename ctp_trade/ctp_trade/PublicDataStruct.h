#pragma once

#include "SolveReDefinition.h"
// Has To Be Behind 'SolveReDefinition.h'
#include <cassert>
#include <ctime>
#include <fstream>

#include <map>
#include <list>
#include <vector>
#include <memory>

#include <mutex>
#include <condition_variable>

#include "ThostFtdcUserApiStruct.h"

// Action Signal
const int TECH_SIGNAL_MIN			= -15;
const int TECH_SIGNAL_SELL_INTENSE	= -15;
const int TECH_SIGNAL_SELL			= -10;

const int TECH_SIGNAL_NOTHING		= 0;

const int TECH_SIGNAL_BUY			= 10;
const int TECH_SIGNAL_BUY_INTENSE	= 15;
const int TECH_SIGNAL_MAX			= -15;

// Action Reason
const int TECH_REASON_NOTHING	= 1;

const int TECH_REASON_GOLDFORK	= 4;
const int TECH_REASON_DEADFORK	= 5;
const int TECH_REASON_LONG		= 6;
const int TECH_REASON_SHORT		= 7;

#define MIN(a,b)            (((a) < (b)) ? (a) : (b))
#define MAX(a,b)            (((a) > (b)) ? (a) : (b))

#define SIGNAL_ISBUY(x)		(x>0)
#define SIGNAL_ISSELL(x)	(x<0)

struct candle_bar
{
	mtk_data_name		bar_name;
	mtk_data_exchange	bar_ecg;

	size_t trade_day;
	size_t trade_time;

	double open_price;
	double high_price;
	double low_price;
	double close_price;

	double change_point;
	double change_percent;

	size_t volume_size;

	candle_bar()
	{
		memset(this, 0x00, sizeof(candle_bar));
	}
};

struct td_attribute
{
	CThostFtdcRspInfoField response_msg;

	int front_id;
	int session_id;
	int order_ref;
	char exchange_time[5][9];

	size_t trade_day;
	
	td_attribute()
	{
		memset(this, 0x00, sizeof(td_attribute));
	}
};

struct trade_handle
{
	std::mutex mtx;
	std::condition_variable cv;
	bool ready_flag{ false };

	void wait_handle()
	{
		std::unique_lock <std::mutex> lck(mtx);
		while (!ready_flag)
		{
			cv.wait(lck);
		}
	}

	void notify_handle()
	{
		std::unique_lock <std::mutex> lck(mtx);
		ready_flag = true;
		cv.notify_all();
	}
};

struct operation_attribute
{
	int action{ 0 };
	int quantity{ 0 };
	double price{ 0.0 };

	struct  
	{
		double	commission_rate{ 0.0 };
		int		point_spread{ 0 };
	}tax_fee;

	std::string instrument;
	std::mutex  lock_mtx;
};

struct tech_attribute
{
	int attribute_type{ 0 };
};

struct ma_attribute : public tech_attribute
{
	int flag_goldfork{ 0 };
	int flag_deadfork{ 0 };
	int flag_long{ 0 };
	int flag_short{ 0 };

	std::vector<int> period_cycle;

	ma_attribute()
	{
		period_cycle.reserve(5);
	}
};