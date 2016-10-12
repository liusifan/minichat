

#include "logic_monitor.h"

static LogicMonitor * g_monitor = nullptr;

LogicMonitor * LogicMonitor::GetDefault() 
{
    static LogicMonitor monitor;
    if(!g_monitor) {
        return &monitor;
    } else {
        return g_monitor;
    }

}

void LogicMonitor::SetDefault(LogicMonitor * monitor)
{
    g_monitor = monitor;
}

LogicMonitor::LogicMonitor()
{

}

LogicMonitor::~LogicMonitor()
{

}

void LogicMonitor::ReportAuthCount()
{
}

void LogicMonitor::ReportAuthSuccCount()
{
}

void LogicMonitor::ReportAuthFailCount()
{
}

void LogicMonitor::ReportMsgCount()
{
}

void LogicMonitor::ReportMsgSuccCount()
{
}

void LogicMonitor::ReportMsgFailCount()
{
}

void LogicMonitor::ReportCgiAuthCount()
{
}

void LogicMonitor::ReportCgiAuthSuccCount()
{
}

void LogicMonitor::ReportCgiAuthFailCount()
{
}

void LogicMonitor::ReportCgiSendmsgCount()
{
}

void LogicMonitor::ReportCgiSendmsgSuccCount()
{
}

void LogicMonitor::ReportCgiSendmsgFailCount()
{
}

void LogicMonitor::ReportCgiSyncCount()
{
}

void LogicMonitor::ReportCgiSyncSuccCount()
{
}

void LogicMonitor::ReportCgiSyncFailCount()
{
}

void LogicMonitor::ReportMsgClientCount()
{
}

void LogicMonitor::ReportMsgClientSuccCount()
{
}

void LogicMonitor::ReportMsgClientFailCount()
{
}

void LogicMonitor::ReportToolsSendMsgRunTime(int runtime_ms)
{

}

