#include <lvgl.h>
#include "pin_config.h"
#include <Arduino_GFX_Library.h>

// C arrays for images (gauge indices)
// #include "ind9.c" 
// #include "ind12_off.c"
// #include "ind12_on.c"
#include "Gauge_Icons.h"

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
static lv_obj_t *ref_circle;
// static lv_obj_t *img_ind8;
// static lv_obj_t *img_ind9;
// static lv_obj_t *img_ind10;
// static lv_obj_t *img_ind11;
// static lv_obj_t *img_ind12;
// static lv_obj_t *img_ind4;
// static lv_obj_t *img_ind3;
// static lv_obj_t *img_ind2;
// static lv_obj_t *img_ind1;
static unsigned long last_switch = 0;
static int current_img_index = 0; // 0 = off, 1 = on

// static lv_obj_t *gauge_img_ind[9]  = {
//   img_ind8,
//   img_ind9,
//   img_ind10,
//   img_ind11,
//   img_ind12,
//   img_ind1,
//   img_ind2,
//   img_ind3,
//   img_ind4,
// }

// array to refer to image objects
static lv_obj_t *gauge_img_ind[9];
static lv_img_dsc_t gauge_img_dsc_ind[9][2];

/******************************************************************************
*                              HELPER FUNCTIONS                               *
******************************************************************************/

// initialize array of gauge objects
void create_gauge_images(lv_obj_t *parent) {
  for (int i = 0; i < 9; i++) {
    gauge_img_ind[i] = lv_img_create(parent);
  }
  gauge_img_dsc_ind[0][0] = img_ind8_off;
  gauge_img_dsc_ind[0][1] = img_ind8_on;
  gauge_img_dsc_ind[1][0] = img_ind9_off;
  gauge_img_dsc_ind[1][1] = img_ind9_on;
  gauge_img_dsc_ind[2][0] = img_ind10_off;
  gauge_img_dsc_ind[2][1] = img_ind10_on;
  gauge_img_dsc_ind[3][0] = img_ind11_off;
  gauge_img_dsc_ind[3][1] = img_ind11_on;
  gauge_img_dsc_ind[4][0] = img_ind12_off;
  gauge_img_dsc_ind[4][1] = img_ind12_on;
  gauge_img_dsc_ind[5][0] = img_ind1_off;
  gauge_img_dsc_ind[5][1] = img_ind1_on;
  gauge_img_dsc_ind[6][0] = img_ind2_off;
  gauge_img_dsc_ind[6][1] = img_ind2_on;
  gauge_img_dsc_ind[7][0] = img_ind3_off;
  gauge_img_dsc_ind[7][1] = img_ind3_on;
  gauge_img_dsc_ind[8][0] = img_ind4_off;
  gauge_img_dsc_ind[8][1] = img_ind4_on;
}

void example_lvgl_rounder_cb(struct _lv_disp_drv_t *disp_drv, lv_area_t *area)
{
    if(area->x1 % 2 !=0)area->x1--;
    if(area->y1 % 2 !=0)area->y1--;
    
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

  create_gauge_images(scr);

  // Create image for index 12
  // ref_circle = lv_obj_create(scr);
  // lv_obj_center(ref_circle);
  // lv_obj_set_size(ref_circle, 233, 233); 	//Or in one function

  // index 8
  gauge_img_ind[0] = lv_img_create(scr);
  lv_img_set_src(gauge_img_ind[0], &gauge_img_dsc_ind[0][0]);
  lv_obj_set_pos(gauge_img_ind[0], 32, 290);

  // index 9
  gauge_img_ind[1] = lv_img_create(scr);
  lv_img_set_src(gauge_img_ind[1], &gauge_img_dsc_ind[1][0]);
  lv_obj_set_pos(gauge_img_ind[1], 8, 210);

  // index 10
  gauge_img_ind[2] = lv_img_create(scr);
  lv_img_set_src(gauge_img_ind[2], &gauge_img_dsc_ind[2][0]);
  lv_obj_set_pos(gauge_img_ind[2], 32, 106);
  
  // index 11
  gauge_img_ind[3] = lv_img_create(scr);
  lv_img_set_src(gauge_img_ind[3], &gauge_img_dsc_ind[3][0]);
  lv_obj_set_pos(gauge_img_ind[3], 103, 34);

  // index 12
  gauge_img_ind[4] = lv_img_create(scr);
  lv_img_set_src(gauge_img_ind[4], &gauge_img_dsc_ind[4][0]);
  lv_obj_set_pos(gauge_img_ind[4], 208, 10);
  
  // index 1
  gauge_img_ind[5] = lv_img_create(scr);
  lv_img_set_src(gauge_img_ind[5], &gauge_img_dsc_ind[5][0]);
  lv_obj_set_pos(gauge_img_ind[5], 287, 34);
  
  // index 2
  gauge_img_ind[6] = lv_img_create(scr);
  lv_img_set_src(gauge_img_ind[6], &gauge_img_dsc_ind[6][0]);
  lv_obj_set_pos(gauge_img_ind[6], 340, 106);

  // index 3
  gauge_img_ind[7] = lv_img_create(scr);
  lv_img_set_src(gauge_img_ind[7], &gauge_img_dsc_ind[7][0]);
  lv_obj_set_pos(gauge_img_ind[7], 361, 210);
  
  // index 4
  gauge_img_ind[8] = lv_img_create(scr);
  lv_img_set_src(gauge_img_ind[8], &gauge_img_dsc_ind[8][0]);
  lv_obj_set_pos(gauge_img_ind[8], 340, 290);

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
  if (now - last_switch >= 1000) {
    Serial.println(current_img_index);
    // USBSerial.println(current_img_index);
    last_switch = now;
    current_img_index = 1 - current_img_index;
    if (current_img_index == 0) {
      for (int i = 0; i < 9; i++) {
        lv_img_set_src(gauge_img_ind[i], &gauge_img_dsc_ind[i][1]);
      } 
      // lv_img_set_src(gauge_img_ind[8], &img_ind12_on);
      // lv_obj_set_style_img_recolor(img_ind12, lv_color_hsv_to_rgb(16, 100, 49), 0);
    } else {
      for (int i = 0; i < 9; i++) {
        lv_img_set_src(gauge_img_ind[i], &gauge_img_dsc_ind[i][0]);
      }
      // lv_img_set_src(gauge_img_ind[8], &img_ind12_off);
      // lv_obj_set_style_img_recolor(img_ind12, lv_color_hsv_to_rgb(0, 0, 0), 0);

    }
    // Force LVGL to redraw this object on the next lv_timer_handler pass
    // lv_obj_invalidate(img_ind12);
    // lv_img_cache_invalidate_src(NULL);
  }
  
  lv_timer_handler();
  delay(5);
}
