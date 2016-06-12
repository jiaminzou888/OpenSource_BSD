#include "CandleBar.h"

#include <algorithm>

CCandleBar::CCandleBar()
{
	candle_type = MIN_ONE;

	candles_.reserve(256);
	mutex_ = std::move(std::shared_ptr<std::mutex>(new std::mutex));
}

void CCandleBar::set_candle_type(int type)
{
	candle_type = type;
}

int	CCandleBar::get_candle_type()
{
	return candle_type;
}

size_t	CCandleBar::get_candle_size()
{
	std::lock_guard<std::mutex> lck(*mutex_);
	return candles_.size();
}

void CCandleBar::push_bar(candle_bar& bar)
{
	std::lock_guard<std::mutex> lck(*mutex_);
	candles_.emplace_back(bar);
}

bool CCandleBar::calculate_specified_period(size_t strat_pos, int compose_mutiple, candle_bar& des_bar)
{
	std::lock_guard<std::mutex> lck(*mutex_);

	if (candles_.size() > strat_pos + compose_mutiple)
	{
		int src_count = 0;

		auto start_iterator = candles_.begin() + strat_pos;
		for_each(start_iterator, start_iterator + compose_mutiple, [&src_count, &des_bar, compose_mutiple](candle_bar& src_bar)
		{
			src_count++;
			if (1 == src_count)
			{
				memcpy_s(&des_bar, sizeof(des_bar), &src_bar, sizeof(des_bar));
			}
			else
			{
				des_bar.volume_size += src_bar.volume_size;

				des_bar.close_price = src_bar.close_price;

				if (abs(des_bar.high_price - src_bar.high_price) < 0.000001)
				{
					des_bar.high_price = src_bar.high_price;
				}

				if (abs(des_bar.low_price - src_bar.low_price) > 0.000001)
				{
					des_bar.low_price = src_bar.low_price;
				}
			}

			if (compose_mutiple == src_count)
			{
				des_bar.change_point = des_bar.close_price - des_bar.open_price;
				des_bar.change_percent = (des_bar.change_point / des_bar.open_price) * 100.;

				src_count = 0;
			}
		});

		return true;
	}
	else
	{
		return false;
	}
}

void CCandleBar::get_bars_and_clear(std::vector<candle_bar>& target)
{
	std::lock_guard<std::mutex> lck(*mutex_);

	if (!candles_.empty())
	{
		target.insert(target.end(), candles_.begin(), candles_.end());
		candles_.clear();
	}
}

CCandleBar& CCandleBar::operator + (const std::vector<candle_bar>& target)
{
	std::lock_guard<std::mutex> lck(*mutex_);
	this->insert_candles_backward(target);

	return *this;
}

void CCandleBar::convert_kdata_and_clear(size_t compose_mutiple, CCandleBar& des_candles)
{
	std::lock_guard<std::mutex> lck(*mutex_);

	size_t candle_nums = candles_.size();

	if (compose_mutiple < 2 || candle_nums < compose_mutiple)
	{
		return;
	}

	// Total Calculation And Replacement
	int src_count = 0;
	candle_bar des_bar;
	size_t end_pos = candle_nums % compose_mutiple;

	std::for_each(candles_.begin(), candles_.end() - end_pos, [&src_count, &des_bar, &compose_mutiple, &des_candles](candle_bar& src_bar)
	{
		src_count ++;
		if (1 == src_count)
		{
			memcpy_s(&des_bar, sizeof(des_bar), &src_bar, sizeof(des_bar));
		}
		else
		{
			des_bar.volume_size += src_bar.volume_size;

			des_bar.close_price = src_bar.close_price;

			if (abs(des_bar.high_price - src_bar.high_price) < 0.000001)
			{
				des_bar.high_price = src_bar.high_price;
			}

			if (abs(des_bar.low_price - src_bar.low_price) > 0.000001)
			{
				des_bar.low_price = src_bar.low_price;
			}
		}

		if (compose_mutiple == src_count)
		{
			des_bar.change_point	= des_bar.close_price - des_bar.open_price;
			des_bar.change_percent	= (des_bar.change_point / des_bar.open_price) * 100.;

			src_count = 0;
			des_candles.push_bar(des_bar);
		}
	});
	
	candles_.erase(candles_.begin(), candles_.end() - end_pos);
}

bool CCandleBar::get_ma(size_t date_index, int periods, int type, double& data)
{
	std::lock_guard<std::mutex> lck(*mutex_);

	if (date_index < 0 || date_index >= candles_.size() || periods <= 0 || periods > date_index + 1)
	{
		return false;
	}
	
	int		k_count = 0;
	double	ma_data = 0;

	for (size_t i = date_index; i >= 0; --i)
	{
		ma_data += get_price(date_index, type);
		k_count++;
		if (periods == k_count)
		{
			data = ma_data / periods;
			return true;
		}
	}

	return false;
}

void CCandleBar::insert_candles_backward(const std::vector<candle_bar>& target)
{
	candles_.insert(candles_.end(), target.begin(), target.end());
}

double CCandleBar::get_price(size_t date_index, int type)
{
	candle_bar& bar = candles_[date_index];

	double price = 0;
	switch (type)
	{
	case OPEN:
		price = bar.open_price;
		break;
	case CLOSE:
		price = bar.close_price;
		break;
	case HIGH:
		price = bar.high_price;
		break;
	case LOW:
		price = bar.low_price;
		break;
	default:
		abort();
	}

	return price;
}