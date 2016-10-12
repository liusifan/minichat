
#pragma once

class LogicMonitor
{
public:
    static LogicMonitor * GetDefault();
    static void SetDefault(LogicMonitor * monitor);

    LogicMonitor();
    ~LogicMonitor();

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


