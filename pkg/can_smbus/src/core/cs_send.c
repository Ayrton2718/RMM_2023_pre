#include "cs_send.h"

#include "io/cs_tx.h"
#include "cs_checker.h"

struct CSSend_info_t
{
    CSChecker_t checker;

    CSType_gatewayId_t gw_id;
    CSType_canId_t id;
    CSType_canReg_t reg;
    CSType_regChannel_t channel;
};

static CSSend_t CSSend_create(CSType_gatewayId_t gw_id, CSType_canId_t id, CSType_canReg_t reg, CSType_regChannel_t channel);


CSSend_t CSSend_createUser(CSType_gatewayId_t gw_id, CSType_canId_t id, CSType_canReg_t reg)
{
    return CSSend_create(gw_id, id, reg, CSType_regChannel_USER);
}

CSSend_t CSSend_createSystem(CSType_gatewayId_t gw_id, CSType_canId_t id, CSType_canReg_t reg)
{
    return CSSend_create(gw_id, id, reg, CSType_regChannel_SYSTEM);
}

static CSSend_t CSSend_create(CSType_gatewayId_t gw_id, CSType_canId_t id, CSType_canReg_t reg, CSType_regChannel_t channel)
{
    struct CSSend_info_t* _obj = (struct CSSend_info_t*)SAMEM_MALLOC(sizeof(struct CSSend_info_t));

    _obj->checker = CSCHECKER_ID;

    _obj->gw_id = gw_id;
    _obj->id = id;
    _obj->reg = reg;
    _obj->channel = channel;
    return _obj;
}


void CSSend_close(CSSend_t obj)
{
    if(obj != NULL)
    {
        struct CSSend_info_t* _obj = (struct CSSend_info_t*)obj;
        CSCHECKER_CHECK(obj, _obj->checker);
        SAMEM_FREE(obj);
    }
}


void CSSend_send(CSSend_t obj, const CSData_t* data)
{
    struct CSSend_info_t* _obj = (struct CSSend_info_t*)obj;
    CSCHECKER_CHECK(obj, _obj->checker);

    CSTx_sendReg(_obj->gw_id, _obj->id, _obj->reg, CSData_getSize(data), data->var.Buff, CSTx_priority_Normal, _obj->channel);
}