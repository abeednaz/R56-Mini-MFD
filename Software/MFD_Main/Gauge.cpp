/**
 * @file Gauge.cpp
 * Implementation of Gauge library
 * Intended for use with Waveshare ESP32-S3-Touch-AMOLED-1.75 module
 */

/******************************************************************************
*                              LIBRARY INCLUDES
******************************************************************************/
#include "Gauge.h"

// AMOLED devboard pin definitions
#include "pin_config.h" 

// SD card includes
#include <Wire.h>
#include <SD_MMC.h>

#include "lv_port_fs.h" // LVGL filesystem driver for pulling images/fonts
#include "lv_screen_driver.h" // LVGL screen driver for accessing screen

/******************************************************************************
*                                  DEFINES
******************************************************************************/

#define IMAGES_ROOT "/images/main_gauge/"

#define INDEX_ON 1
#define INDEX_OFF 0
#define INDEX_CLEAR 2

#define ICON_TYPE 1
#define UNIT_TYPE 2
#define IND_TYPE  3
#define TRIP_ICON_TYPE 4

#define BINFILE_HEADER_SIZE 4


/******************************************************************************
*                              HELPER FUNCTIONS
******************************************************************************/

namespace ImageDrawing {
  // SD_Init()
  // Parameters: none
  // Returns: SD card filesize if able; if any failures occur then 0
  int SD_Init() {
    Wire.begin(IIC_SDA, IIC_SCL);
    SD_MMC.setPins(SDMMC_CLK, SDMMC_CMD, SDMMC_DATA);

    if (!SD_MMC.begin("/sdcard", true)) {
      Serial.println("Card Mount Failed");
      return 0;
    }

    uint8_t cardType = SD_MMC.cardType();
    if (cardType == CARD_NONE) {
      Serial.println("No SD_MMC card attached");
      return 0;
    }

    Serial.print("SD_MMC Card Type: ");
    if (cardType == CARD_MMC) {
      Serial.println("MMC");
    } else if (cardType == CARD_SD) {
      Serial.println("SDSC");
    } else if (cardType == CARD_SDHC) {
      Serial.println("SDHC");
    } else {
      Serial.println("UNKNOWN");
      return 0;
    }

    uint64_t cardSize = SD_MMC.cardSize() / (1024 * 1024);
    Serial.println("SD_MMC Card Size: " + String(cardSize) + "MB");
    return cardSize;
  }

  // Load_Image_Data_To_PSRAM()
  // Given an SD card individual filepath, allocates a picture into PSRAM
  // Parameters:
  // - *path: string for the filepath to open off the SD card (.bin file)
  // - *size: int of the size received (to be read afterwards)
  // Returns:
  // - pointer to data saved in memory, or null if anything failed
  // Notes:
  // LVGL allows for setting image source at runtime using 
  // (i.e.) lv_img_set_src(icon, "S:my_icon.bin");
  // This function preloads the image into PSRAM so the source can be set more quickly
  // rather than accessing the SD card every time an image is updated
  uint8_t* Load_Image_Data_To_PSRAM(const char *path, uint32_t *size) {
      File f = SD_MMC.open(path, FILE_READ);
      // catch failure in mounted SD card
      if (!f) {
          Serial.printf("Failed to open %s\n", path);
          *size = 0;
          return nullptr;
      }

      size_t fileSize = f.size();
      // catch any files that have no data/only a header
      if (fileSize <= BINFILE_HEADER_SIZE) { 
          Serial.printf("File too small: %s\n", path);
          *size = 0;
          f.close();
          return nullptr;
      }

      // Allocate enough for everything after the header
      size_t dataSize = fileSize - BINFILE_HEADER_SIZE;
      uint8_t *buf = (uint8_t*)ps_malloc(dataSize);
      if (!buf) {
          Serial.printf("PSRAM alloc failed for %s (%d bytes)\n", path, dataSize);
          *size = 0;
          f.close();
          return nullptr;
      }

      // Skip the header data from the binfile
      f.seek(BINFILE_HEADER_SIZE);  
      // Save everything after the header data
      size_t bytesRead = f.read(buf, dataSize);
      f.close();

      if (bytesRead != dataSize) {
          Serial.printf("Read error for %s\n", path);
          free(buf);
          *size = 0;
          return nullptr;
      }

      *size = dataSize;
      return buf;
  }

  lv_img_dsc_t Create_Blank_LV_Image_Dsc(uint32_t width, uint32_t height){
    lv_img_dsc_t img_dsc = {
      .header = {
      .cf = LV_IMG_CF_RGB565A8,
      .always_zero = 0,
      .reserved = 0,
      .w = width,
      .h = height,
      },
      // data_size and data get updated by call to loadImageDataToPSRAM
      .data_size = 0, 
      .data = nullptr,
    };
    return img_dsc;
  }

