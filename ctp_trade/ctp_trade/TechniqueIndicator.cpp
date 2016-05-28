#include "TechniqueIndicator.h"


CTechniqueIndicator::CTechniqueIndicator(CCandleBar& data)
: base_kdata(&data)
{

}

CTechniqueIndicator::~CTechniqueIndicator()
{

}

bool CTechniqueIndicator::valid_tech_signal(int signal)
{
	return (signal >= TECH_SIGNAL_MIN && signal <= TECH_SIGNAL_MAX);
}

bool CTechniqueIndicator::asset_calculate_parameters(int date_index, int periods)
{
	if (!base_kdata.get() || date_index < 0 || date_index >= base_kdata->candle_size() || periods <= 0)
	{
		return false;
	}
	if (!is_valid_parameters())
	{
		set_default_parameters();
	}

	return true;
}

// The Size Of period_cycle Is 3 In Default
int CTechniqueIndicator::get_fork_signal(int date_index, std::vector<int> period_cycle, int type, int gold_fork, int dead_fork, int& action_reason)
{
	action_reason = TECH_REASON_NOTHING;

	if (date_index <= 0)
	{
		return TECH_SIGNAL_NOTHING;
	}

	// Calculate Moving Average In Different Periods
	double last_minor, last_major, last_more;
	double now_minor, now_major, now_more;
	last_minor = last_major = last_more = 0;
	now_minor = now_major = now_more = 0;

	for (int i = 1; i < period_cycle.size(); ++i)
	{
		// date_index Represents Today, min Represents Short Period
		if (!calculate(date_index, MIN(period_cycle[i - 1], period_cycle[i]), type, false, now_minor)
			|| !calculate(date_index, MAX(period_cycle[i - 1], period_cycle[i]), type, false, now_major)
			|| !calculate(date_index - 1, MIN(period_cycle[i - 1], period_cycle[i]), type, false, last_minor)
			|| !calculate(date_index - 1, MAX(period_cycle[i - 1], period_cycle[i]), type, false, last_major))
		{
			return TECH_SIGNAL_NOTHING;
		}

		if (last_minor < now_minor && last_major > now_major
			&& last_major > last_minor && now_minor >= now_major)
		{
			if (i == period_cycle.size() - 1)
			{
				action_reason = TECH_REASON_GOLDFORK;
				return gold_fork;
			}
			for (int l = i + 1; l < period_cycle.size(); ++l)
			{
				if (!calculate(date_index, period_cycle[l], type, false, now_more)
					|| !calculate(date_index - 1, period_cycle[l], type, false, last_more))
				{
					return TECH_SIGNAL_NOTHING;
				}

				if (now_more > last_more)
				{
					action_reason = TECH_REASON_GOLDFORK;
					return gold_fork;
				}
			}
		}

		if (last_minor > now_minor && last_major < now_major
			&& last_major < last_minor &&  now_minor <= now_major)
		{
			if (i == period_cycle.size() - 1)
			{
				action_reason = TECH_REASON_DEADFORK;
				return dead_fork;
			}
			for (int l = i + 1; l < period_cycle.size(); ++l)
			{
				if (!calculate(date_index, period_cycle[l], type, false, now_more)
					|| !calculate(date_index - 1, period_cycle[l], type, false, last_more))
				{
					return TECH_SIGNAL_NOTHING;
				}

				if (now_more < last_more)
				{
					action_reason = TECH_REASON_DEADFORK;
					return dead_fork;
				}
			}
		}
		
	}

	return TECH_SIGNAL_NOTHING;
}

int CTechniqueIndicator::get_trend_signal(int date_index, std::vector<int> period_cycle, int type, int dir_long, int dir_short, int& action_reason)
{
	action_reason = TECH_REASON_NOTHING;

	if (date_index <= 0)
	{
		return TECH_SIGNAL_NOTHING;
	}

	// Calculate Moving Average In Different Periods
	double last_minor, last_major;
	double now_minor, now_major;
	last_minor = last_major = 0;
	now_minor = now_major = 0;

	int trend_direct = TECH_SIGNAL_NOTHING;
	for (int i = 1; i < period_cycle.size(); ++i)
	{
		// date_index Represents Today, min Represents Short Period
		if (!calculate(date_index, MIN(period_cycle[i - 1], period_cycle[i]), type, false, now_minor)
			|| !calculate(date_index, MAX(period_cycle[i - 1], period_cycle[i]), type, false, now_major)
			|| !calculate(date_index - 1, MIN(period_cycle[i - 1], period_cycle[i]), type, false, last_minor)
			|| !calculate(date_index - 1, MAX(period_cycle[i - 1], period_cycle[i]), type, false, last_major))
		{
			return TECH_SIGNAL_NOTHING;
		}

		if (now_minor > last_minor && now_major > last_major
			&& now_minor > now_major && (now_minor - now_major) > (last_minor - last_major)
			&& (1 == i || SIGNAL_ISBUY(trend_direct)))
		{
			action_reason	= TECH_REASON_LONG;
			trend_direct	= dir_long;
		}
		else if (now_minor < last_minor && now_major < last_major
			&& now_minor < now_major && (now_minor - now_major) < (last_minor - last_major)
			&& (1 == i || SIGNAL_ISSELL(trend_direct)))
		{
			action_reason	= TECH_REASON_SHORT;
			trend_direct	= dir_short;
		}
		else
		{
			action_reason	= TECH_REASON_NOTHING;
			trend_direct	= TECH_SIGNAL_NOTHING;
		}
	}

	return trend_direct;
}

