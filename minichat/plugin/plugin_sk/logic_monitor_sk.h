
#pragma once

#include "logic_monitor.h"

class LogicMonitorSK : public LogicMonitor
{
public:

    LogicMonitorSK();
    ~LogicMonitorSK();

    virtual void ReportAuthCount();
    virtual void ReportAuthSuccCount();
    virtual void ReportAuthFailCount();

    virtual void ReportMsgCount();
    virtual void ReportMsgSuccCount();
    virtual void ReportMsgFailCount();

    virtual void ReportCgiAuthCount();
    virtual void ReportCgiAuthSuccCount();
    virtual void ReportCgiAuthFailCount();

    virtual void ReportCgiSendmsgCount();
    virtual void ReportCgiSendmsgSuccCount();
    virtual void ReportCgiSendmsgFailCount();

    virtual void ReportCgiSyncCount();
    virtual void ReportCgiSyncSuccCount();
    virtual void ReportCgiSyncFailCount();

    virtual void ReportMsgClientCount();
    virtual void ReportMsgClientSuccCount();
    virtual void ReportMsgClientFailCount();

    virtual void ReportToolsSendMsgRunTime(int runtime_ms);

};