  // Load_Image_Data_To_Descriptors()
  // Goes into SD card and calls load function to load many images into PSRAM
  // Parameters:
  // - *image_descriptors: pointer to LVGL image descriptors struct
  // - type: ICON_TYPE or UNIT_TYPE 
  // Returns:
  // - None (*image_descriptors is modified in this function)
  // Notes:
  // - Function is overloaded to allow for 1D lv_img_dsc_t array (icons & units)
  //   and for 2d lv_img_dsc_t array (on/off indices)

  // --- Overload for 1D array (icons, units) ---
  void Load_Image_Data_To_Descriptors(lv_img_dsc_t img_dsc_1D[], char type) {
      const uint16_t (*dims)[2];
      const char (*filenames)[32];
      char subpath[16];
      char buf1[256];
      int num_icons;

      if (type == ICON_TYPE) {
          dims = GAUGE_ICON_DIMENSIONS;
          filenames = GAUGE_ICON_FILENAMES;
          num_icons = (int)GAUGE_TYPE_MAX;
          strcpy(subpath, "icon/");
      } else if (type == UNIT_TYPE) {
          dims = GAUGE_UNIT_DIMENSIONS;
          filenames = GAUGE_UNIT_FILENAMES;
          num_icons = (int)GAUGE_TYPE_MAX;
          strcpy(subpath, "unit/");
      } else if (type == TRIP_ICON_TYPE) {
          dims = TRIP_ICON_DIMENSIONS;
          filenames = TRIP_ICON_FILENAMES;
          num_icons = (int)TRIP_NUM_ICONS;
          strcpy(subpath, "icon/");
      } else {
          return; // not valid for 1D
      }
      
      for (int i = 0; i < num_icons; i++) {
          lv_img_dsc_t img_dsc;

          // construct filepath
          sprintf(buf1, "%s%s%s", IMAGES_ROOT, subpath, filenames[i]);
          Serial.println(buf1);

          img_dsc = Create_Blank_LV_Image_Dsc(dims[i][0], dims[i][1]);
          uint8_t *img_map = Load_Image_Data_To_PSRAM(buf1, &img_dsc.data_size);
          if (img_map) { img_dsc.data = img_map; }

          // update array
          img_dsc_1D[i] = img_dsc;
          
      }
      // assignGaugeIndexImages(type);
      
  }

  // --- Overload for 2D array (indices) ---
  void Load_Image_Data_To_Descriptors(lv_img_dsc_t img_dsc_2D[][2], char type) {
      if (type != IND_TYPE) return;

      const uint16_t (*dims)[2] = GAUGE_IND_DIMENSIONS;
      char buf1[256], buf2[256];
      char indices[GAUGE_NUM_INDICES] = { 8, 9, 10, 11, 12, 1, 2, 3, 4 };

      for (int i = 0; i < GAUGE_NUM_INDICES; i++) {
          lv_img_dsc_t img_index_off_dsc;
          lv_img_dsc_t img_index_on_dsc;
          int file_ind = indices[i];

          sprintf(buf1, "%sindex/ind%d_off.bin", IMAGES_ROOT, file_ind);
          sprintf(buf2, "%sindex/ind%d_on.bin",  IMAGES_ROOT, file_ind);
          Serial.println(buf1);
          Serial.println(buf2);

          img_index_off_dsc = Create_Blank_LV_Image_Dsc(dims[i][0], dims[i][1]);
          img_index_on_dsc  = Create_Blank_LV_Image_Dsc(dims[i][0], dims[i][1]);

          uint8_t *img_index_off_map =
              Load_Image_Data_To_PSRAM(buf1, &img_index_off_dsc.data_size);
          if (img_index_off_map) { img_index_off_dsc.data = img_index_off_map; }

          uint8_t *img_index_on_map =
              Load_Image_Data_To_PSRAM(buf2, &img_index_on_dsc.data_size);
          if (img_index_on_map) { img_index_on_dsc.data = img_index_on_map; }

          img_dsc_2D[i][0] = img_index_off_dsc;
          img_dsc_2D[i][1] = img_index_on_dsc;
      }

      // assignGaugeIndexImages(type);
  }

}

/******************************************************************************
*                              PRIVATE FUNCTIONS
******************************************************************************/


// Gauge.paintIcon()
// paint the icon and unit for which sensor is being displayed
// Parameters:
// - type: which sensor type should be displayed (oiltemp, boostpress, etc.)
// Returns:
// - None
void Gauge::paintIcon(GaugeType type)
{
  int index = (int)type;
  // draw icon
  lv_img_set_src(_curr_sensor_icon,  &_gauge_sensor_icons_dsc[index]);
  lv_obj_set_pos(_curr_sensor_icon, GAUGE_ICON_POSITIONS[index][0], GAUGE_ICON_POSITIONS[index][1]);
  // draw unit
  lv_img_set_src(_curr_unit_icon,  &_gauge_unit_icons_dsc[index]);
  lv_obj_set_pos(_curr_unit_icon, GAUGE_UNIT_POSITIONS[index][0], GAUGE_UNIT_POSITIONS[index][1]);
}

