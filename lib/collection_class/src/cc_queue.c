// #include "cc_queue.h"
// #include "cc_base_object.h"


// #define CCQUEUE_RING_BUFFER_COUNT (16)

// struct CCQueue_t
// {
//     CCBaseObject_t object;

//     SAQueue_t queue;
//     CC_obj queue_buffer[CCQUEUE_RING_BUFFER_COUNT];
// };


// static void CCQueue_add_nolock(CC_obj obj, CC_obj add_obj);
// static size_t CCQueue_count_nolock(CC_obj obj);
// static CC_obj CCQueue_peek_nolock(CC_obj obj);
// static SABool_t CCQueue_readble_nolock(CC_obj obj);
// static void CCQueue_remove_nolock(CC_obj obj);
// static void CCQueue_clear_nolock(CC_obj obj);

// void CCQueue_destructor_callback_nolock(CC_obj obj);
// void CCQueue_enableLocker_callback_nolock(CC_obj obj);
// CC_obj CCQueue_copy_callback_nolock(CC_obj obj);
// SABool_t CCQueue_replace_callback_nolock(CC_obj obj, CC_obj obj_from);
// size_t CCQueue_toHash_callback_nolock(CC_obj obj);
// void CCQueue_debug_callback_nolock(CC_obj obj, SABuffer_t* strings);
// void CCQueue_dump_callback_nolock(CC_obj obj, OPDumper_t dumper);
// SABool_t CCQueue_load_callback_nolock(CC_obj obj, OPLoader_t loader, OPType_token_t token);


// CC_obj CCQueue_create(void)
// {
//     struct CCQueue_t* _obj = (struct CCQueue_t*)CCBaseObject_create(
//         sizeof(struct CCQueue_t), 
//         CC_QUEUE_TYPE_ID, 
//         CCQueue_destructor_callback_nolock, 
//         CCQueue_enableLocker_callback_nolock,
//         CCQueue_copy_callback_nolock, 
//         CCQueue_replace_callback_nolock,
//         CCQueue_toHash_callback_nolock, 
//         CCQueue_debug_callback_nolock, 
//         CCQueue_dump_callback_nolock,
//         CCQueue_load_callback_nolock);

//     BasicQueue_init(&_obj->queue, sizeof(CC_obj), CCQUEUE_RING_BUFFER_COUNT, _obj->queue_buffer);

//     return _obj;
// }

// size_t CCQueue_count(CC_obj obj)
// {
//     size_t count = 0;

//     if(CCBaseObject_isTypeObject(obj, CC_QUEUE_TYPE_ID))
//     {
//         CCBaseObject_lock(obj);
//         count = CCQueue_count_nolock(obj);
//         CCBaseObject_unlock(obj);
//     }

//     return count;
// }

// void CCQueue_add(CC_obj obj, CC_obj add_obj)
// {
//     if(CCBaseObject_isTypeObject(obj, CC_QUEUE_TYPE_ID))
//     {
//         CCBaseObject_lock(obj);
//         CCQueue_add_nolock(obj, add_obj);
//         CCBaseObject_unlock(obj);
//     }
// }

// CC_obj CCQueue_peek(CC_obj obj)
// {
//     CC_obj peak_obj = NULL;
//     if(CCBaseObject_isTypeObject(obj, CC_QUEUE_TYPE_ID))
//     {
//         CCBaseObject_lock(obj);
//         peak_obj = CCQueue_peek_nolock(obj);
//         CCBaseObject_unlock(obj);
//     }
//     return peak_obj;
// }

// void CCQueue_remove(CC_obj obj)
// {
//     if(CCBaseObject_isTypeObject(obj, CC_QUEUE_TYPE_ID))
//     {
//         CCBaseObject_lock(obj);
//         CCQueue_remove_nolock(obj);
//         CCBaseObject_unlock(obj);
//     }
// }


// SABool_t CCQueue_readble(CC_obj obj)
// {
//     SABool_t readble = SABOOL_FALSE;
//     if(CCBaseObject_isTypeObject(obj, CC_QUEUE_TYPE_ID))
//     {
//         CCBaseObject_lock(obj);
//         readble = CCQueue_readble_nolock(obj);
//         CCBaseObject_unlock(obj);
//     }
//     return readble;
// }


