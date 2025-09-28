/**
 * @file lv_port_fs.c
 * Implementation of LVGL filesystem driver
 * Intended for use with Waveshare ESP32-S3-Touch-AMOLED-1.75 module
 *   with an external SD card mounted in the module
 *
 * Note: for simplicity of Arduino IDE compilation, this file is written to C formatting
 * while using a .cpp file extension
 *
 * Adapted from: https://github.com/lvgl/lvgl/blob/master/examples/porting/lv_port_fs_template.c
 */

/*Copy this file as "lv_port_fs.c" and set this value to "1" to enable content*/
#if 1

/*********************
 *      INCLUDES
 *********************/
#include "lv_port_fs.h"
// #include "../../lvgl.h"
#include "pin_config.h" // GPIO pins for the screen are defined here
#include <Wire.h>
#include "SD_MMC.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void fs_init(void);

static void * fs_open(lv_fs_drv_t * drv, const char * path, lv_fs_mode_t mode);
static lv_fs_res_t fs_close(lv_fs_drv_t * drv, void * file_p);
static lv_fs_res_t fs_read(lv_fs_drv_t * drv, void * file_p, void * buf, uint32_t btr, uint32_t * br);
static lv_fs_res_t fs_write(lv_fs_drv_t * drv, void * file_p, const void * buf, uint32_t btw, uint32_t * bw);
static lv_fs_res_t fs_seek(lv_fs_drv_t * drv, void * file_p, uint32_t pos, lv_fs_whence_t whence);
// static lv_fs_res_t fs_size(lv_fs_drv_t * drv, void * file_p, uint32_t * size_p);
static lv_fs_res_t fs_tell(lv_fs_drv_t * drv, void * file_p, uint32_t * pos_p);

