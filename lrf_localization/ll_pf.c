#include "ll_pf.h"

#include "ll_tomas.h"

#define LLPF_ERROR_GAIN (0.5)

typedef struct
{
    LLType_pos_t pos;
    float weight;
} LLPf_node_t;


typedef struct
{
    LLPf_node_t node[LLPF_NODE_COUNT];

    SARandom_t random;
    LLTomas_t tomas;
} LLPf_info_t;


static void LLPf_setNode(LLPf_t obj, const LLType_pos_t* center);
static void LLPf_culcWeight(LLPf_t obj, LLScanType_t* scan);
static void LLPf_resampling(LLPf_t obj);
static LLType_pos_t LLPf_estimate(LLPf_t obj);


LLPf_t LLPf_create(const LLType_pos_t* init_pos, LLTomas_t tomas)
{
    LLPf_info_t* _obj = SAMEM_MALLOC(sizeof(LLPf_info_t));
    for(size_t i = 0; i < LLPF_NODE_COUNT; i++)
    {
        _obj->node[i].pos.x = init_pos->x;
        _obj->node[i].pos.y = init_pos->y;
        _obj->node[i].pos.yaw = init_pos->yaw;
        _obj->node[i].weight = 1 / (float)LLPF_NODE_COUNT;
    }

    _obj->random = SARandom_create(time(NULL));
    _obj->tomas = tomas;

    return (LLPf_t)_obj;
}


LLType_pos_t LLPf_predict(LLPf_t obj, const LLType_pos_t* d_move, LLScanType_t* scan)
{
    LLPf_setNode(obj, d_move);
    LLPf_culcWeight(obj, scan);

    LLType_pos_t pos = LLPf_estimate(obj);

    LLPf_resampling(obj);

    return pos;
}


static void LLPf_setNode(LLPf_t obj, const LLType_pos_t* d_move)
{
    LLPf_info_t* _obj = (LLPf_info_t*)obj;
    
    float sum_move = d_move->x + d_move->y + d_move->yaw;
    float rate = (fabs(sum_move) * 15) + 0.1;

    // SALOG_INFO("sum_move", "%f", sum_move);

    for(size_t i = 0; i < LLPF_NODE_COUNT; i++)
    {   
        _obj->node[i].pos.x += d_move->x;
        _obj->node[i].pos.x += (SARandom_xorFloatNegaposi(&_obj->random) * 0.3 * rate);

        _obj->node[i].pos.y += d_move->y;
        _obj->node[i].pos.y += (SARandom_xorFloatNegaposi(&_obj->random) * 0.3 * rate);

        _obj->node[i].pos.yaw += d_move->yaw;
        _obj->node[i].pos.yaw += (SARandom_xorFloatNegaposi(&_obj->random) * 5 * rate * M_PI / 180);
    }
}

static void LLPf_culcWeight(LLPf_t obj, LLScanType_t* scan)
{
    LLPf_info_t* _obj = (LLPf_info_t*)obj;

    float weight_sum = 0;

    // LLScanType_sort(scan);
    // LLScanType_t b = LLTomas_virtualScan(_obj->tomas, 0, 0, (-45 / (float)180) * M_PI, 1, 0, M_PI);
    // // LLScanType_t a = LLTomas_simulateRange(_obj->tomas, 0, 0, (-45 / (float)180) * M_PI, scan);
    // LLScanType_show(&b);
    // LLScanType_show(scan);
    // // LLScanType_diffLaserScan(scan, &a);
    // // LLScanType_destructor(&a);
    // LLScanType_destructor(&b);

    uint64_t sum = 0;
    for(size_t i = 0; i < LLPF_NODE_COUNT; i++)
    {
        SATime_timer_t tim;
        SATime_timerStart(&tim);
        LLScanType_t sim_scan = LLTomas_simulateRange(_obj->tomas, _obj->node[i].pos.x, _obj->node[i].pos.y, _obj->node[i].pos.yaw, scan);
        sum += SATime_timerGetUs(&tim);
        
        size_t contain_count = 0;
        float diff_sum = 0;
        size_t angle_count = LLScanType_count(scan);
        for(size_t angle_i = 0; angle_i < angle_count; angle_i++)
        {
            float sim_distance = LLScanType_getDist(&sim_scan, angle_i);
            float scan_distance = LLScanType_getDist(scan, angle_i);

            // if(LLTomas_isContainBox(_obj->tomas, _obj->node[i].pos.x, _obj->node[i].pos.y, _obj->node[i].pos.yaw, LLScanType_getRad(scan, angle_i)))
            {
                if(sim_distance == 0 && scan_distance == 0)
                {
                }else if(sim_distance == 0){
                    diff_sum += LLPF_ERROR_GAIN / 2;//(LLPF_ERROR_GAIN / angle_count) * 20;
                }else if(scan_distance == 0){
                    diff_sum += LLPF_ERROR_GAIN;//(LLPF_ERROR_GAIN / angle_count) * 100;
                }else{
                    float diff = (sim_distance - scan_distance) * (sim_distance - scan_distance);
                    // if(0.5 < diff)
                    // {
                    //     diff_sum += LLPF_ERROR_GAIN;//(LLPF_ERROR_GAIN / angle_count) * 20;
                    // }else{
                    diff_sum += diff;
                    // }
                }
                contain_count++;
            }
            // else
            // {
            //     if(sim_distance == 0 && scan_distance == 0)
            //     {
            //     }else if(sim_distance == 0){
            //         diff_sum += LLPF_ERROR_GAIN * 50;//(LLPF_ERROR_GAIN / angle_count) * 20;
            //     }else if(scan_distance == 0){
            //         // diff_sum += LLPF_ERROR_GAIN;//(LLPF_ERROR_GAIN / angle_count) * 100;
            //     }else{
            //         // float diff = (sim_distance - scan_distance) * (sim_distance - scan_distance);
            //         // if(0.5 < diff)
            //         // {
            //         //     diff_sum += LLPF_ERROR_GAIN;//(LLPF_ERROR_GAIN / angle_count) * 20;
            //         // }else{
            //         //     diff_sum += diff;
            //         // }
            //     }
            // }

        }

        // if((float)contain_count / (float)angle_count < 0.5)
        // {
        //     diff_sum = LLPF_ERROR_GAIN;
        // }
        // diff_sum += 1 / 1 - ((float)contain_count / (float)angle_count);

        if(diff_sum == 0)
        {
            diff_sum += 0.001;
        }

        float weight = 1 / diff_sum;
        _obj->node[i].weight = weight;
        weight_sum += weight;

        LLScanType_destructor(&sim_scan);
    }

    SALOG_INFO("tmp", "%d", sum);

    for(size_t node_i = 0; node_i < LLPF_NODE_COUNT; node_i++)
    {
        _obj->node[node_i].weight /= weight_sum;
    }
}


