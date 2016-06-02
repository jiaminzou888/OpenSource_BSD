#pragma once

#include "PublicDataStruct.h"

class CCandleBar
{
public:
	enum
	{
		OPEN	= 0x01,
		CLOSE	= 0x02,
		HIGH	= 0x03,
		LOW		= 0x04
	};

	enum
	{
		MIN_ONE			= 0x01,
		MIN_FIVE		= 0x02,
		MIN_FIVETEEN	= 0x04,
		MIN_THIRTY		= 0x08,
	};

	CCandleBar();

	// Utility
	void	set_candle_type(int type);
	int		get_candle_type();

	size_t	get_candle_size();
	void	push_bar(candle_bar& bar);
	void	get_bars(std::vector<candle_bar>& target);
	CCandleBar&	operator + (const std::vector<candle_bar>& target);

	// Generic Conversion Between Different Periods
	void convert_kdata(CCandleBar& des_candles, size_t multiple);
	// Get Specific Type Of Price On One Day
	double get_price(int date_index, int type);
	// Calculate Moving Average From The Day Pointed By date_index To One periods Forwarded
	bool   get_ma(int date_index, int periods, int type, double& data);

private:
	void insert_candles_backward(const std::vector<candle_bar>& target);

private:
	int	candle_type;
	std::shared_ptr<std::mutex> mutex_;
	std::vector<candle_bar>		candles_;
};