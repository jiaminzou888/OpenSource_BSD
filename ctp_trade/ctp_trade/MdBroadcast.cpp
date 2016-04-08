#include "MdBroadcast.h"

bool CMdBroadCast::initial_md_broadcast()
{
	bool md_ret = true;

	md_api_ = CThostFtdcMdApi::CreateFtdcMdApi("./spi_con/"); // 'spi_con' has to already been prepared in advance
	if (md_api_ != nullptr)
	{
		md_api_->RegisterSpi(this);
		md_api_->RegisterFront("tcp://180.168.146.187:10010");
		md_api_->Init();
	}
	else
	{
		md_ret = false;
	}

	return md_ret;
}

void CMdBroadCast::release_md_broadcast()
{
	if (md_api_ != nullptr)
	{
		md_api_->RegisterSpi(nullptr);
		md_api_->Release();
		md_api_ = nullptr;
	}
}

bool CMdBroadCast::get_ready_subscribe()
{
	return is_ready_subscribe_;
}

bool CMdBroadCast::subscribe_instruments(char* *needed_ins, int ins_count)
{
	bool sub_ret = true;

	if (is_ready_subscribe_)
	{
		md_api_->SubscribeMarketData(needed_ins, ins_count);
	}
	else
	{
		sub_ret = false;
	}

	return sub_ret;
}

size_t CMdBroadCast::get_mtk_size()
{
	std::lock_guard<std::mutex> lck(mtx_);
	return mtk_queue_.size();
}

const mtk_type CMdBroadCast::get_mtk_head()
{
	std::lock_guard<std::mutex> lck(mtx_);
	return mtk_queue_.front();
}

void  CMdBroadCast::pop_mtk_head()
{
	std::lock_guard<std::mutex> lck(mtx_);
	mtk_queue_.pop_front();
}

std::deque<mtk_type>  CMdBroadCast::get_mtk_queue()
{
	std::lock_guard<std::mutex> lck(mtx_);
	std::deque<mtk_type> temp_one(mtk_queue_.begin(), mtk_queue_.end());
	mtk_queue_.clear();
	return temp_one;
}


/*****************************spi»Øµ÷º¯Êý\***********************************************/
void CMdBroadCast::OnFrontConnected() 
{
	CThostFtdcReqUserLoginField loginField;
	strncpy_s(loginField.BrokerID,	"", sizeof(loginField.BrokerID) - 1);
	strncpy_s(loginField.UserID,	"",	sizeof(loginField.UserID) - 1);
	strncpy_s(loginField.Password,	"",	sizeof(loginField.Password) - 1);

	md_api_->ReqUserLogin(&loginField, 0);
}


void CMdBroadCast::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) 
{
	if (pRspInfo->ErrorID == 0)
	{
		is_ready_subscribe_ = true;
	}
	else
	{
		abort();
	}
}


void CMdBroadCast::OnRtnDepthMarketData(mtk_type *pDepthMarketData)
{
	// I  Prefer To Store Market Data Directly Here Than To Do Any Process, Especially Time-Consuming One.
	std::lock_guard<std::mutex> lck(mtx_);
	mtk_queue_.emplace_back(*pDepthMarketData);
	// Using Producer And Consumer Pattern and Do As The Only One Producer.
}