/**
 * @file lv_screen_driver.cpp
 * Implementation of LVGL screen driver
 * Intended for use with Waveshare ESP32-S3-Touch-AMOLED-1.75 module
 * 
 * Note: for simplicity of Arduino IDE compilation, this file is written to C formatting
 * while using a .cpp file extension
 *
 * Adapted from: https://github.com/lvgl/lvgl/blob/master/examples/porting/lv_port_fs_template.c
 */

/*Copy this file as "lv_screen_driver.cpp" and set this value to "1" to enable content*/
#if 1

/*********************
 *      INCLUDES
 *********************/
#include "lv_screen_driver.h"
// #include "../../lvgl.h"
#include "pin_config.h" // GPIO pins for the screen are defined here
#include <Arduino_GFX_Library.h>

/*********************
 *      DEFINES
 *********************/
#define LV_TIMER_PERIOD_MS 2

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void disp_flush(lv_disp_drv_t* disp, const lv_area_t* area, lv_color_t* color_p);
static void lvgl_rounder_cb(struct _lv_disp_drv_t *disp_drv, lv_area_t *area);
static void increase_lvgl_tick(void *arg);


/**********************
 *  STATIC VARIABLES
 **********************/
static Arduino_DataBus *_bus;
static Arduino_GFX *_gfx;
static lv_disp_draw_buf_t _draw_buf;
static lv_color_t _buf[LCD_WIDTH * 10];

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
void lv_screen_driver_init(void)
{

  // Allocate and initialize bus
  _bus = new Arduino_ESP32QSPI(
      LCD_CS, LCD_SCLK, LCD_SDIO0, LCD_SDIO1,
      LCD_SDIO2, LCD_SDIO3
  );

  // Allocate and initialize gfx
  _gfx = new Arduino_CO5300(
      _bus, 
      LCD_RESET, 
      0, 
      false, 
      LCD_WIDTH, LCD_HEIGHT, 
      6, 0, 0, 0
  );

  _gfx->begin();
  _gfx->Display_Brightness(255);
  
  lv_disp_draw_buf_init(&_draw_buf, _buf, NULL, LCD_WIDTH * 10);

  // Display driver configuration / fn callbacks definition
  static lv_disp_drv_t disp_drv;
  lv_disp_drv_init(&disp_drv);
  disp_drv.hor_res = LCD_WIDTH;
  disp_drv.ver_res = LCD_HEIGHT;
  disp_drv.flush_cb = disp_flush;
  disp_drv.rounder_cb = lvgl_rounder_cb;
  disp_drv.draw_buf = &_draw_buf;
  lv_disp_drv_register(&disp_drv);

  // Set up timer for LVGL
  const esp_timer_create_args_t lvgl_tick_timer_args = {
    .callback = &increase_lvgl_tick,
    .arg = nullptr,
    .dispatch_method = ESP_TIMER_TASK,
    .name = "lvgl_tick",
    .skip_unhandled_events = false
  };
  
  esp_timer_handle_t lvgl_tick_timer = NULL;
  esp_timer_create(&lvgl_tick_timer_args, &lvgl_tick_timer);
  esp_timer_start_periodic(lvgl_tick_timer, LV_TIMER_PERIOD_MS * 1000);
}

void lv_screen_set_brightness(uint8_t brightness)
{
  _gfx->Display_Brightness(brightness);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

// Flush the content of the internal buffer the specific area on the display.
static void disp_flush(lv_disp_drv_t* disp, const lv_area_t* area, lv_color_t* color_p) 
{
  uint32_t w = (area->x2 - area->x1 + 1);
  uint32_t h = (area->y2 - area->y1 + 1);
  _gfx->draw16bitRGBBitmap(area->x1, area->y1, (uint16_t*)&color_p->full, w, h);
  lv_disp_flush_ready(disp);
}

static void lvgl_rounder_cb(struct _lv_disp_drv_t *disp_drv, lv_area_t *area)
{
  if(area->x1 % 2 !=0)area->x1--;
  if(area->y1 % 2 !=0)area->y1--;
  
  if(area->x2 %2 ==0)area->x2++;
  if(area->y2 %2 ==0)area->y2++;
}

static void increase_lvgl_tick(void *arg) {
  // Tell LVGL how many milliseconds has elapsed 
  lv_tick_inc(LV_TIMER_PERIOD_MS);
}


#else /*Enable this file at the top*/

/*This dummy typedef exists purely to silence -Wpedantic.*/
typedef int keep_pedantic_happy;
#endif
