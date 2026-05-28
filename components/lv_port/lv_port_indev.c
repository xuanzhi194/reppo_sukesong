/**
 * @file lv_port_indev_templ.c
 *
 */

/*Copy this file as "lv_port_indev.c" and set this value to "1" to enable content*/
#if 1

/*********************
 *      INCLUDES
 *********************/
#include "lv_port_indev.h"
#include "lvgl.h"
#include "keypad.h"
#include "custom.h"
#include "buzzer.h"
#include "ec06.h"
/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

// static void touchpad_init(void);
// static void touchpad_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data);
// static bool touchpad_is_pressed(void);
// static void touchpad_get_xy(lv_coord_t * x, lv_coord_t * y);

// static void mouse_init(void);
// static void mouse_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data);
// static bool mouse_is_pressed(void);
// static void mouse_get_xy(lv_coord_t * x, lv_coord_t * y);

static void keypad_init(void);
static void keypad_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data);
static uint32_t keypad_get_key(void);

// static void encoder_init(void);
// static void encoder_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data);
// static void encoder_handler(void);
static uint32_t keypad_get_encoder(void);

// static void button_init(void);
// static void button_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data);
// static int8_t button_get_pressed_id(void);
// static bool button_is_pressed(uint8_t id);

/**********************
 *  STATIC VARIABLES
 **********************/
// lv_indev_t * indev_touchpad;
// lv_indev_t * indev_mouse;
lv_indev_t * indev_keypad;
// lv_indev_t * indev_encoder;
// lv_indev_t * indev_button;

// static int32_t encoder_diff;
// static lv_indev_state_t encoder_state;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_port_indev_init(void)
{
    // /**
    //  * Here you will find example implementation of input devices supported by LittelvGL:
    //  *  - Touchpad
    //  *  - Mouse (with cursor support)
    //  *  - Keypad (supports GUI usage only with key)
    //  *  - Encoder (supports GUI usage only with: left, right, push)
    //  *  - Button (external buttons to press points on the screen)
    //  *
    //  *  The `..._read()` function are only examples.
    //  *  You should shape them according to your hardware
    //  */

    static lv_indev_drv_t indev_drv;

    // /*------------------
    //  * Touchpad
    //  * -----------------*/

    // /*Initialize your touchpad if you have*/
    // touchpad_init();

    // /*Register a touchpad input device*/
    // lv_indev_drv_init(&indev_drv);
    // indev_drv.type = LV_INDEV_TYPE_POINTER;
    // indev_drv.read_cb = touchpad_read;
    // indev_touchpad = lv_indev_drv_register(&indev_drv);

    // /*------------------
    //  * Mouse
    //  * -----------------*/

    // /*Initialize your mouse if you have*/
    // mouse_init();

    // /*Register a mouse input device*/
    // lv_indev_drv_init(&indev_drv);
    // indev_drv.type = LV_INDEV_TYPE_POINTER;
    // indev_drv.read_cb = mouse_read;
    // indev_mouse = lv_indev_drv_register(&indev_drv);

    // /*Set cursor. For simplicity set a HOME symbol now.*/
    // lv_obj_t * mouse_cursor = lv_img_create(lv_scr_act());
    // lv_img_set_src(mouse_cursor, LV_SYMBOL_HOME);
    // lv_indev_set_cursor(indev_mouse, mouse_cursor);

    /*------------------
     * Keypad
     * -----------------*/

    /*Initialize your keypad or keyboard if you have*/
    keypad_init();

    /*Register a keypad input device*/
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_KEYPAD;
    indev_drv.read_cb = keypad_read;
    indev_keypad = lv_indev_drv_register(&indev_drv);

    /*Later you should create group(s) with `lv_group_t * group = lv_group_create()`,
     *add objects to the group with `lv_group_add_obj(group, obj)`
     *and assign this input device to group to navigate in it:
     *`lv_indev_set_group(indev_keypad, group);`*/

    /*------------------
     * Encoder
     * -----------------*/

    // /*Initialize your encoder if you have*/
    // encoder_init();

    // /*Register a encoder input device*/
    // lv_indev_drv_init(&indev_drv);
    // indev_drv.type = LV_INDEV_TYPE_ENCODER;
    // indev_drv.read_cb = encoder_read;
    // indev_encoder = lv_indev_drv_register(&indev_drv);

    /*Later you should create group(s) with `lv_group_t * group = lv_group_create()`,
     *add objects to the group with `lv_group_add_obj(group, obj)`
     *and assign this input device to group to navigate in it:
     *`lv_indev_set_group(indev_encoder, group);`*/

    // /*------------------
    //  * Button
    //  * -----------------*/

    // /*Initialize your button if you have*/
    // button_init();

    // /*Register a button input device*/
    // lv_indev_drv_init(&indev_drv);
    // indev_drv.type = LV_INDEV_TYPE_BUTTON;
    // indev_drv.read_cb = button_read;
    // indev_button = lv_indev_drv_register(&indev_drv);

    // /*Assign buttons to points on the screen*/
    // static const lv_point_t btn_points[2] = {
    //     {10, 10},   /*Button 0 -> x:10; y:10*/
    //     {40, 100},  /*Button 1 -> x:40; y:100*/
    // };
    // lv_indev_set_button_points(indev_button, btn_points);
}


