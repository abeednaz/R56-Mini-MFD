#include "Gauge.h"


#define INDEX_ON 1
#define INDEX_OFF 0
#define INDEX_CLEAR 2

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

  // Configure main screen
  // future note: multiple screens needed for having G meter GUI, trip insights GUI
  _main_screen = lv_scr_act(); 
  lv_obj_set_style_bg_color(_main_screen, lv_color_hsv_to_rgb(11, 100, 25), LV_PART_MAIN);
  createGaugeImages(_main_screen);
  assignGaugeImages(_main_screen);

  // Configure number in the center of the gauge
  _label = lv_label_create(_main_screen);
  lv_style_init(&_label_style);
  lv_style_set_text_letter_space(&_label_style, -48);
  lv_style_set_text_color(&_label_style, lv_color_hex(0xfa4300)); 
  lv_obj_set_style_text_font(_label, &MINI_font_numbers, LV_PART_MAIN); 
  lv_obj_add_style(_label, &_label_style, int(LV_PART_MAIN) | int(LV_STATE_DEFAULT));

    // Set up timer for LVGL
  const esp_timer_create_args_t lvgl_tick_timer_args = {
    .callback = &Gauge::increase_lvgl_tick,
    .arg = nullptr,
    .dispatch_method = ESP_TIMER_TASK,
    .name = "lvgl_tick",
    .skip_unhandled_events = false
  };
  
  esp_timer_handle_t lvgl_tick_timer = NULL;
  esp_timer_create(&lvgl_tick_timer_args, &lvgl_tick_timer);
  esp_timer_start_periodic(lvgl_tick_timer, LV_TIMER_PERIOD_MS * 1000);

  // initialize gauge as an oil temp gauge
  setType(GAUGE_TYPE_OIL_TEMP);
}

int Gauge::setType(GaugeType type)
{
  // avoid out of range error
  if (type < 0 || type >= GAUGE_TYPE_MAX){
    return -1;
  }
  _gaugeType = type;
  // set the limits for the current gauge type (predefined)
  _limits = GaugeLimits[int(_gaugeType)];
  // update the icon
  paintIcon(_gaugeType);
  return 0;
}

GaugeType Gauge::getType()
{
  return _gaugeType;
}

void Gauge::paintIcon(GaugeType type) // add param: GaugeType icon
{
  int index = (int)type;
  // draw icon
  lv_img_set_src(_curr_sensor_icon,  &_gauge_sensor_icons_dsc[index]);
  lv_obj_set_pos(_curr_sensor_icon, GAUGE_ICON_POSITIONS[index][0], GAUGE_ICON_POSITIONS[index][1]);
  // draw unit
  lv_img_set_src(_curr_unit_icon,  &_gauge_unit_icons_dsc[index]);
  lv_obj_set_pos(_curr_unit_icon, GAUGE_UNIT_POSITIONS[index][0], GAUGE_UNIT_POSITIONS[index][1]);
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
  // Gauge measurement index icons
  _gauge_index_icons_dsc[0][0] = img_ind8_off;
  _gauge_index_icons_dsc[0][1] = img_ind8_on;
  _gauge_index_icons_dsc[1][0] = img_ind9_off;
  _gauge_index_icons_dsc[1][1] = img_ind9_on;
  _gauge_index_icons_dsc[2][0] = img_ind10_off;
  _gauge_index_icons_dsc[2][1] = img_ind10_on;
  _gauge_index_icons_dsc[3][0] = img_ind11_off;
  _gauge_index_icons_dsc[3][1] = img_ind11_on;
  _gauge_index_icons_dsc[4][0] = img_ind12_off;
  _gauge_index_icons_dsc[4][1] = img_ind12_on;
  _gauge_index_icons_dsc[5][0] = img_ind1_off;
  _gauge_index_icons_dsc[5][1] = img_ind1_on;
  _gauge_index_icons_dsc[6][0] = img_ind2_off;
  _gauge_index_icons_dsc[6][1] = img_ind2_on;
  _gauge_index_icons_dsc[7][0] = img_ind3_off;
  _gauge_index_icons_dsc[7][1] = img_ind3_on;
  _gauge_index_icons_dsc[8][0] = img_ind4_off;
  _gauge_index_icons_dsc[8][1] = img_ind4_on;

  // Gauge sensor icons (oiltemp, water temp, etc. + their corresponding unit)
  _gauge_sensor_icons_dsc[0] = img_oil_icon;
  _gauge_unit_icons_dsc[0] = img_degC_unit;
  _gauge_sensor_icons_dsc[1] = img_coolant_icon;
  _gauge_unit_icons_dsc[1] = img_degC_unit;
  _gauge_sensor_icons_dsc[2] = img_oil_icon;
  _gauge_unit_icons_dsc[2] = img_PSI_unit;
  _gauge_sensor_icons_dsc[3] = img_fuel_icon;
  _gauge_unit_icons_dsc[3] = img_PSI_unit;
  _gauge_sensor_icons_dsc[4] = img_turbo_icon;
  _gauge_unit_icons_dsc[4] = img_PSI_unit;
}

