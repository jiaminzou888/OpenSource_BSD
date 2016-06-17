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

int CTdManager::limit_order_insert(const char* ins_id, char action_direct, char open_flag, double price, int quantity)
{
	CThostFtdcInputOrderField order_limit;
	memset(&order_limit, 0x00, sizeof(CThostFtdcInputOrderField));
	
	strncpy_s(order_limit.InstrumentID, ins_id, sizeof(order_limit.InstrumentID) - 1);
	order_limit.OrderPriceType = THOST_FTDC_OPT_LimitPrice;
	order_limit.Direction = action_direct;
	order_limit.CombOffsetFlag[0] = open_flag;
	order_limit.CombHedgeFlag[0] = THOST_FTDC_HF_Speculation;
	order_limit.LimitPrice = price;
	order_limit.VolumeTotalOriginal = quantity;
	order_limit.TimeCondition = THOST_FTDC_TC_GFD;
	order_limit.VolumeCondition = THOST_FTDC_VC_AV;
	order_limit.ContingentCondition = THOST_FTDC_CC_Immediately;

	return insert_order(order_limit);
}

int	 CTdManager::insert_order(CThostFtdcInputOrderField& order_req)
{
	order_req.ForceCloseReason = THOST_FTDC_FCC_NotForceClose;
	order_req.IsAutoSuspend = 0;
	order_req.UserForceClose = 0;

	// Do Order
	return td_.request_order_insert(order_req);
}