static void * fs_dir_open(lv_fs_drv_t * drv, const char * path);
static lv_fs_res_t fs_dir_read(lv_fs_drv_t * drv, void * rddir_p, char * fn);
static lv_fs_res_t fs_dir_close(lv_fs_drv_t * drv, void * rddir_p);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_port_fs_init(void)
{
    fs_init();

    static lv_fs_drv_t drv;
    lv_fs_drv_init(&drv);

    drv.letter = 'S';   // "S:/" for SD_MMC
    drv.cache_size = 0;

    drv.open_cb = fs_open;
    drv.close_cb = fs_close;
    drv.read_cb = fs_read;
    drv.write_cb = fs_write;
    drv.seek_cb = fs_seek;
    drv.tell_cb = fs_tell;
    drv.dir_open_cb = fs_dir_open;
    drv.dir_read_cb = fs_dir_read;
    drv.dir_close_cb = fs_dir_close;

    lv_fs_drv_register(&drv);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/*Initialize your Storage device and File system.*/
static void fs_init(void)
{
    if (!SD_MMC.begin("/sdcard", true)) {  // 1-bit mode
        Serial.println("Card Mount Failed");
        return;
    }
    uint8_t cardType = SD_MMC.cardType();
    if (cardType == CARD_NONE) {
        Serial.println("No SD card attached");
        return;
    }
    Serial.println("SD_MMC initialized");
}

/**
 * Open a file
 * @param drv       pointer to a driver where this function belongs
 * @param path      path to the file beginning with the driver letter (e.g. S:/folder/file.txt)
 * @param mode      read: FS_MODE_RD, write: FS_MODE_WR, both: FS_MODE_RD | FS_MODE_WR
 * @return          a file descriptor or NULL on error
 */
static void * fs_open(lv_fs_drv_t * drv, const char * path, lv_fs_mode_t mode)
{
    const char * file_path = path; // LVGL already strips drive letter "S:/"
    const char * sd_prefix = "/";  // SD_MMC root

    String full_path = String(sd_prefix) + file_path;

    const char * open_mode = (mode == LV_FS_MODE_WR)             ? FILE_WRITE :
                             (mode == LV_FS_MODE_RD)             ? FILE_READ  :
                             (mode == (LV_FS_MODE_WR | LV_FS_MODE_RD)) ? "r+" : NULL;

    if (!open_mode) return NULL;

    File f = SD_MMC.open(full_path, open_mode);
    if (!f) return NULL;

    // Allocate heap copy of File object
    File * fp = new File(f);
    return fp;
}

/**
 * Close an opened file
 * @param drv       pointer to a driver where this function belongs
 * @param file_p    pointer to a file_t variable. (opened with fs_open)
 * @return          LV_FS_RES_OK: no error or  any error from @lv_fs_res_t enum
 */
static lv_fs_res_t fs_close(lv_fs_drv_t * drv, void * file_p)
{
    File * fp = (File *)file_p;
    if (!fp) return LV_FS_RES_INV_PARAM;

    fp->close();
    delete fp;
    return LV_FS_RES_OK;
}

/**
 * Read data from an opened file
 * @param drv       pointer to a driver where this function belongs
 * @param file_p    pointer to a file_t variable.
 * @param buf       pointer to a memory block where to store the read data
 * @param btr       number of Bytes To Read
 * @param br        the real number of read bytes (Byte Read)
 * @return          LV_FS_RES_OK: no error or  any error from @lv_fs_res_t enum
 */
static lv_fs_res_t fs_read(lv_fs_drv_t * drv, void * file_p, void * buf, uint32_t btr, uint32_t * br)
{
    File * fp = (File *)file_p;
    if (!fp) return LV_FS_RES_INV_PARAM;

    *br = fp->read((uint8_t *)buf, btr);
    return LV_FS_RES_OK;
}

/**
 * Write into a file
 * @param drv       pointer to a driver where this function belongs
 * @param file_p    pointer to a file_t variable
 * @param buf       pointer to a buffer with the bytes to write
 * @param btw       Bytes To Write
 * @param bw        the number of real written bytes (Bytes Written). NULL if unused.
 * @return          LV_FS_RES_OK: no error or  any error from @lv_fs_res_t enum
 */
static lv_fs_res_t fs_write(lv_fs_drv_t * drv, void * file_p, const void * buf, uint32_t btw, uint32_t * bw)
{
    File * fp = (File *)file_p;
    if (!fp) return LV_FS_RES_INV_PARAM;

    *bw = fp->write((const uint8_t *)buf, btw);
    return (*bw == btw) ? LV_FS_RES_OK : LV_FS_RES_FS_ERR;
}

/**
 * Set the read write pointer. Also expand the file size if necessary.
 * @param drv       pointer to a driver where this function belongs
 * @param file_p    pointer to a file_t variable. (opened with fs_open )
 * @param pos       the new position of read write pointer
 * @param whence    tells from where to interpret the `pos`. See @lv_fs_whence_t
 * @return          LV_FS_RES_OK: no error or  any error from @lv_fs_res_t enum
 */
static lv_fs_res_t fs_seek(lv_fs_drv_t * drv, void * file_p, uint32_t pos, lv_fs_whence_t whence)
{
    File * fp = (File *)file_p;
    if (!fp) return LV_FS_RES_INV_PARAM;

    SeekMode mode = (whence == LV_FS_SEEK_SET) ? SeekSet :
                    (whence == LV_FS_SEEK_CUR) ? SeekCur : SeekEnd;

    if (fp->seek(pos, mode)) return LV_FS_RES_OK;
    return LV_FS_RES_FS_ERR;
}
/**
 * Give the position of the read write pointer
 * @param drv       pointer to a driver where this function belongs
 * @param file_p    pointer to a file_t variable.
 * @param pos_p     pointer to to store the result
 * @return          LV_FS_RES_OK: no error or  any error from @lv_fs_res_t enum
 */
static lv_fs_res_t fs_tell(lv_fs_drv_t * drv, void * file_p, uint32_t * pos_p)
{
    File * fp = (File *)file_p;
    if (!fp) return LV_FS_RES_INV_PARAM;

    *pos_p = fp->position();
    return LV_FS_RES_OK;
}

/**
 * Initialize a 'lv_fs_dir_t' variable for directory reading
 * @param drv       pointer to a driver where this function belongs
 * @param path      path to a directory
 * @return          pointer to the directory read descriptor or NULL on error
 */
static void * fs_dir_open(lv_fs_drv_t * drv, const char * path)
{
    String full_path = "/" + String(path);
    File dir = SD_MMC.open(full_path);
    if (!dir || !dir.isDirectory()) return NULL;

    File * dp = new File(dir);
    return dp;
}

/**
 * Read the next filename form a directory.
 * The name of the directories will begin with '/'
 * @param drv       pointer to a driver where this function belongs
 * @param rddir_p   pointer to an initialized 'lv_fs_dir_t' variable
 * @param fn        pointer to a buffer to store the filename
 * @return          LV_FS_RES_OK: no error or  any error from @lv_fs_res_t enum
 */
static lv_fs_res_t fs_dir_read(lv_fs_drv_t * drv, void * rddir_p, char * fn)
{
    File * dir = (File *)rddir_p;
    if (!dir) return LV_FS_RES_INV_PARAM;

    File entry = dir->openNextFile();
    if (!entry) {
        fn[0] = '\0';  // No more files
        return LV_FS_RES_OK;
    }

    if (entry.isDirectory()) {
        sprintf(fn, "/%s", entry.name()); // prefix dirs with "/"
    } else {
        strcpy(fn, entry.name());
    }
    entry.close();
    return LV_FS_RES_OK;
}

/**
 * Close the directory reading
 * @param drv       pointer to a driver where this function belongs
 * @param rddir_p   pointer to an initialized 'lv_fs_dir_t' variable
 * @return          LV_FS_RES_OK: no error or  any error from @lv_fs_res_t enum
 */
static lv_fs_res_t fs_dir_close(lv_fs_drv_t * drv, void * rddir_p)
{
    File * dir = (File *)rddir_p;
    if (!dir) return LV_FS_RES_INV_PARAM;

    dir->close();
    delete dir;
    return LV_FS_RES_OK;
}

#else /*Enable this file at the top*/

/*This dummy typedef exists purely to silence -Wpedantic.*/
typedef int keep_pedantic_happy;
#endif
