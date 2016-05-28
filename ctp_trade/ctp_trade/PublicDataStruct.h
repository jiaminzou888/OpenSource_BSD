#pragma once

#include "SolveReDefinition.h"
// Has To Be Behind 'SolveReDefinition.h'
#include <cassert>
#include <map>
#include <mutex>
#include <vector>
#include <memory>

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