void Gauge::paintTripIcons()
{
  for (int i = 0; i < TRIP_NUM_ICONS; i++){
    lv_img_set_src(_trip_comp_icons[i],  &_trip_comp_icons_dsc[i]);
    lv_obj_set_pos(_trip_comp_icons[i], TRIP_ICON_POSITIONS[i][0], TRIP_ICON_POSITIONS[i][1]);
  }
}

// Gauge.createGaugeImages()
// initialize array of gauge objects
// Parameters:
// - *parent: LVGL screen to create the image in
// Returns:
// - None
void Gauge::createGaugeImages(int gaugeView) {
  if (gaugeView == DISPLAY_TYPE_GAUGE_MAIN){
    ImageDrawing::Load_Image_Data_To_Descriptors(_gauge_index_icons_dsc, IND_TYPE);
    assignGaugeIndexImages(IND_TYPE);
    ImageDrawing::Load_Image_Data_To_Descriptors(_gauge_sensor_icons_dsc, ICON_TYPE);
    assignGaugeIndexImages(ICON_TYPE);
    ImageDrawing::Load_Image_Data_To_Descriptors(_gauge_unit_icons_dsc, UNIT_TYPE);
    assignGaugeIndexImages(UNIT_TYPE);
  }
  else if (gaugeView == DISPLAY_TYPE_TRIP_COMPUTER){
    ImageDrawing::Load_Image_Data_To_Descriptors(_trip_comp_icons_dsc, TRIP_ICON_TYPE);
    assignGaugeIndexImages(TRIP_ICON_TYPE);
  }
}

