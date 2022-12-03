#ifndef ROBOMAS_SRC_C_RM_M3508_H
#define ROBOMAS_SRC_C_RM_M3508_H

#include <std_api/std_api.h>

#ifdef __cplusplus
extern "C" {
#endif

void RMM3508_init(size_t gw_count);

void RMM3508_safetyOn(void);
void RMM3508_safetyOff(void);

void RMM3508_setCoef(size_t gw_id, int number, float p, float i, float d);
void RMM3508_setAdvancedCoef(size_t gw_id, int number, float p, float i, float d, SABool_t inverse);

void RMM3508_setRpmAsM3508(size_t gw_id, int number, int16_t rpm);
void RMM3508_setCurrentAsM3508(size_t gw_id, int number, float current);
void RMM3508_setRpmSyncAsM3508(size_t gw_id, int number1, int number2, int16_t rpm);
void RMM3508_setPitchAsM3508(size_t gw_id, int number, float base_cur, float target_rote);

void RMM3508_setRpmAsM2006(size_t gw_id, int number, int16_t rpm);
void RMM3508_setCurrentAsM2006(size_t gw_id, int number, float current);

int16_t RMM3508_getRpm(size_t gw_id, int number);
float RMM3508_getRealCurrentAsM3508(size_t gw_id, int number);
float RMM3508_getOrderCurrentAsM3508(size_t gw_id, int number);
float RMM3508_getRealCurrentAsM2006(size_t gw_id, int number);
float RMM3508_getOrderCurrentAsM2006(size_t gw_id, int number);
float RMM3508_getTheta(size_t gw_id, int number);
SABool_t RMM3508_isWakeup(size_t gw_id, int number);

#ifdef __cplusplus
}
#endif

#endif /*ROBOMAS_SRC_C_RM_M3508_H*/