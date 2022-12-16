#include "ll_scan_type.h"

LLScanType_t LLScanType_create(LLTimer_t timer)
{
    LLScanType_t scan;
    scan.count = 0;
    SABuffer_create(&scan.scans, sizeof(LLScanType_scanInfo_t));
    scan.time_stamp = LLTimer_getNow(timer);

    return scan;
}

void LLScanType_destructor(LLScanType_t* scan)
{
    SABuffer_destructor(&scan->scans);
}

LLScanType_t LLScanType_clone(LLScanType_t* scan)
{
    LLScanType_t copy_scan;
    copy_scan.count = 0;
    SABuffer_clone(&scan->scans, &copy_scan.scans);
    copy_scan.time_stamp = scan->time_stamp;

    return copy_scan;
}


size_t LLScanType_count(LLScanType_t* scan)
{
    return scan->count;
}


void LLScanType_set(LLScanType_t* scan, float rad, float dist)
{
    scan->count++;
    LLScanType_scanInfo_t info;
    info.angle = rad;
    info.range = dist;
    SABuffer_add(&scan->scans, &info);
}


float LLScanType_getRad(LLScanType_t* scan, size_t i)
{
    float rad = 0;
    if(i < scan->count)
    {
        LLScanType_scanInfo_t info;
        SABuffer_readAtIndex(&scan->scans, i, &info);
        rad = info.angle;
    }else{
        SALOG_ERROR("lrf_localization", "out of index");
    }
    return rad;
}

float LLScanType_getDist(LLScanType_t* scan, size_t i)
{
    float dist = 0;
    if(i < scan->count)
    {
        LLScanType_scanInfo_t info;
        SABuffer_readAtIndex(&scan->scans, i, &info);
        dist = info.range;
    }else{
        SALOG_ERROR("lrf_localization", "out of index");
    }
    return dist;
}


void LLScanType_show(LLScanType_t* scan)
{
    SALOG_INFO("lrf_localization", "%d, %d", scan->time_stamp, scan->count);
    for(size_t i = 0; i < scan->count; i++)
    {
        SALOG_INFO("lrf_localization", "%f : %f", LLScanType_getRad(scan, i) * 180 / M_PI, LLScanType_getDist(scan, i));
    }
}

void LLScanType_diffLaserScan(LLScanType_t* scan1, LLScanType_t* scan2)
{
    size_t len1 = SABuffer_count(&scan1->scans);
    size_t len2 = SABuffer_count(&scan2->scans);

    if(len1 == len2)
    {
        for(size_t i = 0; i < len1; i++)
        {
            LLScanType_scanInfo_t info1;
            LLScanType_scanInfo_t info2;
            SABuffer_readAtIndex(&scan1->scans, i, &info1);

            SABuffer_readAtIndex(&scan2->scans, i, &info2);
            SALOG_INFO("lrf_localization", "%f\t: %f -> %f", info1.angle * 180 / M_PI, info1.range, info2.range);
        }
    }else{
        SALOG_ERROR("lrf_pf", "Length is different.");

    }
}


int LLScanType_sortCmp_callback(const void * a, const void * b)
{
    const LLScanType_scanInfo_t* scan_a = a;
    const LLScanType_scanInfo_t* scan_b = b;
    if(scan_a->angle < scan_b->angle)
    {
        return -1;
    }else if(scan_b->angle < scan_a->angle){
        return 1;
    }else{
        return 0;
    }
}

void LLScanType_sort(LLScanType_t* scan)
{
    SABuffer_sort(&scan->scans, LLScanType_sortCmp_callback);
}

