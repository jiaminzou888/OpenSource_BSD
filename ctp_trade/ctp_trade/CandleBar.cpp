#include "CandleBar.h"

CCandleBar::CCandleBar()
{
	candle_container.reserve(512);
}

int	CCandleBar::convert_kdata(CCandleBar& k_src, CCandleBar& k_des, size_t mul_num)
{
	return 0;
}

std::vector<candle_bar>& CCandleBar::get_candle_container()
{
	return candle_container;
}