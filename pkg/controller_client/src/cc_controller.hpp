#ifndef CONTROLLER_CLIENT_CC_CONTROLLER_HPP
#define CONTROLLER_CLIENT_CC_CONTROLLER_HPP

#include "cc_type.h"

namespace controller
{


typedef enum
{
    EmergencyBtn_Ps = 0,
    EmergencyBtn_TouchPad = 1,
    EmergencyBtn_Mute = 2,
} EmergencyBtn_t;


class Controller
{
private:
    int                 m_channel;

    EmergencyBtn_t      m_eb;
    bool                m_safety_changed;
    bool                m_safety_flg;
    uint8_t             m_safety_count;
    

    uint8_t                 m_packet_flg;
    CCDualsense_sensor_t    m_packet[2];

    pthread_mutex_t     m_locker;

    static bool callback(void* arg, uint8_t* buff, size_t len);

public:
    Controller();
    Controller(int channel, EmergencyBtn_t eb);

    void Controller_cons(int channel, EmergencyBtn_t eb);

    void init(void);

    void connection_wait(int timeout_ms = 5000000);

    CCDualsense_sensor_t get_sensor(void);

    float L2Analog(void){return ((float)this->get_sensor().L2 / 255);}
    float R2Analog(void){return ((float)this->get_sensor().R2 / 255);}
    float rightX(void){return ((float)this->get_sensor().Right_X / 255);}
    float rightY(void){return ((float)this->get_sensor().Right_Y / 255);}
    float leftX(void){return ((float)this->get_sensor().Left_X / 255);}
    float leftY(void){return ((float)this->get_sensor().Left_Y / 255);}

    bool leftDown(void){return this->get_sensor().btn1.left_down;}
    bool leftRight(void){return this->get_sensor().btn1.left_right;}
    bool leftUp(void){return this->get_sensor().btn1.left_up;}
    bool leftLeft(void){return this->get_sensor().btn1.left_left;}

    bool rightDown(void){return this->get_sensor().btn1.right_down;}
    bool rightUp(void){return this->get_sensor().btn1.right_up;}
    bool rightRight(void){return this->get_sensor().btn1.right_right;}
    bool rightLeft(void){return this->get_sensor().btn1.right_left;}

    bool L1(void){return this->get_sensor().btn2.L1;}
    bool R1(void){return this->get_sensor().btn2.R1;}
    bool L2(void){return this->get_sensor().btn2.L2;}
    bool R2(void){return this->get_sensor().btn2.R2;}

    bool minus(void){return this->get_sensor().btn2.minus;}
    bool plus(void){return this->get_sensor().btn2.plus;}
    bool stickR(void){return this->get_sensor().btn2.stickR;}
    bool stickL(void){return this->get_sensor().btn2.stickR;}

    bool PS(void){return this->get_sensor().btn3.PS;}
    bool touch(void){return this->get_sensor().btn3.Touch;}
    bool mute(void){return this->get_sensor().btn3.Mute;}

    // For feedback
    void set_mute(bool is_enable);
    void set_led(uint8_t r, uint8_t g, uint8_t b);
    void set_player(int num); // max 7
    void set_vibeLarge(float rate);
    void set_vibeSmall(float rate);
};

}

#endif /*CONTROLLER_CLIENT_CC_CONTROLLER_HPP*/