// Gauge.assignGaugeIndexImages()
// fill out LVGL object sources/origins for each gauge index
// Parameters:
// - *parent: LVGL screen to create the image in
// Returns:
// - None
void Gauge::assignGaugeIndexImages(char type)
{
  switch(type){
    case ICON_TYPE:
      _curr_sensor_icon = lv_img_create(_main_screen);
      break;
    case UNIT_TYPE:
      _curr_unit_icon = lv_img_create(_main_screen);
      break;
    case IND_TYPE:
      // Gauge measurement index images
      for (int i = 0; i < (sizeof(GAUGE_IND_POSITIONS) / sizeof(uint16_t)) / 2; i++){
        _gauge_index_icons[i] = lv_img_create(_main_screen);
        lv_img_set_src(_gauge_index_icons[i], &_gauge_index_icons_dsc[i][0]);
        lv_obj_set_pos(_gauge_index_icons[i], GAUGE_IND_POSITIONS[i][0], GAUGE_IND_POSITIONS[i][1]);
      }
      break;
    case TRIP_ICON_TYPE:
      for (int i = 0; i < TRIP_NUM_ICONS; i++){
        _trip_comp_icons[i] = lv_img_create(_trip_computer_screen);
      }
      break;
  }
  

  // _curr_sensor_icon = lv_img_create(parent);
  // _curr_unit_icon = lv_img_create(parent);
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

// Gauge.paintIndices()
// Parameters:
// - startIndex: index of the gauge to begin painting on (0 thru 8, inclusive)
// - endIndex: index of the gauge to end painting on (0 thru 8, inclusive)
// - state: color to paint index (0 = off/black, 1 = on/FG color)
void Gauge::paintIndices(int startIndex, int endIndex, char state) 
{
  for (int i = startIndex; i <= endIndex; i++) {
    paintIndex(i, state);
  }
}

// Gauge.paintGauge
// Paints gauge indices based on input value
// Scaled based on range of current gauge in GaugePainter.cpp->lims[]
// Parameters:
// - value: value of the sensor
// Returns:
// - None
void Gauge::paintGauge(int value) 
{
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

// Gauge.findNextGaugeState
// calculate how many indices should be lit based on the input
// Scaled based on range of current gauge in GaugePainter.cpp->lims[]
// Parameters:
// - value: numerical parameter
// - limits: limits of the active sensor
// - outState: array of which indices should be on/off
// Returns:
// - None (*outState is modified in this function)
void Gauge::findNextGaugeState(int value, Limits limits, char* outState) 
{
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

// Gauge.paintValue
// Prints the numerical parameter to the center of the screen
// Parameters:
// - value: numerical parameter
// Returns:
// - None
void Gauge::paintValue(int value)
{
  char value_str[8];

  // set value into the lvgl label text
  itoa(value, value_str, 10);
  lv_label_set_text(_label, value_str);
  lv_obj_align(_label, LV_ALIGN_CENTER, 0, 0);
}

/******************************************************************************
*                              PUBLIC FUNCTIONS
******************************************************************************/

// Gauge()
// Constructor for new Gauge object 
// Parameters: 
// - none
// Returns: 
// - none
Gauge::Gauge() 
{
  ;
}

// Gauge.begin()
// Initialize parameters for gauge
// Parameters: 
// - none
// Returns: 
// - none
void Gauge::begin() {
  ImageDrawing::SD_Init();
  
  lv_init();

  lv_screen_driver_init();
  lv_port_fs_init();

  // Configure main screen
  // future note: multiple screens needed for having G meter GUI, trip insights GUI
  _main_screen = lv_obj_create(NULL); 
  _trip_computer_screen = lv_obj_create(NULL); 
  viewGaugeMain();

  lv_obj_set_style_bg_color(_main_screen, lv_color_hsv_to_rgb(11, 100, 25), LV_PART_MAIN);
  lv_obj_set_style_bg_color(_trip_computer_screen, lv_color_hsv_to_rgb(11, 100, 25), LV_PART_MAIN);

  createGaugeImages(DISPLAY_TYPE_GAUGE_MAIN);
  // assignGaugeIndexImages(_main_screen);

  createGaugeImages(DISPLAY_TYPE_TRIP_COMPUTER);

  // font definition
  lv_font_t * MINI_font_numbers;
  MINI_font_numbers = lv_font_load("S:/images/main_gauge/font/MINI_font_numbers_96.bin");
  if(MINI_font_numbers == NULL) {
    Serial.println("Font load failed!");
  } else {
    Serial.println("Font loaded OK.");
  }

  // Configure number in the center of the gauge
  _label = lv_label_create(_main_screen);
  lv_style_init(&_label_style);
  lv_style_set_text_letter_space(&_label_style, -48);
  lv_style_set_text_color(&_label_style, lv_color_hex(0xfa4300)); 
  lv_obj_set_style_text_font(_label, MINI_font_numbers, LV_PART_MAIN); 
  lv_obj_add_style(_label, &_label_style, int(LV_PART_MAIN) | int(LV_STATE_DEFAULT));

  // initialize gauge as an oil temp gauge
  setType(GAUGE_TYPE_OIL_TEMP);

  paintTripIcons();
  
  // while(1){
  //   delay(5000);
  //   viewTripComputer();
  //   delay(5000);
  //   viewGaugeMain();
  // }

  // while(1); // TEMPORARY STOP
}

// Gauge.setType()
// Set the gauge type (oiltemp, boostpress, etc.)
// Parameters: 
// - type: desired GaugeType
// Returns: 
// - 0 if successful
// - -1 if failed
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

// Gauge.getType()
// Get the current gauge type (oiltemp, boostpress, etc.)
// Parameters: 
// - none
// Returns: 
// - _gaugeType: current GaugeType
GaugeType Gauge::getType()
{
  return _gaugeType;
}

void Gauge::viewGaugeMain()
{
  lv_scr_load(_main_screen);
}
void Gauge::viewTripComputer()
{
  lv_scr_load(_trip_computer_screen);
}

// Gauge.update()
// Main function for interfacing with driver program
// Updates internal GaugeData struct, prints center value, paints indices as needed
// Parameters:
// - data: a full GaugeData struct
// Returns:
// - 0 if successful
int Gauge::update(GaugeData data)
{
  int valueToUpdate = 0;
  // save the updated data struct
  _data = data;

  // choose the value to print
  switch(_gaugeType){
    case GAUGE_TYPE_OIL_TEMP:
      valueToUpdate = _data.OilTemp_C;
      break; 
    case GAUGE_TYPE_COOLANT_TEMP:
      valueToUpdate = _data.CoolantTemp_C;
      break; 
    case GAUGE_TYPE_OIL_PRESS:
      valueToUpdate = _data.OilPress_PSI;
      break; 
    case GAUGE_TYPE_FUEL_PRESS:
      valueToUpdate = _data.FuelPress_PSI;
      break; 
    case GAUGE_TYPE_BOOST_PRESS:
      valueToUpdate = _data.BoostPress_PSI;
      break; 
    case GAUGE_TYPE_MAX:
      break;
  }
  
  // update the gauge and indices with the current value
  paintGauge((int)valueToUpdate);
  paintValue((int)valueToUpdate);
  // allow LVGL to continue updating the screen
  lv_timer_handler();
  return 0;
}

// Gauge.setBrightness()
// Set brightness of the display
// Parameters:
// - brightness: 0-255
// Returns:
// - none
void Gauge::setBrightness(uint8_t brightness)
{
  lv_screen_set_brightness(brightness);
}
