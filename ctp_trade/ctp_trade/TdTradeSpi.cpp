#include "TdTradeSpi.h"
#include "TdManager.h"

#include <string>

CTdTradeSpi::CTdTradeSpi(CTdManager* manager_ptr)
: manager_pointer(manager_ptr)
{

}

bool CTdTradeSpi::initial_td_trade()
{
	bool td_ret = true;

	td_api_ = CThostFtdcTraderApi::CreateFtdcTraderApi("./spi_con/td/");
	if (td_api_ != nullptr)
	{
		td_api_->RegisterSpi(this);
		td_api_->SubscribePublicTopic(THOST_TERT_QUICK);
		td_api_->SubscribePrivateTopic(THOST_TERT_QUICK);
		td_api_->RegisterFront("tcp://180.168.146.187:10000");
		td_api_->Init();
	}
	else
	{
		td_ret = false;
	}

	return td_ret;
}

void CTdTradeSpi::release_td_trade()
{
	if (td_api_)
	{
		td_api_->RegisterSpi(nullptr);
		td_api_->Release();
		td_api_ = nullptr;
	}
}

void CTdTradeSpi::wait_td_event()
{
	td_event.wait_handle();
}

void CTdTradeSpi::notify_td_event()
{
	td_event.notify_handle();
}

bool CTdTradeSpi::get_td_connect_flag()
{
	return connect_td_flag_;
}

bool CTdTradeSpi::is_error_rsp(CThostFtdcRspInfoField* rsp)
{
	return (rsp && rsp->ErrorID != 0);
}

int CTdTradeSpi::request_user_login()
{
	CThostFtdcReqUserLoginField login_field;
	memset(&login_field, 0x00, sizeof(CThostFtdcReqUserLoginField));

	strncpy_s(login_field.BrokerID, "9999",		sizeof(login_field.BrokerID) - 1);
	strncpy_s(login_field.UserID,	"057131",	sizeof(login_field.UserID) - 1);
	strncpy_s(login_field.Password, "1998two",	sizeof(login_field.Password) - 1);

	return td_api_->ReqUserLogin(&login_field, request_id++);
}

int CTdTradeSpi::request_settle_confirm()
{
	CThostFtdcSettlementInfoConfirmField confirm_field;
	memset(&confirm_field, 0x00, sizeof(CThostFtdcSettlementInfoConfirmField));

	strncpy_s(confirm_field.BrokerID, "9999", sizeof(confirm_field.BrokerID) - 1);
	strncpy_s(confirm_field.InvestorID, "057131", sizeof(confirm_field.InvestorID) - 1);
	
	return td_api_->ReqSettlementInfoConfirm(&confirm_field, request_id++);
}

void CTdTradeSpi::OnFrontConnected()
{
	connect_td_flag_ = true;
}

void CTdTradeSpi::OnFrontDisconnected(int nReason)
{
	connect_td_flag_ = false;
}

void CTdTradeSpi::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if (bIsLast && !is_error_rsp(pRspInfo) && pRspUserLogin)
	{
		td_params.front_id = pRspUserLogin->FrontID;
		td_params.session_id = pRspUserLogin->SessionID;
		td_params.order_ref = atoi(pRspUserLogin->MaxOrderRef);
		td_params.trade_day = atoi(pRspUserLogin->TradingDay);
		
		strcpy(td_params.exchange_time[SHFE], pRspUserLogin->SHFETime);
		strcpy(td_params.exchange_time[DCE],  pRspUserLogin->DCETime);
		strcpy(td_params.exchange_time[CZCE], pRspUserLogin->CZCETime);
		strcpy(td_params.exchange_time[FFEX], pRspUserLogin->FFEXTime);
		strcpy(td_params.exchange_time[INE],  pRspUserLogin->INETime);

		notify_td_event();
	}
	else
	{
		if (pRspInfo)
		{
			int copy_rsp_size = sizeof(CThostFtdcRspInfoField)-1;
			memcpy_s(&(td_params.response_msg), copy_rsp_size, pRspInfo, copy_rsp_size);
		}
	}
}

void CTdTradeSpi::OnRspUserLogout(CThostFtdcUserLogoutField *pUserLogout, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	
}

void CTdTradeSpi::OnRspSettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if (bIsLast && !is_error_rsp(pRspInfo) && pSettlementInfoConfirm)
	{
		notify_td_event();
	}
	else
	{
		if (pRspInfo)
		{
			int copy_rsp_size = sizeof(CThostFtdcRspInfoField)-1;
			memcpy_s(&(td_params.response_msg), copy_rsp_size, pRspInfo, copy_rsp_size);
		}
	}
}