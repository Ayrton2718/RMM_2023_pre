#ifndef CAN_SMBUS_SRC_CORE_IO_CS_IO_H
#define CAN_SMBUS_SRC_CORE_IO_CS_IO_H

#include "cs_type.h"

#ifdef __cplusplus
extern "C" {
#endif

#define CSIO_DATAPACK_COUNT (4)

struct CSIo_dataPack_t
{
    CSType_gatewayId_t      gw_id;
    uint8_t                 data_count;
    struct CSType_csFrame_t data[CSIO_DATAPACK_COUNT];
};

void CSIo_initializer(void);

SABool_t CSIo_connectionWait(size_t gw_id);

SABool_t CSIo_read(struct CSIo_dataPack_t* data_pack);

void CSIo_send(const struct CSIo_dataPack_t* data_pack);

#ifdef __cplusplus
}
#endif

#endif /*CAN_SMBUS_SRC_CORE_IO_CS_IO_H*/