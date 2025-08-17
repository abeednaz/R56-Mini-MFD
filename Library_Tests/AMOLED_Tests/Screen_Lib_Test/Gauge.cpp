#include "Gauge.h"

// Gauge object constructor
// Parameters: none
// Returns: none
// Constructor for new Gauge object
Gauge::Gauge(): _bus(nullptr), _gfx(nullptr)
{
  lv_disp_draw_buf_init(&_draw_buf, _buf, NULL, LCD_WIDTH * 10);
}

// Gauge.begin()
// Parameters: pins
// Returns: none
// Begins new gauge
void Gauge::begin(int xSize, int ySize) {
  // Allocate and initialize bus
  _bus = new Arduino_ESP32QSPI(
      LCD_CS, LCD_SCLK, LCD_SDIO0, LCD_SDIO1,
      LCD_SDIO2, LCD_SDIO3
  );

  // Allocate and initialize gfx
  _gfx = new Arduino_CO5300(
      _bus, LCD_RESET, 0, false, LCD_WIDTH, LCD_HEIGHT, 6, 0, 0, 0
  );


  _scr = lv_scr_act(); // declare main screen

  lv_obj_set_style_bg_color(_scr, lv_color_hsv_to_rgb(11, 100, 25), LV_PART_MAIN);

  _gfx->begin();
  // gfx->fillScreen(BLACK);
  _gfx->Display_Brightness(255);

  lv_init();
  lv_disp_draw_buf_init(&_draw_buf, _buf, NULL, LCD_WIDTH * 10);

  lv_disp_drv_t disp_drv;
  lv_disp_drv_init(&disp_drv);
  disp_drv.hor_res = LCD_WIDTH;
  disp_drv.ver_res = LCD_HEIGHT;
  disp_drv.flush_cb = my_disp_flush;
  disp_drv.rounder_cb = example_lvgl_rounder_cb;
  disp_drv.draw_buf = &_draw_buf;

  disp_drv.user_data = this; // pass "this" pointer for callbacks to find vars from this fn

  lv_disp_drv_register(&disp_drv);

    // Set up timer for LVGL
  const esp_timer_create_args_t lvgl_tick_timer_args = {
    .callback = &example_increase_lvgl_tick,
    .name = "lvgl_tick"
  };
  
  esp_timer_handle_t lvgl_tick_timer = NULL;
  esp_timer_create(&lvgl_tick_timer_args, &lvgl_tick_timer);
  esp_timer_start_periodic(lvgl_tick_timer, LV_TIMER_PERIOD_MS * 1000);
}

void Gauge::my_disp_flush(lv_disp_drv_t* disp, const lv_area_t* area, lv_color_t* color_p) {
    Gauge* self = static_cast<Gauge*>(disp->user_data); // inherit _gfx from Gauge object

    uint32_t w = (area->x2 - area->x1 + 1);
    uint32_t h = (area->y2 - area->y1 + 1);

#if (LV_COLOR_16_SWAP != 0)
    self->_gfx->draw16bitBeRGBBitmap(area->x1, area->y1, (uint16_t*)&color_p->full, w, h);
#else
    self->_gfx->draw16bitRGBBitmap(area->x1, area->y1, (uint16_t*)&color_p->full, w, h);
#endif

    lv_disp_flush_ready(disp);
}

void Gauge::example_lvgl_rounder_cb(struct _lv_disp_drv_t *disp_drv, lv_area_t *area)
{
    if(area->x1 % 2 !=0)area->x1--;
    if(area->y1 % 2 !=0)area->y1--;
    
    if(area->x2 %2 ==0)area->x2++;
    if(area->y2 %2 ==0)area->y2++;
}

void Gauge::example_increase_lvgl_tick(void *arg) {
  // Tell LVGL how many milliseconds has elapsed 
  lv_tick_inc(LV_TIMER_PERIOD_MS);
}

// initialize array of gauge objects
void Gauge::createGaugeImages(lv_obj_t *parent) {
  for (int i = 0; i < 9; i++) {
    _gauge_img_ind[i] = lv_img_create(parent);
  }
  _gauge_img_dsc_ind[0][0] = img_ind8_off;
  _gauge_img_dsc_ind[0][1] = img_ind8_on;
  _gauge_img_dsc_ind[1][0] = img_ind9_off;
  _gauge_img_dsc_ind[1][1] = img_ind9_on;
  _gauge_img_dsc_ind[2][0] = img_ind10_off;
  _gauge_img_dsc_ind[2][1] = img_ind10_on;
  _gauge_img_dsc_ind[3][0] = img_ind11_off;
  _gauge_img_dsc_ind[3][1] = img_ind11_on;
  _gauge_img_dsc_ind[4][0] = img_ind12_off;
  _gauge_img_dsc_ind[4][1] = img_ind12_on;
  _gauge_img_dsc_ind[5][0] = img_ind1_off;
  _gauge_img_dsc_ind[5][1] = img_ind1_on;
  _gauge_img_dsc_ind[6][0] = img_ind2_off;
  _gauge_img_dsc_ind[6][1] = img_ind2_on;
  _gauge_img_dsc_ind[7][0] = img_ind3_off;
  _gauge_img_dsc_ind[7][1] = img_ind3_on;
  _gauge_img_dsc_ind[8][0] = img_ind4_off;
  _gauge_img_dsc_ind[8][1] = img_ind4_on;
}

int Gauge::updateGauge(GaugeData data)
{
  lv_timer_handler();
  return 0;
}

