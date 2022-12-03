#ifndef CONTROLLER_CLIENT_CC_TYPE_H
#define CONTROLLER_CLIENT_CC_TYPE_H

#include <std_api/std_api.h>

#ifdef __cplusplus
extern "C"{
#endif

#define CCTYPE_SEND_PORT   (60024)
#define CCTYPE_FEEDBACK_PORT   (60025)

typedef struct
{
	uint8_t left_down: 1;
	uint8_t left_right: 1;
	uint8_t left_up: 1;
	uint8_t left_left: 1;
	uint8_t right_down: 1;
	uint8_t right_right: 1;
	uint8_t right_up: 1;
	uint8_t right_left: 1;
}__attribute__((__packed__)) CCDualsense_Btn1_t;

typedef struct
{
	uint8_t L1: 1;
	uint8_t R1: 1;
	uint8_t L2: 1;
	uint8_t R2: 1;
	uint8_t minus: 1;
	uint8_t plus: 1;
	uint8_t stickR: 1;
	uint8_t stickL: 1;
}__attribute__((__packed__)) CCDualsense_Btn2_t;

typedef struct 
{
	uint8_t PS: 1;
	uint8_t Touch: 1;
	uint8_t Mute: 1;
	uint8_t none0: 1;
	uint8_t none1: 1;
	uint8_t none2: 1;
	uint8_t none3: 1;
}__attribute__((__packed__)) CCDualsense_Btn3_t;


typedef struct
{
	uint32_t sensor_timestamp;

    uint8_t L2;
    uint8_t R2;
	uint8_t Right_X;
    uint8_t Right_Y;
	uint8_t Left_X;
    uint8_t Left_Y;
    CCDualsense_Btn1_t btn1;
	CCDualsense_Btn2_t btn2;
	CCDualsense_Btn3_t btn3;
}__attribute__((__packed__)) CCDualsense_sensor_t;

typedef struct
{
    uint8_t channel;

    CCDualsense_sensor_t sensor;
}__attribute__((__packed__)) CCType_sensorPacket_t;



typedef struct
{
	uint8_t lightbar[3];
	uint8_t player_num;
	uint8_t mute_led;
	uint8_t vibe_large;
	uint8_t vibe_small;
}__attribute__((__packed__)) CCDualsense_feedback_t;

typedef struct
{
	CCDualsense_feedback_t feedback;
}__attribute__((__packed__)) CCType_feedbackPacket_t;

#ifdef __cplusplus
}
#endif

#endif /*CONTROLLER_CLIENT_CC_TYPE_H*/