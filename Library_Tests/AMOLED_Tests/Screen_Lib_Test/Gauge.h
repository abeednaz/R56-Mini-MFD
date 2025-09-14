#ifndef __GAUGE_H
#define __GAUGE_H

#include <lvgl.h>
#include "pin_config.h" // GPIO pins for the screen are defined here
#include <Arduino_GFX_Library.h>
#include "Gauge_Icons.h" // Bitmap images for gauge icons are defined here

// SD card includes
#include <Wire.h>
#include <SD_MMC.h>

#define LV_TIMER_PERIOD_MS 2
#define GAUGE_NUM_INDICES 9

#define OTEMP_MIN 10
#define OTEMP_MAX 170

#define CTEMP_MIN 10
#define CTEMP_MAX 170

#define OPRESS_MIN 9
#define OPRESS_MAX 99

#define FPRESS_MIN 0
#define FPRESS_MAX 1700

#define BPRESS_MIN 0
#define BPRESS_MAX 12

#define IMAGES_ROOT "/images/main_gauge/"

#define BINFILE_HEADER_SIZE 4

typedef enum
{
  GAUGE_TYPE_OIL_TEMP,
  GAUGE_TYPE_COOLANT_TEMP,
  GAUGE_TYPE_OIL_PRESS,
  GAUGE_TYPE_FUEL_PRESS,
  GAUGE_TYPE_BOOST_PRESS,
  // G_METER,
  //GAUGE_TYPE_TRIP_INSIGHTS,
  GAUGE_TYPE_MAX,
} GaugeType;

typedef struct
{
  int lowerLim;
  int upperLim;
} Limits;

const Limits GaugeLimits[(int)GAUGE_TYPE_MAX] = {
  {OTEMP_MIN, OTEMP_MAX},
  {CTEMP_MIN, CTEMP_MAX},
  {OPRESS_MIN, OPRESS_MAX},
  {FPRESS_MIN, FPRESS_MAX},
  {BPRESS_MIN, BPRESS_MAX}
};

// define origins (top left) of each index image
const uint16_t GAUGE_IND_POSITIONS[GAUGE_NUM_INDICES][2] = 
{
  {  30, 288 }, // top-left position of index 8
  {   6, 208 }, // top-left position of index 9
  {  30, 103 }, // top-left position of index 10
  { 103, 30  }, // top-left position of index 11
  { 208, 6   }, // top-left position of index 12
  { 288, 30  }, // top-left position of index 1
  { 341, 103 }, // top-left position of index 2
  { 363, 208 }, // top-left position of index 3
  { 341, 288 }, // top-left position of index 4
};
// define sizes of each index image
const uint16_t GAUGE_IND_DIMENSIONS[GAUGE_NUM_INDICES][2] = 
{
  { 94, 75 }, // width, height of index 8
  { 98, 50 }, // width, height of index 9
  { 94, 75 }, // width, height of index 10
  { 75, 94 }, // width, height of index 11
  { 50, 98 }, // width, height of index 12
  { 75, 94 }, // width, height of index 1
  { 94, 75 }, // width, height of index 2
  { 98, 50 }, // width, height of index 3
  { 94, 75 }, // width, height of index 4
};

// define origins (top left) of each sensor icon image
const uint16_t GAUGE_ICON_POSITIONS[(int)GAUGE_TYPE_MAX][2] = 
{
  { 155, 385 }, // top-left position of oil icon (temp)
  { 178, 346 }, // top-left position of coolant icon (temp)
  { 155, 385 }, // top-left position of oil icon (pressure)
  { 167, 332 }, // top-left position of fuel icon (pressure)
  { 191, 374 }, // top-left position of turbo icon (boost pressure)
};
// define sizes of each sensor icon image
const uint16_t GAUGE_ICON_DIMENSIONS[(int)GAUGE_TYPE_MAX][2] = 
{
  { 161,  62 }, // width, height of oil icon (temp)
  { 115, 105 }, // width, height of coolant icon (temp)
  { 161,  62 }, // width, height of oil icon (pressure)
  { 122, 122 }, // width, height of fuel icon (pressure)
  { 161,  62 }, // width, height of turbo icon (boost pressure)
};