// for resampling
static void LLPf_resampling(LLPf_t obj)
{
    LLPf_info_t* _obj = (LLPf_info_t*)obj;

    float recip_ess = 0;
    for(size_t i = 0; i < LLPF_NODE_COUNT; i++)
    {
        recip_ess += _obj->node[i].weight * _obj->node[i].weight;
    }

    float ess = 1 / recip_ess;

    // CCLOG_INFO("%f", ess);
    // SALOG_INFO("lrf_localization", "%f", LLPF_NODE_COUNT / ess);

    if(ess < (LLPF_NODE_COUNT / (float)1.5))
    {
        // LLType_pos_t befo_nodes[LLPF_NODE_COUNT];
        // float befo_weight[LLPF_NODE_COUNT];
        // memcpy((void*)befo_nodes, (void*)_obj->node, sizeof(LLType_pos_t) * LLPF_NODE_COUNT);
        // memcpy((void*)befo_weight, (void*)_obj->weight, sizeof(float) * LLPF_NODE_COUNT);
        LLPf_node_t befo_node[LLPF_NODE_COUNT];
        memcpy(befo_node, _obj->node, sizeof(LLPf_node_t) * LLPF_NODE_COUNT);

        const float space = (float)1 / LLPF_NODE_COUNT;
        float target_weight = space / 2;
        float w_sum = 0;
        size_t current_index = 0;
        // CCLOG_INFO("%f", space);

        float total_weight = 0;

        for(size_t target_index = 0; target_index < LLPF_NODE_COUNT; target_index++)
        {
            w_sum += befo_node[target_index].weight;

            for(; target_weight < w_sum;)
            {
                // SALOG_INFO("lrf_localization", "%d <- %d", current_index, target_index);
                _obj->node[current_index].pos = befo_node[target_index].pos;
                _obj->node[current_index].weight = befo_node[target_index].weight;
                total_weight += befo_node[target_index].weight;
                current_index++;
                target_weight += space;
            }
        }

        for(size_t node_i = 0; node_i < LLPF_NODE_COUNT; node_i++)
        {
            _obj->node[node_i].weight /= total_weight;
        }
    }
}


int LLPf_quickSortCmp_callback(const void * a, const void * b)
{
    const LLPf_node_t* node_a = a;
    const LLPf_node_t* node_b = b;
    if(node_a->weight < node_b->weight)
    {
        return -1;
    }else if(node_b->weight < node_a->weight){
        return 1;
    }else{
        return 0;
    }
}

static LLType_pos_t LLPf_estimate(LLPf_t obj)
{
    LLPf_info_t* _obj = (LLPf_info_t*)obj;

    qsort(_obj->node, LLPF_NODE_COUNT, sizeof(LLPf_node_t), LLPf_quickSortCmp_callback);

    LLType_pos_t local;
    local.x = 0;
    local.y = 0;
    local.yaw = 0;

    // printf("%f -> %f\n", _obj->node[0].weight, _obj->node[LLPF_NODE_COUNT - 1].weight);

    float total_weight = 0;
    for(size_t i = 0; i < 10; i++)
    {
        total_weight += _obj->node[LLPF_NODE_COUNT - 1 - i].weight;
    }

    for(size_t i = 0; i < 10; i++)
    {
        float weight = _obj->node[LLPF_NODE_COUNT - 1 - i].weight / total_weight;
        local.x += _obj->node[LLPF_NODE_COUNT - 1 - i].pos.x * weight;
        local.y += _obj->node[LLPF_NODE_COUNT - 1 - i].pos.y * weight;
        local.yaw += _obj->node[LLPF_NODE_COUNT - 1 - i].pos.yaw * weight;
        // SALOG_INFO("ll_pf", "(%f, %f, %f) <- %f", 
        //     _obj->node[LLPF_NODE_COUNT - 1 - i].pos.x, 
        //     _obj->node[LLPF_NODE_COUNT - 1 - i].pos.y, 
        //     _obj->node[LLPF_NODE_COUNT - 1 - i].pos.yaw,
        //     _obj->node[LLPF_NODE_COUNT - 1 - i].weight);
    }

    // for(size_t i = 0; i < LLPF_NODE_COUNT; i++)
    // {
    //     float weight = _obj->node[i].weight;
    //     local.x += _obj->node[i].pos.x * weight;
    //     local.y += _obj->node[i].pos.y * weight;
    //     local.yaw += _obj->node[i].pos.yaw * weight;
    // }

    return local;
}
