#include "ll_local_lrf.hpp"

#include "ll_g2.h"
#include "ll_history.h"
#include "ll_pf.h"
#include "ll_scan_type.h"
#include "ll_timer.h"
#include "ll_tomas.h"


using namespace odom_driver;

namespace lrf_localization
{

LocalLrf::LocalLrf(void)
{
}

LocalLrf::LocalLrf(const LocalLrfParam_t* param)
{
    this->LocalLrf_cons(param);
}


void LocalLrf::LocalLrf_cons(const LocalLrfParam_t* param)
{
    this->m_timer = LLTimer_create();
#ifndef LL_LRF_SIM
    this->m_g2 = LLG2_create("/dev/ttyUSB0", this->m_timer);
#endif /*LL_LRF_SIM*/

    this->m_tomas = LLTomas_create(this->m_timer);
    LLTomas_drawBox(this->m_tomas, 0,      2, 0.175, 0.175);
    LLTomas_drawBox(this->m_tomas, 2.5,    2, 0.175, 0.175);
    LLTomas_drawBox(this->m_tomas, -2.5,   2, 0.175, 0.175);
    LLTomas_drawBox(this->m_tomas, 1.5,    4, 0.175, 0.175);
    LLTomas_drawBox(this->m_tomas, -1.5,   4, 0.175, 0.175);

    LLType_pos_t pos;
    pos.x = param->init_x;
    pos.y = param->init_y;
    pos.yaw = param->init_teta;
    this->m_pf = LLPf_create(&pos, this->m_tomas);
    this->m_history = LLHistory_create(this->m_timer, pos);

#ifndef LL_LRF_SIM
    this->m_befo_wheel.x = 0;
    this->m_befo_wheel.y = 0;
    this->m_befo_wheel.theta = 0;
    this->m_local.LocalWheel_cons(&param->wheel_param);
#else
    this->m_wm = WMGl_create("10.42.0.1", 3003, 12.3, 9.65);
    WMGl_layer_t bg = WMGl_createLayer(this->m_wm);

    WMGl_vector_t vertex1[] = {{-5.2, -1.3}, {-5.2, -2.5}, {-4, -2.5}, {-4, -1.3}, {-5.2, -1.3}};
    WMGl_addLine(bg, {0, 0, 255}, vertex1, 5);
    WMGl_vector_t vertex2[] = {{4, -1.3}, {4, -2.5}, {5.2, -2.5}, {5.2,  -1.3}, {4, -1.3}};
    WMGl_addLine(bg, {0, 0, 255}, vertex2, 5);
    WMGl_vector_t vertex3[] = {{-4, 9.5}, {4, 9.5}, {4, 0}, {-4, 0}, {-4, 9.5}};
    WMGl_addLine(bg, {0, 0, 255}, vertex3, 5);
    WMGl_vector_t vertex4[] = {{-4, 0}, {4, 0}, {4, -2.5}, {-4, -2.5}, {-4, 0}};
    WMGl_addLine(bg, {0, 0, 255}, vertex4, 5);

    WMGl_vector_t vertex5[] = {{0-0.175, 2-0.175}, {0-0.175, 2+0.175}, {0+0.175, 2+0.175}, {0+0.175, 2-0.175}, {0-0.175, 2-0.175}};
    WMGl_addLine(bg, {0, 255, 0}, vertex5, 5);
    WMGl_vector_t vertex6[] = {{2.5-0.175, 2-0.175}, {2.5-0.175, 2-0+0.175}, {2.5+0.175, 2+0.175}, {2.5+0.175, 2-0.175}, {2.5-0.175, 2-0.175}};
    WMGl_addLine(bg, {0, 255, 0}, vertex6, 5);
    WMGl_vector_t vertex7[] = {{-2.5-0.175, 2-0.175}, {-2.5-0.175, 2+0.175}, {-2.5+0.175, 2+0.175}, {-2.5+0.175, 2-0.175}, {-2.5-0.175, 2-0.175}};
    WMGl_addLine(bg, {0, 255, 0}, vertex7, 5);
    WMGl_vector_t vertex8[] = {{1.5-0.175, 4-0.175}, {1.5-0.175, 4+0.175}, {1.5+0.175, 4+0.175}, {1.5+0.175, 4-0.175}, {1.5-0.175, 4-0.175}};
    WMGl_addLine(bg, {0, 255, 0}, vertex8, 5);
    WMGl_vector_t vertex9[] = {{-1.5-0.175, 4-0.175}, {-1.5-0.175, 4+0.175}, {-1.5+0.175, 4+0.175}, {-1.5+0.175, 4-0.175}, {-1.5-0.175, 4-0.175}};
    WMGl_addLine(bg, {0, 255, 0}, vertex9, 5);
    WMGl_render(this->m_wm, 0, bg);
    // this->m_map = LLMap_create(10.7, 12.3, 5.35, 9.65);

    // LLMap_addRectangle(this->m_map, -4, 9.5, 4, 0, 0.02, LLMap_writeType_Support);
    // LLMap_addRectangle(this->m_map, -4, 0, 4, -2.5, 0.02, LLMap_writeType_Support);
    // LLMap_addRectangle(this->m_map, -5.2, -1.3, -4, -2.5, 0.02, LLMap_writeType_Support);
    // LLMap_addRectangle(this->m_map, 4, -1.3, 5.2, -2.5, 0.02, LLMap_writeType_Support);

    // LLMap_addBox(this->m_map, 0, 2, 0.175, 0.175, LLMap_writeType_Object);
    // LLMap_addBox(this->m_map, 2.5, 2, 0.175, 0.175, LLMap_writeType_Object);
    // LLMap_addBox(this->m_map, -2.5, 2, 0.175, 0.175, LLMap_writeType_Object);
    // LLMap_addBox(this->m_map, 1.5, 4, 0.175, 0.175, LLMap_writeType_Object);
    // LLMap_addBox(this->m_map, -1.5, 4, 0.175, 0.175, LLMap_writeType_Object);

    this->m_virtual_tomas = LLTomas_clone(this->m_tomas);
    // LLTomas_drawBox(this->m_virtual_tomas, 1.5, -1, 0.5, 0.5);

    // LLMap_addBox(this->m_map, 1.5, -1, 0.5, 0.5, LLMap_writeType_Object);
#endif /*LL_LRF_SIM*/

    this->m_daemon_running = false;
    SALOCKER_INIT(&this->m_locker, NULL);
}


void LocalLrf::init(void)
{
#ifndef LL_LRF_SIM
    this->m_local.init();
#endif /*LL_LRF_SIM*/
}


odom_driver::Vector_t LocalLrf::estimate(void)
{
    if(this->m_daemon_running == false)
    {
        this->run();
    }

    Vector_t now_pos;
    SALOCKER_LOCK(&this->m_locker);
    now_pos = this->m_now_estimate;
    SALOCKER_UNLOCK(&this->m_locker);

    return now_pos;
}


void LocalLrf::daemon_start(void)
{
#ifndef LL_LRF_SIM
    this->m_daemon_running = true;
    TCLauncher_launchWithArgs("od_local_wheel", LocalLrf::daemon_wheel, this);
    TCLauncher_launchWithArgs("od_local_lrf", LocalLrf::daemon, this);
#endif /*LL_LRF_SIM*/
}


void LocalLrf::run(void)
{
#ifndef LL_LRF_SIM
    scan = LLG2_getValue(this->m_g2);
#else
    static LLType_pos_t real_pos = {0, -0.5, 0};

    SALOCKER_LOCK(&this->m_locker);
    LLType_pos_t now_pos = LLHistory_calcPos(this->m_history);
    SALOCKER_UNLOCK(&this->m_locker);

    LLScanType_t scan = LLTomas_virtualScan(this->m_virtual_tomas, real_pos.x, real_pos.y, real_pos.yaw, 1, 0, M_PI);

    LLType_pos_t delta = {0, 0, 0};

    WMGl_layer_t layer = WMGl_createLayer(this->m_wm);
    size_t len = LLScanType_count(&scan);
    for(size_t i = 0; i < len; i++)
    {
        float angle = LLScanType_getRad(&scan, i);
        float dist = LLScanType_getDist(&scan, i);


        float tar_x = real_pos.x + dist * cosf(angle - real_pos.yaw);
        float tar_y = real_pos.y + dist * sinf(angle - real_pos.yaw);

        WMGl_vector_t vertex[2] = {{real_pos.x, real_pos.y}, {tar_x, tar_y}};
        WMGl_addLine(layer, {255, 0, 0}, vertex, 2);
    }
    WMGl_render(this->m_wm, 1, layer);

    CCAutoRelease_startScope();
    {
        CC_obj key_obj = WMGl_getKey(this->m_wm, 100);
        if(CCObject_isObject(key_obj) == SABOOL_TRUE)
        {
            CCAutoRelease_add(key_obj);
            if(CCString_compare(key_obj, "KeyW") == 0){
                delta.y += 0.05;
            }else if(CCString_compare(key_obj, "KeyS") == 0){
                delta.y -= 0.05;
            }else if(CCString_compare(key_obj, "KeyA") == 0){
                delta.x -= 0.05;
            }else if(CCString_compare(key_obj, "KeyD") == 0){
                delta.x += 0.05;
            }else if(CCString_compare(key_obj, "KeyE") == 0){
                delta.yaw += 5 * M_PI / 180;
            }else if(CCString_compare(key_obj, "KeyQ") == 0){
                delta.yaw -= 5 * M_PI / 180;
            }

            CC_obj bin = CCAutoRelease_add(CCJsonSerializer_dump(key_obj, SABOOL_FALSE));
            SALOG_INFO("key_obj", "%s", CCBinary_getRaw(bin));
        }
    }
    CCAutoRelease_doneScope();

    real_pos.x += delta.x;
    real_pos.y += delta.y;
    real_pos.yaw += delta.yaw;
    
    SALOCKER_LOCK(&this->m_locker);
    LLHistory_addDeltaMove(this->m_history, delta);
    SALOCKER_UNLOCK(&this->m_locker);
#endif /*LL_LRF_SIM*/

    if(0 < LLScanType_count(&scan))
    {
        SALOCKER_LOCK(&this->m_locker);
        LLType_pos_t now_pos = LLHistory_calcPos(this->m_history);
        LLType_pos_t d_move = LLHistory_getDelta(this->m_history, scan.time_stamp);
        SALOCKER_UNLOCK(&this->m_locker);

#ifndef LL_LRF_SIM
#else
        SARandom_t rand_gen = SARandom_create((uint32_t)time(NULL));
        delta.x += SARandom_xorFloatNegaposi(&rand_gen) * 0.1;
        delta.y += SARandom_xorFloatNegaposi(&rand_gen) * 0.1;
        delta.yaw += SARandom_xorFloatNegaposi(&rand_gen) * 0.1;
#endif /*LL_LRF_SIM*/

        uint64_t enc_tim = LLTimer_getNow(this->m_timer);
        

        LLType_pos_t est_pos = LLPf_predict(this->m_pf, &d_move, &scan);
        
        static float lp_x = 0;
        static float lp_y = 0;
        static float lp_yaw = 0;

        SALOG_INFO("lrf_localization", "(%f, %f, %f), (%f, %f, %f)", 
                    est_pos.x, est_pos.y, est_pos.yaw * 180 / M_PI,
                    est_pos.x - now_pos.x, est_pos.y - now_pos.y, (est_pos.yaw - now_pos.yaw) * 180 / M_PI);

        SALOCKER_LOCK(&this->m_locker);
        LLHistory_update(this->m_history, est_pos, scan.time_stamp);
        this->m_now_estimate.x = est_pos.x;
        this->m_now_estimate.y = est_pos.y;
        this->m_now_estimate.theta = est_pos.yaw;
        SALOCKER_UNLOCK(&this->m_locker);
    }

    LLScanType_destructor(&scan);
}


void* LocalLrf::daemon(void* arg)
{
    LocalLrf* me = (LocalLrf*)arg;

    while(1)
    {
        me->run();
        std_api::Delay::ms(5);
    }

    return NULL;
}


void LocalLrf::wheel_run(void)
{
    LLType_pos_t delta;
#ifndef LL_LRF_SIM
    Vector_t local = this->m_local.estimate();

    delta.x = local.x - this->m_befo_wheel.x;
    delta.y = local.y - this->m_befo_wheel.y;
    delta.yaw = local.theta - this->m_befo_wheel.theta;
#else
    delta.x = 0;
    delta.y = 0;
    delta.yaw = 0;
#endif /*LL_LRF_SIM*/

    SALOCKER_LOCK(&this->m_locker);
    LLHistory_addDeltaMove(this->m_history, delta);
    SALOCKER_UNLOCK(&this->m_locker);

#ifndef LL_LRF_SIM
    this->m_befo_wheel = local;
#endif /*LL_LRF_SIM*/
}

void* LocalLrf::daemon_wheel(void* arg)
{
    LocalLrf* me = (LocalLrf*)arg;

    while(1)
    {
        me->wheel_run();
        std_api::Delay::ms(20);
    }

    return NULL;
}

}