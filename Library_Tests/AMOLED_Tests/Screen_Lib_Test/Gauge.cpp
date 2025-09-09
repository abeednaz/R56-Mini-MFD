#include "Gauge.h"


#define INDEX_ON 1
#define INDEX_OFF 0
#define INDEX_CLEAR 2


// define origins of each index image
const uint16_t GAUGE_IND_POSITIONS[9][2] = 
{
  { 32, 290}, // top-left position of index 8
  {  8, 210}, // top-left position of index 9
  { 32, 106}, // top-left position of index 10
  {103, 34 }, // top-left position of index 11
  {208, 10 }, // top-left position of index 12
  {287, 34 }, // top-left position of index 1
  {340, 106}, // top-left position of index 2
  {361, 210}, // top-left position of index 3
  {340, 290}, // top-left position of index 4
};


// Gauge object constructor
// Parameters: none
// Returns: none
// Constructor for new Gauge object
Gauge::Gauge() //: _bus(nullptr), _gfx(nullptr)
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
}

// Gauge.begin()
// Parameters: pins
// Returns: none
// Begins new gauge
void Gauge::begin() {
  
  _gfx->begin();
  // gfx->fillScreen(BLACK);
  _gfx->Display_Brightness(255);

  lv_init();
  lv_disp_draw_buf_init(&_draw_buf, _buf, NULL, LCD_WIDTH * 10);

  static lv_disp_drv_t disp_drv;
  lv_disp_drv_init(&disp_drv);
  disp_drv.hor_res = LCD_WIDTH;
  disp_drv.ver_res = LCD_HEIGHT;
  disp_drv.flush_cb = disp_flush;
  disp_drv.rounder_cb = lvgl_rounder_cb;
  disp_drv.draw_buf = &_draw_buf;
  lv_disp_drv_register(&disp_drv);

  disp_drv.user_data = this; // pass "this" pointer for callbacks to find vars from this fn

  _scr = lv_scr_act(); // declare main screen
  lv_obj_set_style_bg_color(_scr, lv_color_hsv_to_rgb(11, 100, 25), LV_PART_MAIN);
  createGaugeImages(_scr);
  assignGaugeImages(_scr);

    // Set up timer for LVGL
  const esp_timer_create_args_t lvgl_tick_timer_args = {
    .callback = &Gauge::increase_lvgl_tick,
    .name = "lvgl_tick"
  };
  
  esp_timer_handle_t lvgl_tick_timer = NULL;
  esp_timer_create(&lvgl_tick_timer_args, &lvgl_tick_timer);
  esp_timer_start_periodic(lvgl_tick_timer, LV_TIMER_PERIOD_MS * 1000);
}

void Gauge::disp_flush(lv_disp_drv_t* disp, const lv_area_t* area, lv_color_t* color_p) {
    Gauge* self = static_cast<Gauge*>(disp->user_data); // inherit _gfx from Gauge object

    uint32_t w = (area->x2 - area->x1 + 1);
    uint32_t h = (area->y2 - area->y1 + 1);
    self->_gfx->draw16bitRGBBitmap(area->x1, area->y1, (uint16_t*)&color_p->full, w, h);
    lv_disp_flush_ready(disp);
}

void Gauge::lvgl_rounder_cb(struct _lv_disp_drv_t *disp_drv, lv_area_t *area)
{
  if(area->x1 % 2 !=0)area->x1--;
  if(area->y1 % 2 !=0)area->y1--;
  
  if(area->x2 %2 ==0)area->x2++;
  if(area->y2 %2 ==0)area->y2++;
}

void Gauge::increase_lvgl_tick(void *arg) {
  // Tell LVGL how many milliseconds has elapsed 
  lv_tick_inc(LV_TIMER_PERIOD_MS);
}

