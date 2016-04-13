#pragma once

#include "SolveReDefinition.h"
// Has To Be Behind 'SolveReDefinition.h'
#include <vector>

#include "ThostFtdcUserApiStruct.h"

enum k_types
{
	K_NONE		= 0x00,
	K_MIN		= 0x01,
	K_MIN5		= 0x02,
	K_MIN15		= 0x03,
	K_MIN30		= 0x04,
	K_HOUR		= 0x05,
	K_DAY		= 0x06,
	K_WEEK		= 0x07,
	K_MONTH		= 0x08,
};


struct candle_bar
{
	mtk_data_name		bar_name;
	mtk_data_exchange	bar_ecg;

	size_t bar_type;	// Min_K, 5Min_K, ....
	size_t trade_day;

	double open_price;
	double high_price;
	double low_price;
	double close_price;

	double change_point;
	double change_percent;

	double turn_over;

	candle_bar()
	{
		memset(this, 0x00, sizeof(candle_bar));
	}
};

class CCandleBar
{
public:
	CCandleBar();

	int	convert_kdata(CCandleBar& k_src, CCandleBar& k_des, size_t mul_num);
	std::vector<candle_bar>& get_candle_container();

private:
	std::vector<candle_bar> candle_container;
};