#pragma once

#include "PublicDataStruct.h"
#include "ThostFtdcTraderApi.h"

class CTdManager; // Forward Declaration For Management Pointer

class CTdTradeSpi : private CThostFtdcTraderSpi
{
public:
	enum 
	{
		SHFE	= 0x00,
		DCE		= 0x01,
		CZCE	= 0x02,
		FFEX	= 0x03,
		INE		= 0x04
	};

public:
	CTdTradeSpi(CTdManager* manager_ptr);

	bool initial_td_trade();
	void release_td_trade();
	void wait_td_event();
	void notify_td_event();
	bool get_td_connect_flag();

	int request_user_login();
	int request_settle_confirm();

private:
	bool	connect_td_flag_{ false };
	int		request_id{ 0 };

	trade_handle td_event;
	td_attribute td_params;

private:
	bool is_error_rsp(CThostFtdcRspInfoField *rsp);

private:
	///当客户端与交易后台建立起通信连接时（还未登录前），该方法被调用
	void OnFrontConnected();
	///当客户端与交易后台通信连接断开时，该方法被调用。
	void OnFrontDisconnected(int nReason);
	///登录请求响应
	void OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	///登出请求响应
	void OnRspUserLogout(CThostFtdcUserLogoutField *pUserLogout, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	///投资者结算结果确认响应
	void OnRspSettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

private:
	CThostFtdcTraderApi*		td_api_{ nullptr };
	std::shared_ptr<CTdManager>	manager_pointer;	// TD-Interface Management
};