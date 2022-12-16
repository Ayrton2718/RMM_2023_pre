// #ifdef LL_LRF_SIM

// #include "ll_map.h"

// // http://opencv.jp/opencv-2.1/cpp/basic_structures.html#mat
// #include <opencv2/opencv.hpp>

// // width : x,
// // hight : y

// struct LLMap_inst_t
// {
//     cv::Mat* map;

//     size_t width;
//     size_t height;

//     size_t offset_x;
//     size_t offset_y;
// };

// static inline int LLMap_offsetX(struct LLMap_inst_t* _obj, float x)
// {
//     return _obj->offset_x + x * (1 / LPMAP_PIXEL_METER);
// }

// static inline int LLMap_offsetY(struct LLMap_inst_t* _obj, float y)
// {
//     return _obj->offset_y - y * (1 / LPMAP_PIXEL_METER);
// }

// static inline cv::Scalar LLMap_getColor(LLMap_writeType_t write_type)
// {
//     switch(write_type)
//     {
//         case LLMap_writeType_Laser:
//             return cv::Scalar(0, 0, 255);
        
//         case LLMap_writeType_Object:
//             return cv::Scalar(0, 255, 0);
        
//         case LLMap_writeType_Support:
//             return cv::Scalar(255, 0, 0);

//         default:
//             return cv::Scalar(0, 0, 0);
//     }
// }


// LLMap_t LLMap_create(float x, float y, float offset_x, float offset_y)
// {
//     struct LLMap_inst_t* _obj = (struct LLMap_inst_t*)malloc(sizeof(struct LLMap_inst_t));

//     int __x = x * (1 / LPMAP_PIXEL_METER);
//     int __y = y * (1 / LPMAP_PIXEL_METER);
//     int __offset_x = offset_x * (1 / LPMAP_PIXEL_METER);
//     int __offset_y = offset_y * (1 / LPMAP_PIXEL_METER);

//     int margin_width = (int)roundf(__x * 0.01);
//     int margin_height = (int)roundf(__y * 0.01);

//     _obj->width = __x + margin_width * 2;
//     _obj->height = __y + margin_height * 2;

//     _obj->offset_x = __offset_x + margin_width;
//     _obj->offset_y = __offset_y + margin_height;

//     _obj->map = new cv::Mat(cv::Size(_obj->width, _obj->height), CV_8UC3);

//     cv::rectangle(
//         *_obj->map, 
//         cv::Point(margin_width, margin_height), 
//         cv::Point(_obj->width - margin_width, _obj->height - margin_height), 
//         LLMap_getColor(LLMap_writeType_Support), 0.01 * (1 / LPMAP_PIXEL_METER));
    
//     return _obj;
// }


// LLMap_t LLMap_copy(LLMap_t map)
// {
//     struct LLMap_inst_t* _obj = (struct LLMap_inst_t*)malloc(sizeof(struct LLMap_inst_t));
//     struct LLMap_inst_t* _map = (struct LLMap_inst_t*)map;

//     _obj->map = new cv::Mat();
//     *_obj->map = _map->map->clone();//new cv::Mat(*_map->map);

//     _obj->width = _map->width;
//     _obj->height = _map->height;

//     _obj->offset_x = _map->offset_x;
//     _obj->offset_y = _map->offset_y;

//     return _obj;
// }


// void LLMap_destructor(LLMap_t obj)
// {
//     struct LLMap_inst_t* _obj = (struct LLMap_inst_t*)obj;

//     delete _obj->map;
// }


// void LLMap_addRectangle(LLMap_t obj, float x0, float y0, float x1, float y1, float tick, LLMap_writeType_t write_type)
// {
//     struct LLMap_inst_t* _obj = (struct LLMap_inst_t*)obj;
//     cv::rectangle(
//         *_obj->map, 
//         cv::Point(LLMap_offsetX(_obj, x0), LLMap_offsetY(_obj, y0)), 
//         cv::Point(LLMap_offsetX(_obj, x1), LLMap_offsetY(_obj, y1)), 
//         LLMap_getColor(write_type), tick * (1 / LPMAP_PIXEL_METER));
// }


// void LLMap_addBox(LLMap_t obj, float x0, float y0, float half_width, float half_height, LLMap_writeType_t write_type)
// {
//     struct LLMap_inst_t* _obj = (struct LLMap_inst_t*)obj;
//     cv::rectangle(
//         *_obj->map,
//         cv::Point(LLMap_offsetX(_obj, x0 - half_width),
//         LLMap_offsetY(_obj, y0 + half_height)),
//         cv::Point(LLMap_offsetX(_obj, x0 + half_width),
//         LLMap_offsetY(_obj, y0 - half_height)),
//         LLMap_getColor(write_type),
//         -1);
// }


// void LLMap_addCircle(LLMap_t obj, float center_x, float center_y, float radius, LLMap_writeType_t write_type)
// {
//     struct LLMap_inst_t* _obj = (struct LLMap_inst_t*)obj;
//     cv::circle(*_obj->map, cv::Point(LLMap_offsetX(_obj, center_x), LLMap_offsetY(_obj, center_y)), radius, LLMap_getColor(write_type), -1);
// }


