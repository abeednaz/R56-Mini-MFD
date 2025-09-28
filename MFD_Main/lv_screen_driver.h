/**
 * @file lv_screen_driver.h
 * Header file for LVGL screen driver
 * Intended for use with Waveshare ESP32-S3-Touch-AMOLED-1.75 module
 *
 * Adapted from: https://github.com/lvgl/lvgl/blob/master/examples/porting/lv_port_fs_template.h
 */

/*Copy this file as "lv_screen_driver.h" and set this value to "1" to enable content*/
#if 1

#ifndef LV_SCREEN_DRIVER_H
#define LV_SCREEN_DRIVER_H

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
void lv_screen_driver_init(void);
void lv_screen_set_brightness(uint8_t brightness);

/**********************
 *      MACROS
 **********************/

// #ifdef __cplusplus
// } /*extern "C"*/
// #endif

#endif /*LV_SCREEN_DRIVER_H*/

#endif /*Disable/Enable content*/