// fill out LVGL object sources/origins for each gauge index
void Gauge::assignGaugeImages(lv_obj_t *parent)
{
  // Gauge measurement index images
  for (int i = 0; i < (sizeof(GAUGE_IND_POSITIONS) / sizeof(uint16_t)) / 2; i++){
    _gauge_index_icons[i] = lv_img_create(parent);
    lv_img_set_src(_gauge_index_icons[i], &_gauge_index_icons_dsc[i][0]);
    lv_obj_set_pos(_gauge_index_icons[i], GAUGE_IND_POSITIONS[i][0], GAUGE_IND_POSITIONS[i][1]);
  }

  _curr_sensor_icon = lv_img_create(parent);
  _curr_unit_icon = lv_img_create(parent);

}

// Gauge.paintIndex()
// Parameters:
// - index: index of the gauge to paint (0 thru 8)
// - state: color to paint index (0 = off/black, 1 = on/FG color)
void Gauge::paintIndex(int index, char state) 
{
  if (state == INDEX_ON) {
    lv_img_set_src(_gauge_index_icons[index], &_gauge_index_icons_dsc[index][1]);
  }
  if (state == INDEX_OFF) {
    lv_img_set_src(_gauge_index_icons[index], &_gauge_index_icons_dsc[index][0]);
  }
  if (state == INDEX_CLEAR) {
    ;// cover with orange BG rectangle?
  }
}

void Gauge::paintIndices(int startIndex, int endIndex, char state) {
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
      paintIndex(i, 1); 
    }
  }
  // current number is less than the last number, so
  // go from the end and color indices black in the CCW direction
  else if (doRedraw && value < _gaugeValue_raw ){
    for (int i = GAUGE_NUM_INDICES - 1; i >= topIndex; i--){
      paintIndex(i, 0); 
    }
  }

  // save gauge state and value for next call to this fn
  memcpy(&_gaugeState, &nextGaugeState, GAUGE_NUM_INDICES);
  _gaugeValue_raw = value;
}

// calculate how many indices should be lit based on the input
void Gauge::findNextGaugeState(int value, Limits limits, char* outState) {
  int topIndex;

  // map the range of the value based on the limits for the current sensor
  topIndex = map(value, 
    limits.lowerLim, limits.upperLim, 
    0, GAUGE_NUM_INDICES);
  topIndex = constrain(topIndex, 0, GAUGE_NUM_INDICES);

  // set the indices to on/off for drawing later
  for (int i = 0; i < topIndex; i++){
    outState[i] = INDEX_ON;
  }
  for (int i = topIndex; i < GAUGE_NUM_INDICES; i++){
    outState[i] = INDEX_OFF;
  }
  
}

void Gauge::paintValue(int value)
{
  char value_str[8];

  // set value into the lvgl label text
  itoa(value, value_str, 10);
  lv_label_set_text(_label, value_str);
  lv_obj_align(_label, LV_ALIGN_CENTER, 0, 0);
}

int Gauge::update(GaugeData data)
{
  int valueToUpdate = 0;
  // save the updated data struct
  _data = data;

  switch(_gaugeType){
    case GAUGE_TYPE_OIL_TEMP:
      valueToUpdate = _data.OilTemp;
      break; 
    case GAUGE_TYPE_COOLANT_TEMP:
      valueToUpdate = _data.CoolantTemp;
      break; 
    case GAUGE_TYPE_OIL_PRESS:
      valueToUpdate = _data.OilPress;
      break; 
    case GAUGE_TYPE_FUEL_PRESS:
      valueToUpdate = _data.FuelPress;
      break; 
    case GAUGE_TYPE_BOOST_PRESS:
      valueToUpdate = _data.BoostPress;
      break; 
    case GAUGE_TYPE_MAX:
      break;
  }
  
  paintGauge(valueToUpdate);
  paintValue(valueToUpdate);
  lv_timer_handler();
  return 0;
}

