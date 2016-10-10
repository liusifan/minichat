

#include "iOssAttr.h"
#include "logic_monitor_sk.h"

#define MMMINICHAT_ID 65032

static LogicMonitorSK g_monitor;

LogicMonitorSK::LogicMonitorSK()
{
    LogicMonitor::SetDefault(this);
}

LogicMonitorSK::~LogicMonitorSK()
{

}

void LogicMonitorSK::ReportAuthCount()
{
    OssAttrInc(MMMINICHAT_ID, 0, 1);
}

void LogicMonitorSK::ReportAuthSuccCount()
{
    OssAttrInc(MMMINICHAT_ID, 1, 1);
}

void LogicMonitorSK::ReportAuthFailCount()
{
    OssAttrInc(MMMINICHAT_ID, 2, 1);
}

void LogicMonitorSK::ReportMsgCount()
{
    OssAttrInc(MMMINICHAT_ID, 3, 1);
}

void LogicMonitorSK::ReportMsgSuccCount()
{
    OssAttrInc(MMMINICHAT_ID, 4, 1);
}

void LogicMonitorSK::ReportMsgFailCount()
{
    OssAttrInc(MMMINICHAT_ID, 5, 1);
}

void LogicMonitorSK::ReportCgiAuthCount()
{
    OssAttrInc(MMMINICHAT_ID, 6, 1);
}

void LogicMonitorSK::ReportCgiAuthSuccCount()
{
    OssAttrInc(MMMINICHAT_ID, 7, 1);
}

void LogicMonitorSK::ReportCgiAuthFailCount()
{
    OssAttrInc(MMMINICHAT_ID, 8, 1);
}

void LogicMonitorSK::ReportCgiSendmsgCount()
{
    OssAttrInc(MMMINICHAT_ID, 9, 1);
}

void LogicMonitorSK::ReportCgiSendmsgSuccCount()
{
    OssAttrInc(MMMINICHAT_ID, 10, 1);
}

void LogicMonitorSK::ReportCgiSendmsgFailCount()
{
    OssAttrInc(MMMINICHAT_ID, 11, 1);
}

void LogicMonitorSK::ReportCgiSyncCount()
{
    OssAttrInc(MMMINICHAT_ID, 12, 1);
}

void LogicMonitorSK::ReportCgiSyncSuccCount()
{
    OssAttrInc(MMMINICHAT_ID, 13, 1);
}

void LogicMonitorSK::ReportCgiSyncFailCount()
{
    OssAttrInc(MMMINICHAT_ID, 14, 1);
}

void LogicMonitorSK::ReportMsgClientCount()
{
    OssAttrInc(MMMINICHAT_ID, 15, 1);
}

void LogicMonitorSK::ReportMsgClientSuccCount()
{
    OssAttrInc(MMMINICHAT_ID, 16, 1);
}

void LogicMonitorSK::ReportMsgClientFailCount()
{
    OssAttrInc(MMMINICHAT_ID, 17, 1);
}