/*------------------
 * Keypad
 * -----------------*/
/*Initialize your keypad*/
static void keypad_init(void)
{
    /*Your code comes here*/
    // keypad_io_cfg(GPIO_NUM_1);
    // keypad_io_cfg(GPIO_NUM_2);
    // keypad_io_cfg(GPIO_NUM_3);
    // keypad_io_cfg(GPIO_NUM_4);
    keypad_io_cfg(GPIO_NUM_5);

}

/*Will be called by the library to read the mouse*/
static void keypad_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data)
{
    static uint32_t last_key = 0;

    /*Get whether the a key is pressed and save the pressed key*/
    uint32_t act_key = keypad_get_encoder();
    // uint32_t act_key = keypad_get_key();
    if(act_key != 0) {
        data->state = LV_INDEV_STATE_PR;

        /*Translate the keys to LVGL control characters according to your key definitions*/
        switch(act_key) {
            case 1:
                act_key = LV_KEY_NEXT;
                break;
            case 2:
                act_key = LV_KEY_PREV;
                break;
            case 3:
                act_key = LV_KEY_LEFT;
                break;
            case 4:
                act_key = LV_KEY_RIGHT;
                break;
            case 5:
                act_key = LV_KEY_ENTER;
                break;
        }

        last_key = act_key;
    }
    else {
        data->state = LV_INDEV_STATE_REL;
    }

    data->key = last_key;
}

/*Get the currently being pressed key.  0 if no key is pressed*/
static uint32_t keypad_get_key(void)
{
    /*Your code comes here*/

    if(keypad_get_key_value(2)==1){
        if(keyboard_start_en == 1){
            return 4;
        }else{
            return 1;
        }
    }else if(keypad_get_key_value(1)==1){
        if(keyboard_start_en == 1){
            return 3;
        }else{
            return 2;
        }
    }else if(keypad_get_key_value(3)==1){
        return 3;  
    }else if(keypad_get_key_value(4)==1){
        return 4;
    }else if(keypad_get_key_value(5)==1){
        return 5;  
    }
    return 0;
}

/*get key from encoder function*/
static uint32_t keypad_get_encoder(void)
{
    /*Your code comes here*/
    /* ENTER 键优化：短时硬件去抖 + 最小触发间隔。
     * 相比“长释放窗口”(450ms)手感更跟手，同时仍避免连续快速切屏。 */
    #define ENTER_EDGE_DEBOUNCE_MS 35u
    #define ENTER_MIN_INTERVAL_MS  180u
    static uint8_t  enter_raw_last = 0;
    static uint8_t  enter_stable   = 0;
    static uint8_t  enter_consumed = 0;     /* 稳定按下后是否已上报本次点击 */
    static uint8_t  enter_has_emit = 0;
    static uint32_t enter_raw_ts   = 0;     /* 原始电平最近变化时间 */
    static uint32_t enter_emit_ts  = 0;     /* 上一次有效点击时间 */

    uint8_t enter_raw = (keypad_get_key_value(5) == 1) ? 1 : 0;
    uint32_t now_ms = lv_tick_get();

    if (enter_raw != enter_raw_last) {
        enter_raw_last = enter_raw;
        enter_raw_ts = now_ms;
    }

    /* 原始电平保持足够时间后才更新稳定态 */
    if ((uint32_t)(now_ms - enter_raw_ts) >= ENTER_EDGE_DEBOUNCE_MS &&
        enter_stable != enter_raw_last) {
        enter_stable = enter_raw_last;
        if (enter_stable == 0) {
            enter_consumed = 0; /* 松开后允许下一次点击 */
        }
    }

    if (enter_stable && !enter_consumed) {
        if (!enter_has_emit || (uint32_t)(now_ms - enter_emit_ts) >= ENTER_MIN_INTERVAL_MS) {
            enter_consumed = 1;
            enter_has_emit = 1;
            enter_emit_ts  = now_ms;
            return 5;
        }
        /* 在最小间隔内，吞掉本次按下，等待先释放再接收下一次 */
        enter_consumed = 1;
    }

    uint8_t encoder_value = check_encoder_direction();
    if(encoder_value == 1){
        if(keyboard_start_en == 1){
            return 4;
        }else{
            return 1;
        }
    }else if(encoder_value == 2){
        if(keyboard_start_en == 1){
            return 3;
        }else{
            return 2;
        }        
    }
    return 0;
}
#else /*Enable this file at the top*/

/*This dummy typedef exists purely to silence -Wpedantic.*/
typedef int keep_pedantic_happy;
#endif
