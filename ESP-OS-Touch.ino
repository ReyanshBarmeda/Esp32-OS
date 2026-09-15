#include <WiFi.h>
#include <SPI.h>
#include <SD.h>              
#include <SimpleFtpServer.h> 
#include <TFT_eSPI.h> 
#include <lvgl.h> 
#include "time.h" 
#include "secret_config.h" // 🛡️ Securely loads your private credentials locally

// --- Core Hardware Layout Specifications ---
#define SCREEN_WIDTH  320
#define SCREEN_HEIGHT 240
#define SD_CS_PIN     5      
#define TOUCH_CS_PIN  21     // Dedicated Chip Select for touchscreen controller

// --- Network Setup Links ---
const char* ssid     = SECRET_SSID;     // Mapped from secret_config.h
const char* password = SECRET_PASSWORD; // Mapped from secret_config.h

// --- Network Time Profiles ---
const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 19800; // Timezone Offset in Seconds (+5:30)
const int   daylightOffset_sec = 0;

FtpServer ftpSrv;
TFT_eSPI tft = TFT_eSPI(); 

// --- LVGL Interface Layout Widgets ---
static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf[SCREEN_WIDTH * 10]; 
static lv_obj_t * label_status;
static lv_obj_t * label_time; 
static lv_obj_t * textarea_log;
static lv_obj_t * bar_storage; 
static lv_obj_t * label_storage_txt;

// ==========================================
// 1. HARDWARE CALLBACK ENGINES (TFT & Touch)
// ==========================================
void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p) {
    uint32_t w = (area->x2 - area->x1 + 1);
    uint32_t h = (area->y2 - area->y1 + 1);
    tft.startWrite();
    tft.setAddrWindow(area->x1, area->y1, w, h);
    tft.pushColors((uint16_t *)&color_p->full, w * h, true);
    tft.endWrite();
    lv_disp_flush_ready(disp);
}

// Low-level touch reader that reports touch coordinates to LVGL
void my_touchpad_read(lv_indev_drv_t * indev_driver, lv_indev_data_t * data) {
    uint16_t touchX, touchY;
    bool touched = tft.getTouch(&touchX, &touchY, 600); // 600 = threshold sensitivity

    if(!touched) {
        data->state = LV_INDEV_STATE_REL;
    } else {
        data->state = LV_INDEV_STATE_PR;
        data->point.x = touchX;
        data->point.y = touchY;
    }
}

// Graphical Desktop Button Interaction Event
static void btn_event_cb(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);
    if(code == LV_EVENT_CLICKED) {
        int state = digitalRead(2);
        digitalWrite(2, !state);
        if(!state) {
            lv_textarea_add_text(textarea_log, "[GUI] LED turned ON\n");
        } else {
            lv_textarea_add_text(textarea_log, "[GUI] LED turned OFF\n");
        }
    }
}

// ==========================================
// 2. DESKTOP INTERFACE ARCHITECTURE SETUP
// ==========================================
void create_gui_desktop() {
    // A. Desktop Status Bar Layout Frame
    lv_obj_t * header = lv_obj_create(lv_scr_act());
    lv_obj_set_size(header, SCREEN_WIDTH, 35);
    lv_obj_align(header, LV_ALIGN_TOP_MID, 0, 0);
    lv_obj_set_style_bg_color(header, lv_palette_main(LV_PALETTE_BLUE_GREY), 0);
    lv_obj_set_style_radius(header, 0, 0); 

    label_status = lv_label_create(header);
    lv_label_set_text(label_status, "WiFi: Offline");
    lv_obj_align(label_status, LV_ALIGN_LEFT_MID, -5, 0);
    lv_obj_set_style_text_font(label_status, &lv_font_montserrat_10, 0);

    label_time = lv_label_create(header);
    lv_label_set_text(label_time, "--:--:--");
    lv_obj_align(label_time, LV_ALIGN_RIGHT_MID, 5, 0);
    lv_obj_set_style_text_font(label_time, &lv_font_montserrat_10, 0);

    // B. Interactive Controller Desktop Button
    lv_obj_t * btn = lv_btn_create(lv_scr_act());
    lv_obj_set_size(btn, 120, 40);
    lv_obj_align(btn, LV_ALIGN_LEFT_MID, 15, -40);
    lv_obj_add_event_cb(btn, btn_event_cb, LV_EVENT_ALL, NULL);

    lv_obj_t * label_btn = lv_label_create(btn);
    lv_label_set_text(label_btn, "Toggle LED");
    lv_obj_center(label_btn);

    // C. Data Capacity Matrix Module Widget
    lv_obj_t * storage_box = lv_obj_create(lv_scr_act());
    lv_obj_set_size(storage_box, 120, 65);
    lv_obj_align(storage_box, LV_ALIGN_LEFT_MID, 15, 25);
    lv_obj_set_style_radius(storage_box, 4, 0);

    label_storage_txt = lv_label_create(storage_box);
    lv_label_set_text(label_storage_txt, "SD: Searching...");
    lv_obj_align(label_storage_txt, LV_ALIGN_TOP_MID, 0, -5);
    lv_obj_set_style_text_font(label_storage_txt, &lv_font_montserrat_10, 0);

    bar_storage = lv_bar_create(storage_box);
    lv_obj_set_size(bar_storage, 90, 12);
    lv_obj_align(bar_storage, LV_ALIGN_BOTTOM_MID, 0, -2);
    lv_bar_set_range(bar_storage, 0, 100);

    // D. Central Operating Diagnostics Logger
    textarea_log = lv_textarea_create(lv_scr_act());
    lv_obj_set_size(textarea_log, 150, 160);
    lv_obj_align(textarea_log, LV_ALIGN_RIGHT_MID, -15, 15);
    lv_textarea_set_text(textarea_log, "--- OS Boot Log ---\n");
    lv_obj_set_style_text_font(textarea_log, &lv_font_montserrat_10, 0);
}

