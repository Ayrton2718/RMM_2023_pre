#ifndef CAN_SMBUS_SRC_CORE_IO_CS_TYPE_H
#define CAN_SMBUS_SRC_CORE_IO_CS_TYPE_H

#include <stdint.h>
#include <stdio.h>
#include <sys/time.h>
#include <std_api/std_api.h>

#ifdef __cplusplus
extern "C" {
#endif

// #define CS_DEBUG_ON

#define CSTYPE_BROADCAST_ADDR	(0x001)

#define CSTYPE_SAFETY_TIMEOUT 	(500) // 500ms


typedef uint16_t CSType_canId_t;
typedef uint8_t CSType_canReg_t;
typedef uint8_t CSType_gatewayId_t; 


struct CSType_csFrame_t
{
    CSType_canId_t id;
    CSType_canReg_t reg;
    uint8_t len;
    uint8_t data[4];
};

typedef enum
{
    CSType_regChannel_SYSTEM = 0,
    CSType_regChannel_USER,
} CSType_regChannel_t;


#define CSTYPE_GET_NUMBER_ID(my_id) (my_id & 0x3FF)

#define CSTYPE_IS_HOST_ID(my_id) ((my_id & 0x400) == 0)
#define CSTYPE_IS_DRIV_ID(my_id) ((my_id & 0x400) == 0x400)

#define CSTYPE_GET_HOST_ID(my_id) (my_id & 0x3FF)
#define CSTYPE_GET_DRIV_ID(my_id) ((my_id & 0x3FF) | 0x400)


typedef enum
{
	CSType_brcReg_Safety = 0x00,
	CSType_brcReg_Unsafe = 0x01,
	CSType_brcReg_Arp = 0x10,
	CSType_brcReg_ChipInit = 0x20
} CSType_brcReg_t;


typedef enum
{
	CSType_driverType_Encoder = 0x02,
	CSType_driverType_AbsEncoder = 0x03,
	CSType_driverType_Gyro = 0x04,
	CSType_driverType_Switch = 0x05,
	CSType_driverType_LPMotor = 0x11,
	CSType_driverType_HPMotor = 0x12,
	CSType_driverType_Servo = 0x13
} CSType_driverType_t;


typedef enum
{
    CSType_requestMode_Request = 0,
    CSType_requestMode_Interval
} CSType_requestMode_t;

typedef enum
{
	CSType_sysDrivReg_StartOrder = 0x00,
	CSType_sysDrivReg_Mmaster0 = 0x08,
	CSType_sysDrivReg_Mmaster1 = 0x09,
	CSType_sysDrivReg_Mmaster2 = 0x0A,
	CSType_sysDrivReg_Mmaster3 = 0x0B
} CSType_sysDrivReg_t;


#define CSTYPE_GET_NUMBER_REG(reg) 		(reg & 0x0F)

#define CSTYPE_IS_SYS_REG(reg) 		((reg & 0x10) == 0x00)
#define CSTYPE_IS_USER_REG(reg) 	((reg & 0x10) == 0x10)

#define CSTYPE_IS_WRITE_REG(reg) 		((reg & 0xE0) == 0x00)
#define CSTYPE_IS_REQUEST_REG(reg) 		((reg & 0xE0) == 0x20)
#define CSTYPE_IS_WRITEACK_REG(reg) 	((reg & 0xE0) == 0x40)
#define CSTYPE_IS_REQUESTACK_REG(reg) 	((reg & 0xE0) == 0x60)
#define CSTYPE_IS_ACK_REG(reg) 			((reg & 0xF0) == 0xE0) // for Ack
#define CSTYPE_IS_BRC_REG(reg) 			((reg & 0xF0) == 0xF0) // for broadcast


#define CSTYPE_GET_SYS_REG(reg) 	(reg & 0xEF)
#define CSTYPE_GET_USER_REG(reg) 	((reg & 0xEF) | 0x10)

#define CSTYPE_GET_WRITE_REG(reg) 		(reg & 0x1F)
#define CSTYPE_GET_REQUEST_REG(reg) 	((reg & 0x1F) | 0x20)
#define CSTYPE_GET_WRITEACK_REG(reg) 	((reg & 0x1F) | 0x40)
#define CSTYPE_GET_REQUESTACK_REG(reg) 	((reg & 0x1F) | 0x60)
#define CSTYPE_GET_ACK_REG(reg) 		((reg & 0x0F) | 0xE0)
#define CSTYPE_GET_BRC_REG(reg) 		((reg & 0x0F) | 0xF0)



CSType_canId_t CSType_createId(CSType_driverType_t type, CSType_canId_t dial_id);
const char* CSType_driverConvertName(CSType_driverType_t driver_id);

#ifdef __cplusplus
}
#endif

#endif /*CAN_SMBUS_SRC_CORE_IO_CS_TYPE_H*/