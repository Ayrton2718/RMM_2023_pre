#include "cs_system.h"

#include "io/cs_io.h"
#include "io/cs_tx.h"
#include "cs_send.h"

struct CSSystem_t
{
    pthread_t safety_thread;

    pthread_mutex_t locker;
    SABool_t is_enable_safety;
};

static struct CSSystem_t g_obj;


void* CSSystem_safetyThread(void* args);
static void CSSystem_safeMode(void);
static void CSSystem_unsafeMode(void);


void CSSystem_initializer(void)
{
    SALOCKER_INIT(&g_obj.locker, NULL);
    g_obj.is_enable_safety = SABOOL_FALSE; 

    SATHREAD_CREATE("CSSystem_safetyThread", &g_obj.safety_thread, NULL, CSSystem_safetyThread, NULL);
}


void CSSystem_connectionWait(size_t gw_id)
{
    CSIo_connectionWait(gw_id);
}

void CSSystem_safetyOn(void)
{
    SALOCKER_LOCK(&g_obj.locker);
    g_obj.is_enable_safety = SABOOL_TRUE;
    SALOCKER_UNLOCK(&g_obj.locker);

    CSSystem_unsafeMode();
}

void CSSystem_safetyOff(void)
{
    SALOCKER_LOCK(&g_obj.locker);
    g_obj.is_enable_safety = SABOOL_FALSE;
    SALOCKER_UNLOCK(&g_obj.locker);

    CSSystem_safeMode();
}

SABool_t CSSystem_isSafety(void)
{
    SABool_t is_safety;
    SALOCKER_LOCK(&g_obj.locker);
    is_safety = g_obj.is_enable_safety;
    SALOCKER_UNLOCK(&g_obj.locker);

    return is_safety;
}

void* CSSystem_safetyThread(void* args)
{
    while(1)
    {
        SABool_t is_safety;
        SALOCKER_LOCK(&g_obj.locker);
        is_safety = g_obj.is_enable_safety;
        SALOCKER_UNLOCK(&g_obj.locker);


        if(is_safety == SABOOL_FALSE)
        {
            CSSystem_safeMode();
        }else{
            CSSystem_unsafeMode();
        }
        SATime_delayMs(200);
    }
}

void CSSystem_allReboot(void)
{
    uint8_t reset_buff[4] = {'C', 'H', 'I', 'P'};
    CSTx_sendBrc(0, CSType_brcReg_ChipInit, 4, reset_buff, CSTx_priority_High);
    CSTx_sendBrc(1, CSType_brcReg_ChipInit, 4, reset_buff, CSTx_priority_High);
    CSTx_sendBrc(2, CSType_brcReg_ChipInit, 4, reset_buff, CSTx_priority_High);
    CSTx_sendBrc(3, CSType_brcReg_ChipInit, 4, reset_buff, CSTx_priority_High);

}



static void CSSystem_safeMode(void)
{
    uint8_t safe_buff[4] = {0x00, 0x00, 0x00, 0x00};
    CSTx_sendBrc(0, CSType_brcReg_Safety, 0, safe_buff, CSTx_priority_High);
    CSTx_sendBrc(1, CSType_brcReg_Safety, 0, safe_buff, CSTx_priority_High);
    // CSTx_sendBrc(2, CSType_brcReg_Safety, 0, safe_buff, CSTx_priority_High);
    // CSTx_sendBrc(3, CSType_brcReg_Safety, 0, safe_buff, CSTx_priority_High);

}

static void CSSystem_unsafeMode(void)
{
    uint8_t unsafe_buff[4] = {'U', 'N', 'S', 'A'};
    CSTx_sendBrc(0, CSType_brcReg_Unsafe, 4, unsafe_buff, CSTx_priority_High);
    CSTx_sendBrc(1, CSType_brcReg_Unsafe, 4, unsafe_buff, CSTx_priority_High);
    // CSTx_sendBrc(2, CSType_brcReg_Unsafe, 4, unsafe_buff, CSTx_priority_High);
    // CSTx_sendBrc(3, CSType_brcReg_Unsafe, 4, unsafe_buff, CSTx_priority_High);

}