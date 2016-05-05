#include "CandleBar.h"

CCandleBar::CCandleBar()
{
	candles_.reserve(256);
}

void CCandleBar::push_bar(candle_bar& bar)
{
	candles_.emplace_back(bar);
}

int	CCandleBar::convert_kdata(CCandleBar& k_src, CCandleBar& k_des, size_t mul_num)
{
	// Convert k_src Bars To k_des Bar According To Their Multiple
	if (mul_num < 2)
	{
		return 0;
	}
	


	return 0;
}