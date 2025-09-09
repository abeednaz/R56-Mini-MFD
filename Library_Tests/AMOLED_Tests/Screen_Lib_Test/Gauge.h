#ifndef __GAUGE_H
#define __GAUGE_H

#include <lvgl.h>
#include "pin_config.h" // GPIO pins for the screen are defined here
#include <Arduino_GFX_Library.h>
#include "Gauge_Icons.h" // Bitmap images for gauge icons are defined here

#define LV_TIMER_PERIOD_MS 2
#define GAUGE_NUM_INDICES 9

typedef enum
{
  GAUGE_TYPE_COOLANT_TEMP,
  GAUGE_TYPE_OIL_TEMP,
  GAUGE_TYPE_OIL_PRESS,
  GAUGE_TYPE_BOOST_PRESS,
  // G_METER,
  GAUGE_TYPE_TRIP_INSIGHTS,
  GAUGE_TYPE_MAX,
} GaugeType;

typedef struct
{
  int upperLim;
  int lowerLim;
} Limits;

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
    void setType(GaugeType type);
    GaugeType getType();
    int update(GaugeData data);
    void printDebugMsg(String s);

  private:
    // Necessary variables for drawing to screen
    Arduino_DataBus *_bus;
    Arduino_GFX *_gfx;
    lv_disp_draw_buf_t _draw_buf;
    lv_color_t _buf[LCD_WIDTH * 10];

    // screen specifics
    lv_obj_t *_label; // number displayed on screen
    lv_style_t _label_style; // style for the number displayed
    lv_obj_t *_scr; // active screen -- need to have multiple for different gauge types

    // array to refer to image objects
    lv_obj_t *_gauge_img_ind[GAUGE_NUM_INDICES]; // pointer to image bitmaps
    lv_img_dsc_t _gauge_img_dsc_ind[GAUGE_NUM_INDICES][2]; // pointer to image descriptors

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

    void createGaugeImages(lv_obj_t *parent); // during init, fill out array of image objects
    void assignGaugeImages(lv_obj_t *parent);

    void paintGauge(int value);
    void paintValue(int value);
    void paintIndex(char index, char state);
    void paintIndices(char startIndex, char endIndex, char state);
    void findNextGaugeState(int value, Limits limits, char* outState);
    void paintIcon(char icon);
    void paintUnit(char unit);
    void clearIcon();
    void clearUnit();
    void clearNumber();
    void getLimits(int lims[2]);
    void startupAnimation();

};


#endif // __GAUGE_H