// define origins (top left) of each unit image
const uint16_t GAUGE_UNIT_POSITIONS[(int)GAUGE_TYPE_MAX][2] = 
{
  { 209, 316 }, // top-left position of oil unit (deg C)
  { 315, 346 }, // top-left position of coolant unit (deg C)
  { 209, 316 }, // top-left position of oil unit (PSI)
  { 315, 346 }, // top-left position of fuel unit (PSI)
  { 209, 316 }, // top-left position of boost unit (PSI)
};
// define origins (top left) of each unit image
const uint16_t GAUGE_UNIT_DIMENSIONS[(int)GAUGE_TYPE_MAX][2] = 
{
  { 53, 49 }, // width, height of oil unit (deg C)
  { 53, 49 }, // width, height of coolant unit (deg C)
  { 53, 49 }, // width, height of oil unit (PSI)
  { 53, 49 }, // width, height of fuel unit (PSI)
  { 53, 49 }, // width, height of boost unit (PSI)
};

// Data type to update the gauge from external driver code
typedef struct
{
  // Main Gauge values
  // initalize them to 1 to avoid any div/0 errors
  int CoolantTemp = 0;
  int OilTemp = 0;
  int OilPress = 0;
  int BoostPress = 0;
  int BaroPress = 1;
  int MAF = 1;
  int MAP = 1;
  float AFR = 14.7;
  int RunTime = 0;
  int FuelPress = 0;

  // Trip Insights statistics
  int DriveCycleTime_sec = 0;
  float DriveCycleDist_mi = 0;
  float DriveCycleFuelUsage = 0;

  // IMU values (not yet implemented)
  float X_accel = 0; 
  float Y_accel = 0; 
  float Z_accel = 0;
} GaugeData;

class Gauge
{
  public:
    Gauge();
    void begin();
    int setType(GaugeType type);
    GaugeType getType();
    int update(GaugeData data);
    void printDebugMsg(String s);

  private:
    /* ------------------------------ PRIVATE VARIABLES ------------------------------ */

    // Necessary variables for drawing to screen
    Arduino_DataBus *_bus;
    Arduino_GFX *_gfx;
    lv_disp_draw_buf_t _draw_buf;
    lv_color_t _buf[LCD_WIDTH * 10];

    // screen specifics
    lv_obj_t *_label; // number displayed on screen
    lv_style_t _label_style; // style for the number displayed
    lv_obj_t *_main_screen; // active screen -- need to have multiple for different gauge types

    // array to refer to image objects
    lv_obj_t *_gauge_index_icons[GAUGE_NUM_INDICES]; // pointer to image bitmaps for gauge indices
    lv_img_dsc_t _gauge_index_icons_dsc[GAUGE_NUM_INDICES][2]; // pointer to image descriptors

    // variables for referring to the sensor icon images
    lv_obj_t *_curr_sensor_icon;
    lv_img_dsc_t _curr_sensor_icon_dsc;
    lv_img_dsc_t _gauge_sensor_icons_dsc[(int)GAUGE_TYPE_MAX]; // pointer to image descriptors
    
    // variables for referring to the unit images
    lv_obj_t *_curr_unit_icon;
    lv_img_dsc_t _curr_unit_icon_dsc;
    lv_img_dsc_t _gauge_unit_icons_dsc[(int)GAUGE_TYPE_MAX]; // pointer to image descriptors

    // LVGL drawing function callbacks
    static void disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p);
    static void lvgl_rounder_cb(struct _lv_disp_drv_t *disp_drv, lv_area_t *area);
    static void increase_lvgl_tick(void *arg);

    char _gaugeState[GAUGE_NUM_INDICES]; // Gauge state - how many indices are lit
    char value_str[4]; // Gauge value - printed value, up to 3 digits
    int _gaugeValue_raw; // Raw gauge value
    GaugeType _gaugeType; // Type of gauge
    Limits _limits; // Integer limits of gauge
    GaugeData _data; // Data struct for gauge


    /* ------------------------------ PRIVATE FUNCTIONS ------------------------------ */

    // uint8_t* loadImageDataToPSRAM(const char *path, uint32_t *size);
    void createGaugeImages(lv_obj_t *parent); // during init, fill out array of image objects
    void assignGaugeImages(lv_obj_t *parent);

    void paintGauge(int value);
    void paintIcon(GaugeType type);
    void paintValue(int value);
    void paintIndex(int index, char state);
    void paintIndices(int startIndex, int endIndex, char state);
    void findNextGaugeState(int value, Limits limits, char* outState);
    void paintUnit(char unit);
    void clearIcon();
    void clearUnit();
    void clearNumber();
    void getLimits(int lims[2]);
    void startupAnimation();

};


#endif // __GAUGE_H