// ==========================================
// 3. MAIN RUNTIME BOOT CONFIGURATION
// ==========================================
void setup() {
    Serial.begin(115200);
    pinMode(2, OUTPUT);
    digitalWrite(2, LOW);

    // Initializing Screen Matrix
    tft.init();
    tft.setRotation(1);

    // Initializing GUI Engine Structures
    lv_init();
    lv_disp_draw_buf_init(&draw_buf, buf, NULL, SCREEN_WIDTH * 10);

    // Bind Screen Driver into Graphical Pipeline
    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = SCREEN_WIDTH;
    disp_drv.ver_res = SCREEN_HEIGHT;
    disp_drv.flush_cb = my_disp_flush;
    disp_drv.draw_buf = &draw_buf;
    lv_disp_drv_register(&disp_drv);

    // Bind Touch Driver into Graphical Pipeline
    static lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = my_touchpad_read;
    lv_indev_drv_register(&indev_drv);

    // Render visual workspace framework assets
    create_gui_desktop();
    lv_textarea_add_text(textarea_log, "[SYS] UI Engine Ready.\n");

    // Initialize Mass Storage Unit via SPI Link
    lv_textarea_add_text(textarea_log, "[SYS] Checking SD Module...\n");
    if (SD.begin(SD_CS_PIN)) {
        uint64_t totalMB = SD.totalBytes() / (1024 * 1024);
        lv_textarea_add_text(textarea_log, "[SYS] SD Card Verified!\n");
        lv_textarea_add_text(textarea_log, (String("[SYS] Size: ") + totalMB + " MB\n").c_str());
        ftpSrv.begin("admin", "password"); 
    } else {
        lv_textarea_add_text(textarea_log, "[ERR] SD Mount Failed!\n");
    }

    // Connect to Wireless Access Link using secure credential hooks
    WiFi.begin(ssid, password);
    lv_textarea_add_text(textarea_log, "[NET] Scanning WiFi Link...\n");
}

// ==========================================
// 4. MAIN CENTRAL PROCESSOR LOOP
// ==========================================
void loop() {
    static uint32_t last_sec_tick = 0;
    
    // Process layout updates and evaluate touch points
    lv_timer_handler(); 
    delay(5);

    static bool connected_flag = false;
    if (WiFi.status() == WL_CONNECTED) {
        if (!connected_flag) {
            connected_flag = true;
            lv_label_set_text_fmt(label_status, "IP: %s", WiFi.localIP().toString().c_str());
            lv_textarea_add_text(textarea_log, "[NET] Connection Up!\n");
            
            configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
            lv_textarea_add_text(textarea_log, "[TIME] Clock Calibrated.\n");
            lv_textarea_add_text(textarea_log, "[FTP] Server Active.\n");
        }
        
        // Feed FTP daemon cycle to process incoming desktop transactions
        ftpSrv.handleFTP(); 
    }

    // Dynamic Performance Checking Framework (Triggers every 1 second)
    if (millis() - last_sec_tick > 1000) {
        last_sec_tick = millis();

        if (connected_flag) {
            struct tm timeinfo;
            if (getLocalTime(&timeinfo)) {
                lv_label_set_text_fmt(label_time, "%02d:%02d:%02d", timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
            }
        }

        uint64_t totalBytes = SD.totalBytes();
        uint64_t usedBytes = SD.usedBytes();
        if (totalBytes > 0) {
            int storagePercent = (usedBytes * 100) / totalBytes;
            lv_bar_set_value(bar_storage, storagePercent, LV_ANIM_ON);
            lv_label_set_text_fmt(label_storage_txt, "SD: %d%% Used", storagePercent);
        }
    }
}
