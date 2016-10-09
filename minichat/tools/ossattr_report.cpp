

#include "ossattr_report.h"
#include "iOssAttr.h"

#define MMMINICHAT_ID 65032

void ReportAuthCount()
{
    OssAttrInc(MMMINICHAT_ID, 0, 1);
}

void ReportAuthSuccCount()
{
    OssAttrInc(MMMINICHAT_ID, 1, 1);
}

void ReportAuthFailCount()
{
    OssAttrInc(MMMINICHAT_ID, 2, 1);
}

void ReportMsgCount()
{
    OssAttrInc(MMMINICHAT_ID, 3, 1);
}

void ReportMsgSuccCount()
{
    OssAttrInc(MMMINICHAT_ID, 4, 1);
}

void ReportMsgFailCount()
{
    OssAttrInc(MMMINICHAT_ID, 5, 1);
}










