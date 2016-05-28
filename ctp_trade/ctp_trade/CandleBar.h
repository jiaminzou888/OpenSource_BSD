#pragma once

#include "PublicDataStruct.h"

class CCandleBar
{
public:
	CCandleBar();

	enum
	{
		OPEN	= 0x01,
		CLOSE	= 0x02,
		HIGH	= 0x03,
		LOW		= 0x04
	};

	void	push_bar(candle_bar& bar);
	size_t	candle_size();

	// Generic Conversion Between Different Periods
	void	convert_kdata(CCandleBar& des_candles, size_t multiple);

	// Get Specific Type Of Price On One Day
	double get_price(int date_index, int type);
	// Calculate Moving Average From The Day Pointed By date_index To One periods Forwarded
	bool   get_ma(int date_index, int periods, int type, double& data);

private:
	std::shared_ptr<std::mutex> mutex_;
	std::vector<candle_bar>		candles_;
};