// void CCQueue_clear(CC_obj obj)
// {
//     if(CCBaseObject_isTypeObject(obj, CC_QUEUE_TYPE_ID))
//     {
//         CCBaseObject_lock(obj);
//         CCQueue_clear_nolock(obj);
//         CCBaseObject_unlock(obj);
//     }
// }

// static void CCQueue_add_nolock(CC_obj obj, CC_obj add_obj)
// {
//     struct CCQueue_t* _obj = (struct CCQueue_t*)obj;

//     if(BasicQueue_isFill(&_obj->queue))
//     {
//         CCLOG_WARNNING_NOFMT("Queue overflowed!");
//         CCQueue_remove_nolock(obj);
//     }

//     CCBaseObject_retain(add_obj);
//     BasicQueue_add(&_obj->queue, &add_obj);
// }

// static size_t CCQueue_count_nolock(CC_obj obj)
// {
//     struct CCQueue_t* _obj = (struct CCQueue_t*)obj;

//     return BasicQueue_count(&_obj->queue);
// }

// static CC_obj CCQueue_peek_nolock(CC_obj obj)
// {
//     struct CCQueue_t* _obj = (struct CCQueue_t*)obj;

//     if(BasicQueue_count(&_obj->queue) == 0)
//     {
//         CCLOG_WARNNING_NOFMT("Queue reader catch up at writer!");
//         return NULL;
//     }

//     CC_obj tmp;
//     BasicQueue_readonly(&_obj->queue, &tmp, 0);
//     return tmp;
// }


// static SABool_t CCQueue_readble_nolock(CC_obj obj)
// {
//     struct CCQueue_t* _obj = (struct CCQueue_t*)obj;

//     if(BasicQueue_count(&_obj->queue) == 0)
//     {
//         return SABOOL_FALSE;
//     }else{
//         return SABOOL_TRUE;
//     }
// }


// static void CCQueue_remove_nolock(CC_obj obj)
// {
//     struct CCQueue_t* _obj = (struct CCQueue_t*)obj;

//     if(BasicQueue_count(&_obj->queue) == 0)
//     {
//         CCLOG_WARNNING_NOFMT("Queue reader catch up at writer!");
//         return;
//     }

//     CC_obj tmp;
//     BasicQueue_get(&_obj->queue, &tmp);
//     CCBaseObject_release(tmp);
// }

// static void CCQueue_clear_nolock(CC_obj obj)
// {
//     struct CCQueue_t* _obj = (struct CCQueue_t*)obj;
//     for(size_t i = 0; i < BasicQueue_count(&_obj->queue); i++)
//     {
//         CC_obj tmp;
//         BasicQueue_readonly(&_obj->queue, &tmp, i);
//         CCBaseObject_release(tmp);
//     }

//     BasicQueue_clear(&_obj->queue);
// }


// void CCQueue_destructor_callback_nolock(CC_obj obj)
// {
//     CCQueue_clear_nolock(obj);
// }

// void CCQueue_enableLocker_callback_nolock(CC_obj obj)
// {
//     struct CCQueue_t* _obj = (struct CCQueue_t*)obj;

//     for(size_t i = 0; i < BasicQueue_count(&_obj->queue); i++)
//     {
//         CC_obj tmp;
//         BasicQueue_readonly(&_obj->queue, &tmp, i);
//         CCBaseObject_enableLocker(tmp);
//     }
// }

// CC_obj CCQueue_copy_callback_nolock(CC_obj obj)
// {
//     struct CCQueue_t* _obj = (struct CCQueue_t*)obj;

//     struct CCQueue_t* copy_obj = (struct CCQueue_t*)CCQueue_create();
//     BasicQueue_init(&copy_obj->queue, BasicQueue_dataSize(&_obj->queue), BasicQueue_maxCount(&_obj->queue), copy_obj->queue_buffer);

//     for(size_t i = 0; i < BasicQueue_count(&_obj->queue); i++)
//     {
//         CC_obj tmp;
//         BasicQueue_readonly(&_obj->queue, &tmp, i);

//         CCQueue_add_nolock(copy_obj, tmp);
//     }

//     return copy_obj;
// }

// SABool_t CCQueue_replace_callback_nolock(CC_obj obj, CC_obj obj_from)
// {
//     struct CCQueue_t* _obj = (struct CCQueue_t*)obj;

//     struct CCQueue_t* _obj_from = (struct CCQueue_t*)obj_from;

