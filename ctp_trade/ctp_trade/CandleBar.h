#pragma once

#include "PublicDataStruct.h"


class CCandleBar
{
public:
	CCandleBar();

	static int	convert_kdata(CCandleBar& k_src, CCandleBar& k_des, size_t mul_num);

	void push_bar(candle_bar& bar);

	

private:
	std::vector<candle_bar> candles_;
};