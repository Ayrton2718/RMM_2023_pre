#ifndef CC_JSON_SERIALIZER_H
#define CC_JSON_SERIALIZER_H

#include "cc_type.h"

#ifdef __cplusplus
extern "C" {
#endif

CC_obj CCJsonSerializer_dump(CC_obj obj, SABool_t visible);

SABool_t CCJsonSerializer_staticLoad(CC_obj obj, CC_obj bin_obj);
CC_obj CCJsonSerializer_dynamicLoad(CC_obj bin_obj);

#ifdef __cplusplus
}
#endif

#endif /*CC_JSON_SERIALIZER_H*/