//     if(BasicQueue_count(&_obj->queue) == BasicQueue_count(&_obj_from->queue))
//     {
//         for(size_t i = 0; i < BasicQueue_count(&_obj_from->queue); i++)
//         {
//             CC_obj _obj_current_tmp, _obj_from_current_tmp;
//             BasicQueue_readonly(&_obj->queue, &_obj_current_tmp, i);
//             BasicQueue_readonly(&_obj_from->queue, &_obj_from_current_tmp, i);

//             if(CCBaseObject_replace(_obj_current_tmp, _obj_from_current_tmp) == SABOOL_FALSE)
//             {
//                 return SABOOL_FALSE;
//             }
//         }
//         return SABOOL_TRUE;
//     }else{
//         return SABOOL_FALSE;
//     }

//     return SABOOL_TRUE;
// }

// size_t CCQueue_toHash_callback_nolock(CC_obj obj)
// {
//     struct CCQueue_t* _obj = (struct CCQueue_t*)obj;

//     size_t hash = 0;

//     for(size_t i = 0; i < BasicQueue_count(&_obj->queue); i++)
//     {
//         CC_obj tmp;
//         BasicQueue_readonly(&_obj->queue, &tmp, i);

//         hash += CCBaseObject_toHash(tmp) ^ i;
//     }

//     return hash;
// }

// void CCQueue_debug_callback_nolock(CC_obj obj, SABuffer_t* string)
// {
//     struct CCQueue_t* _obj = (struct CCQueue_t*)obj;
    
//     SABuffer_add(string, "[");

//     for(size_t i = 0; i < BasicQueue_count(&_obj->queue); i++)
//     {
//         CC_obj tmp;
//         BasicQueue_readonly(&_obj->queue, &tmp, i);

//         CCBaseObject_debug(tmp, string);

//         SABuffer_add(string, ",");        
//     }

//     SABuffer_add(string, "]");
// }


// void CCQueue_dump_callback_nolock(CC_obj obj, OPDumper_t dumper)
// {
//     struct CCQueue_t* _obj = (struct CCQueue_t*)obj;

// }

// SABool_t CCQueue_load_callback_nolock(CC_obj obj, OPLoader_t loader, OPType_token_t token)
// {

// }
// // void CCQueue_toJson_callback_nolock(CC_obj obj, SABuffer_t* string, SABool_t visible, size_t indent)
// // {
// //     struct CCQueue_t* _obj = (struct CCQueue_t*)obj;

// //     SABuffer_add(string, "[");

// //     for(size_t i = 0; i < BasicQueue_count(&_obj->queue); i++)
// //     {
// //         CC_obj tmp;
// //         BasicQueue_readonly(&_obj->queue, &tmp, i);

// //         // CCBaseObject_toJson(tmp, string, visible);
// //         CCBaseObject_toJson(tmp, string, SABOOL_FALSE, 0);

// //         SABuffer_add(string, ",");        
// //     }

// //     SABuffer_add(string, "]");
// // }

// // SABool_t CCQueue_loadJson_callback_nolock(CC_obj obj, struct CCJsonParser_t* parser, struct CCJsonParser_jsonToken_t* json_token)
// // {
// //     struct CCQueue_t* _obj = (struct CCQueue_t*)obj;

// //     SABool_t is_success =  SABOOL_TRUE;

// //     size_t json_indexSize = 0; 
// //     if(CCJsonParser_arrayCount(parser, json_token, &json_indexSize))
// //     {
// //         for(size_t i = 0; i < json_indexSize; i++)
// //         {
// //             struct CCJsonParser_jsonToken_t child_token;
// //             if(CCJsonParser_getArray(parser, json_token, i, &child_token))
// //             {
// //                 CC_obj tmp;
// //                 if(BasicQueue_readonly(&_obj->queue, &tmp, i) == 1)
// //                 {
// //                     if(CCBaseObject_loadJson(tmp, parser, &child_token) == SABOOL_FALSE)
// //                     {
// //                         is_success = SABOOL_FALSE;
// //                     }
// //                 }else{
// //                     is_success = SABOOL_FALSE;
// //                     break;
// //                 }
// //             }else{
// //                 is_success = SABOOL_FALSE;
// //                 break;
// //             }
// //         }
// //     }else{
// //         is_success = SABOOL_FALSE;
// //     }

// //     return is_success;
// // }
