/*************************************************************************
 * Copyright (c) 2014 eProsima. All rights reserved.
 *
 * This copy of eProsima RTPS is licensed to you under the terms described in the
 * EPROSIMARTPS_LIBRARY_LICENSE file included in this distribution.
 *
 *************************************************************************/

/**
 * @file WriterProxyLiveliness.cpp
 *
 */

#include "eprosimartps/reader/timedevent/WriterProxyLiveliness.h"
#include "eprosimartps/reader/StatefulReader.h"
#include "eprosimartps/reader/WriterProxy.h"
#include "eprosimartps/resources/ResourceEvent.h"
#include "eprosimartps/utils/RTPSLog.h"



namespace eprosima {
namespace rtps {

WriterProxyLiveliness::WriterProxyLiveliness(WriterProxy* wp,boost::posix_time::milliseconds interval):
				TimedEvent(&wp->mp_SFR->mp_event_thr->io_service,interval),
				mp_WP(wp)
{

}

WriterProxyLiveliness::~WriterProxyLiveliness()
{
	timer->cancel();
	delete(timer);
}

void WriterProxyLiveliness::event(const boost::system::error_code& ec)
{
	m_isWaiting = false;
	if(ec == boost::system::errc::success)
	{
	
		pDebugInfo(MAGENTA<<"WriterProxyLiveliness: checking Writer: "<<mp_WP->param.remoteWriterGuid<<DEF<<endl;);
		if(!mp_WP->checkLiveliness())
		{
			pWarning("WriterProxyLiveliness failed, leaseDuration was "<< this->getIntervalMsec()<< " ms"<< endl;);
			mp_WP->mp_SFR->matched_writer_remove(mp_WP->param.remoteWriterGuid);
			return;
		}
		this->restart_timer();
	}
	else if(ec==boost::asio::error::operation_aborted)
	{
		pInfo("WriterProxyLiveliness aborted"<<endl);
	}
	else
	{
		pDebugInfo("WriterProxyLiveliness boost message: " <<ec.message()<<endl);
	}
}



} /* namespace rtps */
} /* namespace eprosima */