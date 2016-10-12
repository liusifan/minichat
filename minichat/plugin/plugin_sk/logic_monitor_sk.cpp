

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

void LogicMonitorSK::ReportToolsSendMsgRunTime(int runtime_ms)
{
    int key = 0;

    if (runtime_ms < 5) {
        key = 0;
    } else if (runtime_ms < 10) {
        key = 1;
    } else if (runtime_ms < 15) {
        key = 2;
    } else if (runtime_ms < 20) {
        key = 3;
    } else if (runtime_ms < 25) {
        key = 4;
    } else if (runtime_ms < 30) {
        key = 5;
    } else if (runtime_ms < 35) {
        key = 6;
    } else if (runtime_ms < 40) {
        key = 7;
    } else if (runtime_ms < 50) {
        key = 8;
    } else if (runtime_ms < 60) {
        key = 9;
    } else if (runtime_ms < 70) {
        key = 10;
    } else if (runtime_ms < 100) {
        key = 11;
    } else if (runtime_ms < 200) {
        key = 12;
    } else if (runtime_ms < 300) {
        key = 13;
    } else if (runtime_ms < 400) {
        key = 14;
    } else {
        key = 15;
    }

    key += 18;
    OssAttrInc(MMMINICHAT_ID, key, 1);

    OssAttrInc(MMMINICHAT_ID, 40, runtime_ms);
}