// void LLMap_addLine(LLMap_t obj, float start_x, float start_y, float end_x, float end_y, LLMap_writeType_t write_type)
// {
//     struct LLMap_inst_t* _obj = (struct LLMap_inst_t*)obj;
//     cv::line(
//         *_obj->map, 
//         cv::Point(LLMap_offsetX(_obj, start_x), LLMap_offsetY(_obj, start_y)), 
//         cv::Point(LLMap_offsetX(_obj, end_x), LLMap_offsetY(_obj, end_y)), 
//         LLMap_getColor(write_type), 1);
// }


// void LLMap_addDot(LLMap_t obj, float x, float y, LLMap_writeType_t write_type)
// {
//     struct LLMap_inst_t* _obj = (struct LLMap_inst_t*)obj;
//     cv::circle(*_obj->map, 
//         cv::Point(LLMap_offsetX(_obj, x), LLMap_offsetY(_obj, y)),
//         6,  LLMap_getColor(write_type), -1);
// }


// void LLMap_removeLines(LLMap_t obj)
// {
//     struct LLMap_inst_t* _obj = (struct LLMap_inst_t*)obj;

//     for(size_t x = 0; x < _obj->width; x++)
//     {
//         for(size_t y = 0; y < _obj->height; y++)
//         {
//             uint8_t* pixelPtr = (uint8_t*)_obj->map->data;
//             int map_col = _obj->map->cols;

//             pixelPtr[y * map_col * 3 + x * 3 + 2] = 0;
//         }
//     }
// }


// void LLMap_writeScan(LLMap_t obj, float x0, float y0, float theta, LLScanType_t scan, LLMap_writeType_t write_type)
// {
//     struct LLMap_inst_t* _obj = (struct LLMap_inst_t*)obj;

//     size_t len = LLScanType_count(&scan);

//     for(size_t i = 0; i < len; i++)
//     {
//         float angle = LLScanType_getRad(&scan, i);
//         float dist = LLScanType_getDist(&scan, i);


//         float tar_x = x0 + dist * cosf(angle - theta);
//         float tar_y = y0 + dist * sinf(angle - theta);

//         LLMap_addLine(obj, x0, y0, tar_x, tar_y, write_type);
//     }
// }


// size_t LLMap_width(LLMap_t obj)
// {
//     struct LLMap_inst_t* _obj = (struct LLMap_inst_t*)obj;
//     return _obj->width;
// }

// size_t LLMap_height(LLMap_t obj)
// {
//     struct LLMap_inst_t* _obj = (struct LLMap_inst_t*)obj;
//     return _obj->height;
// }


// void LLMap_show(LLMap_t obj, const char* tag, LLType_pos_t* pos)
// {
//     struct LLMap_inst_t* _obj = (struct LLMap_inst_t*)obj;

//     cv::namedWindow(tag, 0);
//     cv::resizeWindow(tag, cv::Size(768, (768 *_obj->width) / _obj->height));
//     cv::imshow(tag, *_obj->map);

//     LLType_pos_t tmp = {0, 0, 0};
//     if(pos == NULL)
//     {
//         pos = &tmp;
//     }

//     // SATime_timer_t tim;
//     // SATime_timerStart(&tim);
    
//     int key = cv::waitKey(100);
//     switch(key)
//     {
//     case 119:
//         SALOG_INFO("ll_map", "Up");
//         pos->y += 0.05;
//         break;
    
//     case 115:
//         SALOG_INFO("ll_map", "Down");
//         pos->y -= 0.05;
//         break;

//     case 100:
//         SALOG_INFO("ll_map", "Left");
//         pos->x += 0.05;
//         break;

//     case 97:
//         SALOG_INFO("ll_map", "Right");
//         pos->x -= 0.05;
//         break;

//     case 101:
//         SALOG_INFO("ll_map", "turnR");
//         pos->yaw += 5 * M_PI / 180;
//         break;

//     case 113:
//         SALOG_INFO("ll_map", "turnL");
//         pos->yaw -= 5 * M_PI / 180;
//         break;

//     case 27:
//         exit(0);
//         break;

//     default:
//         SALOG_INFO("ll_map", "%d", key);
//         break;
//     }

//     // uint64_t delta = SATime_timerGetMs(&tim);
//     // if(delta < 100)
//     // {
//     //     SATime_delayMs(100 - delta);
//     // }
// }

// void LLMap_showWait(LLMap_t obj, const char* tag)
// {
//     struct LLMap_inst_t* _obj = (struct LLMap_inst_t*)obj;

//     cv::namedWindow(tag, 0);
//     cv::resizeWindow(tag, cv::Size(768, (768 *_obj->width) / _obj->height));
//     cv::imshow(tag, *_obj->map);

//     while(1)
//     {
//         int key = cv::waitKey(0);
//         // CCLOG_INFO("%d", key);
//         switch(key)
//         {
//         case 119:
//             SALOG_INFO("ll_map", "Up");
//             break;
//         case 115:
//             SALOG_INFO("ll_map", "Down");
//             break;

//         case 97:
//             SALOG_INFO("ll_map", "Left");
//             break;

//         case 100:
//             SALOG_INFO("ll_map", "Right");
//             break;

//         case 27:
//             exit(0);
//             return;

//         default:
//             return;
//         }
//     }
// }

// #endif