bool CTechniqueIndicator::is_valid_parameters()
{
	return false;
}

void CTechniqueIndicator::set_default_parameters()
{

}

void CTechniqueIndicator::attach_parameters(tech_attribute* attr)
{

}

// operation
int  CTechniqueIndicator::get_signal(int date_index, int type, int& reason)
{
	reason = TECH_REASON_NOTHING;
	return TECH_SIGNAL_NOTHING;
}

bool CTechniqueIndicator::get_minmax_info(int beg_date, int end_date, double& min_data, double& max_data)
{
	return false;
}

bool CTechniqueIndicator::calculate(int date_index, int periods, int type, bool use_last, double& data)
{
	return false;
}


//////////////////////////////////////////////////////////////////////////
CMovingAverage::CMovingAverage(CCandleBar& data)
: CTechniqueIndicator(data)
{

}

CMovingAverage::~CMovingAverage()
{

}

bool CMovingAverage::is_valid_parameters()
{
	return (((TYPE_MA == attribute.attribute_type || TYPE_MA_EXP == attribute.attribute_type)
		&& (valid_tech_signal(attribute.flag_goldfork) && valid_tech_signal(attribute.flag_deadfork))
		&& (valid_tech_signal(attribute.flag_long) && valid_tech_signal(attribute.flag_short))
		&& attribute.period_cycle.size() > 0));
}

void CMovingAverage::set_default_parameters()
{
	attribute.attribute_type	= TYPE_MA;
	attribute.flag_goldfork		= TECH_SIGNAL_BUY_INTENSE;
	attribute.flag_deadfork		= TECH_SIGNAL_SELL_INTENSE;
	attribute.flag_long			= TECH_SIGNAL_BUY;
	attribute.flag_short		= TECH_SIGNAL_SELL;

	attribute.period_cycle.clear();
	attribute.period_cycle.push_back(5);
	attribute.period_cycle.push_back(10);
	attribute.period_cycle.push_back(20);
}

void CMovingAverage::attach_parameters(tech_attribute* attr)
{
	ma_attribute* ma_attr = (ma_attribute*)(attr);

	attribute.attribute_type	= ma_attr->attribute_type;

	attribute.flag_goldfork		= ma_attr->flag_goldfork;
	attribute.flag_deadfork		= ma_attr->flag_deadfork;
	attribute.flag_long			= ma_attr->flag_long;
	attribute.flag_short		= ma_attr->flag_short;

	attribute.period_cycle.clear();
	attribute.period_cycle.assign(ma_attr->period_cycle.begin(), ma_attr->period_cycle.end());
}

/*
Calculation Algorithm:
1. MA
MA = General Arithmetic Average

2. MA_EXP
MA_EXP(1) = CLOSE(1)
MA_EXP(i) = (1-a)MA_EXP(i-1) + aCLOSE(i)
Note: a = 2 / (n+1)
*/
bool CMovingAverage::calculate(int date_index, int periods, int type, bool use_last, double& data)
{
	if (!asset_calculate_parameters(date_index, periods))
	{
		return false;
	}
	if (periods > date_index + 1)
	{
		return false;
	}

	double ma_data = 0;
	switch (attribute.attribute_type)
	{
	case TYPE_MA:
	{
		return base_kdata->get_ma(date_index, periods, type, data);
	}
		break;
	case TYPE_MA_EXP:
	{
		if (use_last && data != 0)
		{
			if (0 == date_index)
			{
				ma_data = base_kdata->get_price(date_index, type);
			} 
			else
			{
				ma_data = data*(periods - 1.) / (periods + 1.) + base_kdata->get_price(date_index, type) * 2. / (periods + 1);
			}
		}
		else
		{
			for (int i = 0; i <= date_index; ++i)
			{
				if (0 == i)
				{
					ma_data = base_kdata->get_price(i, type);
				}
				else
				{
					ma_data = ma_data*(periods - 1.) / (periods + 1.) + base_kdata->get_price(date_index, type) * 2. / (periods + 1);
				}
			}
		}

		data = ma_data;
	}
		break;
	default:
		abort();
	}

	return true;
}

int  CMovingAverage::get_signal(int date_index, int type, int& reason)
{
	// If Signal Struck The Gold Or Dead Fork
	int signal = get_fork_signal(date_index, attribute.period_cycle, type, attribute.flag_goldfork, attribute.flag_deadfork, reason);
	if (SIGNAL_ISBUY(signal) || SIGNAL_ISSELL(signal))
	{
		return signal;
	}
	// Or Not
	return get_trend_signal(date_index, attribute.period_cycle, type, attribute.flag_long, attribute.flag_short, reason);
}

bool CMovingAverage::get_minmax_info(int beg_date, int end_date, double& min_data, double& max_data)
{
	return false;
}
