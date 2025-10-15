/**
 * @file lv_port_fs.h
 * Header file for LVGL filesystem driver
 * Intended for use with Waveshare ESP32-S3-Touch-AMOLED-1.75 module
 *   with an external SD card mounted in the module
 *
 * Adapted from: https://github.com/lvgl/lvgl/blob/master/examples/porting/lv_port_fs_template.c
 */

/*Copy this file as "lv_port_fs.h" and set this value to "1" to enable content*/
#if 1

#ifndef LV_PORT_FS_H
#define LV_PORT_FS_H

// #ifdef __cplusplus
// extern "C" {
// #endif

/*********************
 *      INCLUDES
 *********************/
// #include "lvgl/lvgl.h"
#include <lvgl.h>

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/
void lv_port_fs_init(void);


/**********************
 *      MACROS
 **********************/

// #ifdef __cplusplus
// } /*extern "C"*/
// #endif

#endif /*LV_PORT_FS_H*/

#endif /*Disable/Enable content*/
