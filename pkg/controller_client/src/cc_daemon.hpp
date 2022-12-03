#ifndef CONTROLLER_CLIENT_CC_DAEMON_H
#define CONTROLLER_CLIENT_CC_DAEMON_H

#include "cc_type.h"

namespace controller
{

namespace daemon
{

typedef bool (*callback_t)(void* arg, uint8_t* buff, size_t size);


void init(void);

void append(int channel, callback_t callback, void* arg);

void set_mute(int channel, bool is_enable);
void set_led(int channel, uint8_t r, uint8_t g, uint8_t b);
void set_player(int channel, int num);
void set_vibeLarge(int channel, uint8_t value);
void set_vibeSmall(int channel, uint8_t value);

bool is_connected(int channel);

}

}

#endif /*CONTROLLER_CLIENT_CC_DAEMON_H*/