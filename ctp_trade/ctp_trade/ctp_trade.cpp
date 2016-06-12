// ctp_trade.cpp : 定义控制台应用程序的入口点。

#include "TradeManager.h"

int main(int argc, char* argv[])
{
	// Create Different Strategies
	int	ma_data_type = CCandleBar::MIN_FIVE | CCandleBar::MIN_ONE | CCandleBar::MIN_FIVETEEN;

	CMAStrategy ma_stg;
	ma_stg.initial_ma_stg("E:\\OpenSource_BSD\\ctp_trade\\x64\\Debug\\inst_config.ini", "MA_STG", ma_data_type);

	// Trader Manager
	CTradeManager trader;;
	trader.attach_trade_strategy(&ma_stg);

	// Try To Set Strategies List in initial
	trader.initial_trader();

	while (true)
	{
		int i = 0;
	}

	ma_stg.release_ma_stg();

	trader.detach_trade_strategy(&ma_stg);

	trader.release_trader();

	return 0;
}

