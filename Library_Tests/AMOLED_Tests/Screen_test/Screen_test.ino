#include <lvgl.h>
#include "pin_config.h"
#include <Arduino_GFX_Library.h>

// C arrays for images (gauge indices)
#include "ind9.c" 
#include "ind12_off.c"
#include "ind12_on.c"

// #include "HWCDC.h"
// HWCDC USBSerial;

#define LV_TIMER_PERIOD_MS 2

/******************************************************************************
*                            MODULE-LEVEL VARIABLES                           *
******************************************************************************/
Arduino_DataBus *bus = new Arduino_ESP32QSPI(
  LCD_CS, LCD_SCLK, LCD_SDIO0, LCD_SDIO1,
  LCD_SDIO2, LCD_SDIO3);

Arduino_GFX *gfx = new Arduino_CO5300(
  bus,
  LCD_RESET,
  0,
  false,
  LCD_WIDTH,
  LCD_HEIGHT,
  6, 0, 0, 0
);

static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf[LCD_WIDTH * 10];
static lv_obj_t *img_obj;
static unsigned long last_switch = 0;
static int current_img_index = 0; // 0 = off, 1 = on

/******************************************************************************
*                              HELPER FUNCTIONS                               *
******************************************************************************/

void example_lvgl_rounder_cb(struct _lv_disp_drv_t *disp_drv, lv_area_t *area)
{
    if(area->x1 % 2 !=0)area->x1--;
    if(area->y1 % 2 !=0)area->y1--;
    // 变为奇数(如果是偶数就加 1)
    if(area->x2 %2 ==0)area->x2++;
    if(area->y2 %2 ==0)area->y2++;
}

void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p) {
  uint32_t w = (area->x2 - area->x1 + 1);
  uint32_t h = (area->y2 - area->y1 + 1);

#if (LV_COLOR_16_SWAP != 0)
  gfx->draw16bitBeRGBBitmap(area->x1, area->y1, (uint16_t *)&color_p->full, w, h);
#else
  gfx->draw16bitRGBBitmap(area->x1, area->y1, (uint16_t *)&color_p->full, w, h);
#endif

  lv_disp_flush_ready(disp);
}

void example_increase_lvgl_tick(void *arg) {
  /* Tell LVGL how many milliseconds has elapsed */
  lv_tick_inc(LV_TIMER_PERIOD_MS);
}

/******************************************************************************
*                                INIT FUNCTION                                *
******************************************************************************/

void setup() {

  Serial.begin(115200);
  Serial.println("Starting...");
  // USBSerial.begin(115200);

  gfx->begin();
  // gfx->fillScreen(BLACK);
  gfx->Display_Brightness(255);

  lv_init();
  lv_disp_draw_buf_init(&draw_buf, buf, NULL, LCD_WIDTH * 10);

  static lv_disp_drv_t disp_drv;
  lv_disp_drv_init(&disp_drv);
  disp_drv.hor_res = LCD_WIDTH;
  disp_drv.ver_res = LCD_HEIGHT;
  disp_drv.flush_cb = my_disp_flush;
  disp_drv.rounder_cb = example_lvgl_rounder_cb;
  disp_drv.draw_buf = &draw_buf;
  lv_disp_drv_register(&disp_drv);

  // Set the main screen
  lv_obj_t *scr = lv_scr_act();
  lv_obj_set_style_bg_color(scr, lv_color_hsv_to_rgb(11, 100, 25), LV_PART_MAIN);
  // Create image for index 12
  img_obj = lv_img_create(scr);
  lv_img_set_src(img_obj, &img_ind12_on);
  lv_obj_set_pos(img_obj, 208, 10);


  // Set up timer for LVGL
  const esp_timer_create_args_t lvgl_tick_timer_args = {
    .callback = &example_increase_lvgl_tick,
    .name = "lvgl_tick"
  };

  esp_timer_handle_t lvgl_tick_timer = NULL;
  esp_timer_create(&lvgl_tick_timer_args, &lvgl_tick_timer);
  esp_timer_start_periodic(lvgl_tick_timer, LV_TIMER_PERIOD_MS * 1000);

}


/******************************************************************************
*                                 MAIN DRIVER                                 *
******************************************************************************/
void loop() {
  unsigned long now = millis();
  if (now - last_switch >= 5000) {
    Serial.println(current_img_index);
    // USBSerial.println(current_img_index);
    last_switch = now;
    current_img_index = 1 - current_img_index;
    if (current_img_index == 0) {
      lv_img_set_src(img_obj, &img_ind12_on);
    } else {
      lv_img_set_src(img_obj, &img_ind12_off);
    }
    // Force LVGL to redraw this object on the next lv_timer_handler pass
    // lv_obj_invalidate(img_obj);
    // lv_img_cache_invalidate_src(NULL);
  }
  
  lv_timer_handler();
  delay(5);
}
