#pragma once

#include "SolveReDefinition.h"
// Has To Be Behind 'SolveReDefinition.h'
#include <deque>

#include "ThostFtdcMdApi.h"
#include "CppReadWriteLock.hpp"

using mtk_type = CThostFtdcDepthMarketDataField;

class CMdBroadCast : private CThostFtdcMdSpi
{
public:
	bool initial_md_broadcast();
	void release_md_broadcast();

	bool get_ready_subscribe();
	bool subscribe_instruments(char* *needed_ins, int ins_count);

	size_t get_mtk_size();
	const mtk_type get_mtk_head();
	void  pop_mtk_head();
	std::deque<mtk_type>  get_mtk_queue();

private:
	//当客户端与交易后台建立起通信连接时，即还未登录前，该方法被调用
	void OnFrontConnected() override;
	//登录请求响应
	void OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) override;
	//行情回报响应
	void OnRtnDepthMarketData(mtk_type *pDepthMarketData) override;

private:
	CThostFtdcMdApi* md_api_;

	bool is_ready_subscribe_{ false };

	//CRWLock	queue_lock_;	// (Donnot Need It When One Producer And One Consumer Temporarily)
	std::mutex mtx_;
	std::deque<mtk_type> mtk_queue_;
};