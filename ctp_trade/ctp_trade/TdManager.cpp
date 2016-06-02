#include "TdManager.h"

#include "TradeManager.h"

CTdManager::CTdManager(CTradeManager* trade_ptr)
: td_trade_pointer(trade_ptr)
, td_(this)
{

}

bool CTdManager::initial_td_manager()
{
	bool td_ret = true;

	td_ret &= td_.initial_td_trade();

	return td_ret;
}

void CTdManager::release_td_manager()
{
	td_.release_td_trade();
}

int CTdManager::execute_login_confirm()
{
	int exc_ret = 0;

	// Waiting For Login Successfully At First Forever, I'll Optimize It Sooner Or Later.
	while (true)
	{
		if (td_.get_td_connect_flag())
		{
			// user login
			exc_ret += td_.request_user_login();
			if (0 == exc_ret)
			{
				td_.wait_td_event();
			}
			else
			{
				break;
			}

			// user settlement confirmation
			exc_ret += td_.request_settle_confirm();
			if (0 == exc_ret)
			{
				td_.wait_td_event();
			}
			else
			{
				break;
			}

			break;
		}
	}

	return exc_ret;
}