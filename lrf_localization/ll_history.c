#include "ll_history.h"

typedef struct
{
    uint64_t time_stamp;
    LLType_pos_t d_move;
} LLHistory_dmove_t;


typedef struct 
{
    LLType_pos_t offset;
    SABuffer_t d_moves;

    LLTimer_t timer;
} LLHistory_info_t;


LLHistory_t LLHistory_create(LLTimer_t timer, LLType_pos_t init_pos)
{
    LLHistory_info_t* _obj = (LLHistory_info_t*)malloc(sizeof(LLHistory_info_t));
    _obj->offset.x = init_pos.x;
    _obj->offset.y = init_pos.y;
    _obj->offset.yaw = init_pos.yaw;

    SABuffer_create(&_obj->d_moves, sizeof(LLHistory_dmove_t));

    _obj->timer = timer;

    return (LLHistory_t)_obj;
}

void LLHistory_addDeltaMove(LLHistory_t obj, LLType_pos_t d_move)
{
    LLHistory_info_t* _obj = (LLHistory_info_t*)obj;
    LLHistory_dmove_t move;
    move.time_stamp = LLTimer_getNow(_obj->timer);
    move.d_move = d_move;
    SABuffer_add(&_obj->d_moves, &move);
}

void LLHistory_update(LLHistory_t obj, LLType_pos_t pos, uint64_t timer)
{
    LLHistory_info_t* _obj = (LLHistory_info_t*)obj;

    _obj->offset.x = pos.x;
    _obj->offset.y = pos.y;
    _obj->offset.yaw = pos.yaw;

    size_t his_count = SABuffer_count(&_obj->d_moves);

    SALOG_INFO("ll_history", "%d, %d", timer, his_count);

    size_t delete_count = 0;
    for(delete_count = 0; delete_count < his_count; delete_count++)
    {
        LLHistory_dmove_t move;
        SABuffer_readAtIndex(&_obj->d_moves, delete_count, &move);

        if(timer < move.time_stamp)
        {
            break;
        }
    }

    if(delete_count != 0)
    {
        SABuffer_remove_stream(&_obj->d_moves, 0, delete_count);
    }
}

LLType_pos_t LLHistory_calcPos(LLHistory_t obj)
{
    LLHistory_info_t* _obj = (LLHistory_info_t*)obj;

    LLType_pos_t culc_pos;
    culc_pos.x = _obj->offset.x;
    culc_pos.y = _obj->offset.y;
    culc_pos.yaw = _obj->offset.yaw;

    for(size_t i = 0; i < SABuffer_count(&_obj->d_moves); i++)
    {
        LLHistory_dmove_t move;
        SABuffer_readAtIndex(&_obj->d_moves, i, &move);
        culc_pos.x += move.d_move.x;
        culc_pos.y += move.d_move.y;
        culc_pos.yaw += move.d_move.yaw;
    }

    return culc_pos;
}

LLType_pos_t LLHistory_getDelta(LLHistory_t obj, uint64_t timer)
{
    LLHistory_info_t* _obj = (LLHistory_info_t*)obj;

    LLType_pos_t culc_pos;
    culc_pos.x = 0;
    culc_pos.y = 0;
    culc_pos.yaw = 0;

    for(size_t i = 0; i < SABuffer_count(&_obj->d_moves); i++)
    {
        LLHistory_dmove_t move;
        SABuffer_readAtIndex(&_obj->d_moves, i, &move);

        if(timer < move.time_stamp)
        {
            break;
        }

        culc_pos.x += move.d_move.x;
        culc_pos.y += move.d_move.y;
        culc_pos.yaw += move.d_move.yaw;
    }

    return culc_pos;
}