// initialize array of gauge objects
void Gauge::createGaugeImages(lv_obj_t *parent) {
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

// fill out LVGL object sources/origins for each gauge index
void Gauge::assignGaugeImages(lv_obj_t *parent)
{
  for (int i = 0; i < (sizeof(GAUGE_IND_POSITIONS) / sizeof(uint16_t)) / 2; i++){
    _gauge_img_ind[i] = lv_img_create(parent);
    lv_img_set_src(_gauge_img_ind[i], &_gauge_img_dsc_ind[i][0]);
    lv_obj_set_pos(_gauge_img_ind[i], GAUGE_IND_POSITIONS[i][0], GAUGE_IND_POSITIONS[i][1]);
  }
}

// Gauge.paintIndex()
// Parameters:
// - index: index of the gauge to paint (0 thru 8)
// - state: color to paint index (0 = off/black, 1 = on/FG color)
void Gauge::paintIndex(char index, char state) 
{
  if (state == INDEX_ON) {
    lv_img_set_src(_gauge_img_ind[index], &_gauge_img_dsc_ind[index][1]);
  }
  if (state == INDEX_OFF) {
    lv_img_set_src(_gauge_img_ind[index], &_gauge_img_dsc_ind[index][0]);
  }
  if (state == INDEX_CLEAR) {
    ;// cover with orange BG rectangle?
  }
}

void Gauge::paintIndices(char startIndex, char endIndex, char state) {
  for (int i = startIndex; i <= endIndex; i++) {
    paintIndex(i, state);
  }
}

// Gauge.paintGauge
// Paints gauge indices based on input value
// Scaled based on range of current gauge in GaugePainter.cpp->lims[]
// Parameters:
// - value: value to paint
void Gauge::paintGauge(int value) {
  bool doRedraw = false;
  char nextGaugeState[GAUGE_NUM_INDICES];

  findNextGaugeState(value, _limits, nextGaugeState);
  
  // compare the upcoming gauge state with the current one to decide on a redraw
  // if any index does not match, redraw of at least one element will occur
  for (int i = 0; i < GAUGE_NUM_INDICES; i++){
    if (nextGaugeState[i] != _gaugeState[i]){

      Serial.print(value);
      Serial.print("\t| ");
      for(int i = 0; i < GAUGE_NUM_INDICES; i++){
        Serial.print(nextGaugeState[i], DEC);
        Serial.print(" ");
      }
      Serial.print("\t| ");

      doRedraw = true;
      break;
    }    
  }
  
  // for efficient redrawing, go backwards or forwards from current top index
  // and edit only the needed indices to satisfy the next state
  int topIndex = GAUGE_NUM_INDICES - 1; // default to the maximum (all indices are colored)
  for (int i = GAUGE_NUM_INDICES - 1; i >= 0; i--){
    if (nextGaugeState[i] == 0){ // found the uppermost index to be black
      topIndex = i;
    }
  }

  

  // current number is greater than the last number, so
  // go from the current top index and color the indices orange in the CW direction
  if (doRedraw && value > _gaugeValue_raw ){
    for (int i = 0; i <= topIndex; i++){
      Serial.print(i);
      Serial.print(" orange ");
      paintIndex(i, 1); 
    }
    Serial.println();
  }
  // current number is less than the last number, so
  // go from the end and color indices black in the CCW direction
  else if (doRedraw && value < _gaugeValue_raw ){
    for (int i = GAUGE_NUM_INDICES - 1; i >= topIndex; i--){
      Serial.print(i);
      Serial.print(" black ");
      paintIndex(i, 0); 
    }
    Serial.println();
  }

  // save gauge state and value for next call to this fn
  memcpy(&_gaugeState, &nextGaugeState, GAUGE_NUM_INDICES);
  _gaugeValue_raw = value;
}

// calculate how many indices should be lit based on the input
void Gauge::findNextGaugeState(int value, Limits limits, char* outState) {
  int topIndex;

  topIndex = map(value, 
    limits.lowerLim, limits.upperLim, 
    0, GAUGE_NUM_INDICES);
  topIndex = constrain(topIndex, 0, GAUGE_NUM_INDICES);

  // Serial.print(value);
  // Serial.print("\t| ");
  for (int i = 0; i < topIndex; i++){
    outState[i] = INDEX_ON;
    // Serial.print("1 ");
  }
  for (int i = topIndex; i < GAUGE_NUM_INDICES; i++){
    outState[i] = INDEX_OFF;
    // Serial.print("0 ");
  }
  // Serial.println();
  
}

int Gauge::update(GaugeData data)
{
  _limits.lowerLim = 10;
  _limits.upperLim = 170;
  paintGauge(data.CoolantTemp);
  lv_timer_handler();
  return 0;
}

