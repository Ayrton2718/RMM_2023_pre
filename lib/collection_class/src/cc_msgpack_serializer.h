#ifndef CC_MSGPACK_SERIALIZER_H
#define CC_MSGPACK_SERIALIZER_H

#include "cc_type.h"

#ifdef __cplusplus
extern "C" {
#endif

CC_obj CCMsgpackSerializer_dump(CC_obj obj);

SABool_t CCMsgpackSerializer_staticLoad(CC_obj obj, CC_obj bin_obj);
CC_obj CCMsgpackSerializer_dynamicLoad(CC_obj bin_obj);

#ifdef __cplusplus
}
#endif

#endif /*CC_MSGPACK_SERIALIZER_H*/
