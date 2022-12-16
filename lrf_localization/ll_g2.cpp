#ifndef LL_LRF_SIM

#include "ll_g2.h"

#include <CYdLidar.h>
#include "ll_scan_type.h"
#include <std_api/std_api.h>
#include <process_comm/process_comm.h>

#define LLA1M8_BUFF_SIZE    (4)

typedef struct
{
    char comm_path[128];
    CYdLidar* drv;

    pthread_mutex_t     locker;
    SAQueue_t           queue;
    LLScanType_t        scan[LLA1M8_BUFF_SIZE];

    LLTimer_t           timer;
} LLG2_info_t;


static void* LLG2_receiver(LLG2_t obj);


LLG2_t LLG2_create(const char* comm_path, LLTimer_t timer)
{
    LLG2_info_t* _obj = (LLG2_info_t*)SAMEM_MALLOC(sizeof(LLG2_info_t));

    strcpy(_obj->comm_path, comm_path);

    //////////////////////string property/////////////////
    /// lidar port
    _obj->drv = new CYdLidar();
    _obj->drv->setlidaropt(LidarPropSerialPort, _obj->comm_path, strlen(_obj->comm_path));
    /// ignore array
    std::string ignore_ranges;
    ignore_ranges.clear();
    _obj->drv->setlidaropt(LidarPropIgnoreArray, ignore_ranges.c_str(), ignore_ranges.size());

    //////////////////////int property/////////////////
    /// lidar baudrate
    int baudrate = 230400;
    _obj->drv->setlidaropt(LidarPropSerialBaudrate, &baudrate, sizeof(int));
    /// tof lidar
    int optval = TYPE_TRIANGLE;
    _obj->drv->setlidaropt(LidarPropLidarType, &optval, sizeof(int));
    /// device type
    optval = YDLIDAR_TYPE_SERIAL;
    _obj->drv->setlidaropt(LidarPropDeviceType, &optval, sizeof(int));
    /// sample rate
    optval = 4;
    _obj->drv->setlidaropt(LidarPropSampleRate, &optval, sizeof(int));
    /// abnormal count
    optval = 4;
    _obj->drv->setlidaropt(LidarPropAbnormalCheckCount, &optval, sizeof(int));
    /// Intenstiy bit count
    optval = 8;
    _obj->drv->setlidaropt(LidarPropIntenstiyBit, &optval, sizeof(int));

    //////////////////////bool property/////////////////
    /// fixed angle resolution
    bool b_optvalue;
    b_optvalue = false;
    _obj->drv->setlidaropt(LidarPropFixedResolution, &b_optvalue, sizeof(bool));
    /// rotate 180
    b_optvalue = true;
    _obj->drv->setlidaropt(LidarPropReversion, &b_optvalue, sizeof(bool));
    /// Counterclockwise
    b_optvalue = false;
    _obj->drv->setlidaropt(LidarPropInverted, &b_optvalue, sizeof(bool));
    // auto reconnect
    b_optvalue = true;
    _obj->drv->setlidaropt(LidarPropAutoReconnect, &b_optvalue, sizeof(bool));
    /// one-way communication
    b_optvalue = false;
    _obj->drv->setlidaropt(LidarPropSingleChannel, &b_optvalue, sizeof(bool));
    /// intensity
    b_optvalue = false;
    _obj->drv->setlidaropt(LidarPropIntenstiy, &b_optvalue, sizeof(bool));
    /// Motor DTR
    b_optvalue = true;
    _obj->drv->setlidaropt(LidarPropSupportMotorDtrCtrl, &b_optvalue, sizeof(bool));
    /// HeartBeat
    b_optvalue = false;
    _obj->drv->setlidaropt(LidarPropSupportHeartBeat, &b_optvalue, sizeof(bool));

    //////////////////////float property/////////////////
    /// unit: °
    float f_optvalue = 90.0f;
    _obj->drv->setlidaropt(LidarPropMaxAngle, &f_optvalue, sizeof(float));
    f_optvalue = -90.0f;
    _obj->drv->setlidaropt(LidarPropMinAngle, &f_optvalue, sizeof(float));
    /// unit: m
    f_optvalue = 16.0f;
    _obj->drv->setlidaropt(LidarPropMaxRange, &f_optvalue, sizeof(float));
    f_optvalue = 0.28f;
    _obj->drv->setlidaropt(LidarPropMinRange, &f_optvalue, sizeof(float));
    /// unit: Hz
    float frequency = 8.0;
    _obj->drv->setlidaropt(LidarPropScanFrequency, &frequency, sizeof(float));

    _obj->drv->enableGlassNoise(false);
    _obj->drv->enableSunNoise(false);

    bool ret = _obj->drv->initialize();
    if(!ret)
    {
        SALOG_ERROR("lrf_localization", "Fail to initialize %s", _obj->drv->DescribeError());
        return NULL;
    }

    ret = _obj->drv->turnOn();
    if(!ret)
    {
        SALOG_ERROR("lrf_localization", "Fail to start %s", _obj->drv->DescribeError());
        return NULL;
    }

    SALOCKER_INIT(&_obj->locker, NULL);
    SAQueue_create(&_obj->queue, sizeof(LLScanType_t), LLA1M8_BUFF_SIZE, _obj->scan);

    TCLauncher_launchWithArgs("LPLidar_receiver", LLG2_receiver, _obj);

    _obj->timer = timer;

    return (LLG2_t)_obj;
}

void LLG2_stop(LLG2_t obj)
{
    LLG2_info_t* _obj = (LLG2_info_t*)obj;
    _obj->drv->turnOff();
    _obj->drv->disconnecting();
    delete _obj->drv;
}

LLScanType_t LLG2_getValue(LLG2_t obj)
{
    LLG2_info_t* _obj = (LLG2_info_t*)obj;
    LLScanType_t scan;
    SABool_t is_get;

    SALOCKER_LOCK(&_obj->locker);
    is_get = SAQueue_get(&_obj->queue, &scan);
    SALOCKER_UNLOCK(&_obj->locker);

    if(is_get == SABOOL_FALSE)
    {
        scan = LLScanType_create(_obj->timer);
    }

    return scan;
}


static void* LLG2_receiver(LLG2_t obj)
{
    LLG2_info_t* _obj = (LLG2_info_t*)obj;
    LaserScan scan;
    while(1)
    {
        if(_obj->drv->doProcessSimple(scan))
        {
            LLScanType_t ll_scan = LLScanType_create(_obj->timer);
            for (size_t i = 0; i < scan.points.size(); i++)
            {
                const LaserPoint& p = scan.points.at(i);

                if(p.intensity == 0 || p.range == 0 || 6 < p.range)
                {
                    continue;
                }
                float angle = p.angle;
                angle = (M_PI / 2)  - angle;
                if(angle < 0 || M_PI < angle)
                {
                    continue;
                }
                LLScanType_set(&ll_scan, angle, p.range);
            }
            
            SALOCKER_LOCK(&_obj->locker);
            SAQueue_add(&_obj->queue, &ll_scan);
            SALOG_INFO("lrf_localization", "queue count(%d)", SAQueue_count(&_obj->queue));
            SALOCKER_UNLOCK(&_obj->locker);
        }
        else
        {
            SALOG_ERROR("lrf_localization", "Failed to get Lidar Data");
        }

        SATHREAD_YIELD();
    }
}

#endif /*LL_LRF_SIM*/