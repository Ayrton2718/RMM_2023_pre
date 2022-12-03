#ifndef CAN_SMBUS_SRC_ENCODER_HPP
#define CAN_SMBUS_SRC_ENCODER_HPP

#include "core/cs_manager.h"
#include "core/cs_send.h"
#include "core/cs_send_ack.h"
#include "core/cs_reg_share.h"
#include "core/cs_reg_signal.h"

#include "type.hpp"

namespace can_smbus
{

typedef enum
{
    EncoderMode_Count = 0,
    EncoderMode_Th,
    EncoderMode_Dth,
    EncoderMode_Ddth
} EncoderMode_t;

// PPR : Pulse per rotation.
typedef enum
{
    // : Pattern(1~4), 1 is on.
    // Recommended. Maximum RPM is Allowed 15000.
    EncoderPPR_48 = 48,         // : 1111
    EncoderPPR_96 = 96,         // : 1101
    EncoderPPR_100 = 100,       // : 0111
    EncoderPPR_125 = 125,       // : 1011
    EncoderPPR_200 = 200,       // : 0101
    EncoderPPR_250 = 250,       // : 1001
    EncoderPPR_256 = 256,       // : 0011
    EncoderPPR_512 = 512,       // : 0001

    // Not recommended. Maximum RPM is Allowed 7500.
    EncoderPPR_192 = 192,       // : 1110
    EncoderPPR_384 = 384,       // : 1100
    EncoderPPR_400 = 400,       // : 0110
    EncoderPPR_500 = 500,       // : 1010
    EncoderPPR_800 = 800,       // : 0100
    EncoderPPR_1000 = 1000,     // : 1000
    EncoderPPR_1024 = 1024,     // : 0010
    EncoderPPR_2048 = 2048,     // : 0000
} EncoderPPR_t;

struct encoder_param_t
{
    int id;
    EncoderMode_t mode;
    DirectRota_t direction;
    Interval_t interval;
    EncoderPPR_t ppr;
    Gateway_t gw_id;
};


class Encoder
{
private:
    CSManager_t m_manager;

    Interval_t m_interval;

    uint16_t m_ppr;

    EncoderMode_t m_mode;

    CSRegShare_t r_var;
    CSSendAck_t r_ppr;
    CSSend_t r_direction;

    int m_direction_num;

public:
    Encoder(void);
    Encoder(int id, EncoderMode_t mode, DirectRota_t direction = DirectRota_FORWARD, Interval_t interval = Interval_20Hz, EncoderPPR_t ppr = EncoderPPR_48, Gateway_t gw_id = Gateway_1);
    Encoder(struct encoder_param_t* param);

    void Encoder_cons(int id, EncoderMode_t mode, DirectRota_t direction = DirectRota_FORWARD, Interval_t interval = Interval_20Hz, EncoderPPR_t ppr = EncoderPPR_48, Gateway_t gw_id = Gateway_1);
    void Encoder_cons(struct encoder_param_t *param);

    void init(void);

    int32_t getAsInt(void);
    float getAsFloat(void);
};

}

#endif /*CAN_SMBUS_SRC_ENCODER_HPP*/