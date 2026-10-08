# Copyright 2026 NXP
# NXP Proprietary. This software is owned or controlled by NXP and may only be used strictly in
# accordance with the applicable license terms. By expressly accepting such terms or by downloading, installing,
# activating and/or otherwise using the software, you are agreeing that you have read, and that you agree to
# comply with and are bound by, such license terms.  If you do not agree to be bound by the applicable license
# terms, then you may not retain, install, activate or otherwise use the software.

import SDL
import utime as time
import usys as sys
import lvgl as lv
import lodepng as png
import ustruct
import fs_driver

lv.init()
SDL.init(w=320,h=170)

# Register SDL display driver.
disp_buf1 = lv.disp_draw_buf_t()
buf1_1 = bytearray(320*170*4)
disp_buf1.init(buf1_1, None, len(buf1_1)//4)
disp_drv = lv.disp_drv_t()
disp_drv.init()
disp_drv.draw_buf = disp_buf1
disp_drv.flush_cb = SDL.monitor_flush
disp_drv.hor_res = 320
disp_drv.ver_res = 170
disp_drv.register()

# Regsiter SDL mouse driver
indev_drv = lv.indev_drv_t()
indev_drv.init()
indev_drv.type = lv.INDEV_TYPE.POINTER
indev_drv.read_cb = SDL.mouse_read
indev_drv.register()

fs_drv = lv.fs_drv_t()
fs_driver.fs_register(fs_drv, 'Z')

# Below: Taken from https://github.com/lvgl/lv_binding_micropython/blob/master/driver/js/imagetools.py#L22-L94

COLOR_SIZE = lv.color_t.__SIZE__
COLOR_IS_SWAPPED = hasattr(lv.color_t().ch,'green_h')

class lodepng_error(RuntimeError):
    def __init__(self, err):
        if type(err) is int:
            super().__init__(png.error_text(err))
        else:
            super().__init__(err)

# Parse PNG file header
# Taken from https://github.com/shibukawa/imagesize_py/blob/ffef30c1a4715c5acf90e8945ceb77f4a2ed2d45/imagesize.py#L63-L85

def get_png_info(decoder, src, header):
    # Only handle variable image types

    if lv.img.src_get_type(src) != lv.img.SRC.VARIABLE:
        return lv.RES.INV

    data = lv.img_dsc_t.__cast__(src).data
    if data == None:
        return lv.RES.INV

    png_header = bytes(data.__dereference__(24))

    if png_header.startswith(b'\211PNG\r\n\032\n'):
        if png_header[12:16] == b'IHDR':
            start = 16
        # Maybe this is for an older PNG version.
        else:
            start = 8
        try:
            width, height = ustruct.unpack(">LL", png_header[start:start+8])
        except ustruct.error:
            return lv.RES.INV
    else:
        return lv.RES.INV

    header.always_zero = 0
    header.w = width
    header.h = height
    header.cf = lv.img.CF.TRUE_COLOR_ALPHA

    return lv.RES.OK

def convert_rgba8888_to_bgra8888(img_view):
    for i in range(0, len(img_view), lv.color_t.__SIZE__):
        ch = lv.color_t.__cast__(img_view[i:i]).ch
        ch.red, ch.blue = ch.blue, ch.red

# Read and parse PNG file

def open_png(decoder, dsc):
    img_dsc = lv.img_dsc_t.__cast__(dsc.src)
    png_data = img_dsc.data
    png_size = img_dsc.data_size
    png_decoded = png.C_Pointer()
    png_width = png.C_Pointer()
    png_height = png.C_Pointer()
    error = png.decode32(png_decoded, png_width, png_height, png_data, png_size)
    if error:
        raise lodepng_error(error)
    img_size = png_width.int_val * png_height.int_val * 4
    img_data = png_decoded.ptr_val
    img_view = img_data.__dereference__(img_size)

    if COLOR_SIZE == 4:
        convert_rgba8888_to_bgra8888(img_view)
    else:
        raise lodepng_error("Error: Color mode not supported yet!")

    dsc.img_data = img_data
    return lv.RES.OK

# Above: Taken from https://github.com/lvgl/lv_binding_micropython/blob/master/driver/js/imagetools.py#L22-L94

decoder = lv.img.decoder_create()
decoder.info_cb = get_png_info
decoder.open_cb = open_png

def anim_x_cb(obj, v):
    obj.set_x(v)

def anim_y_cb(obj, v):
    obj.set_y(v)

def anim_width_cb(obj, v):
    obj.set_width(v)

def anim_height_cb(obj, v):
    obj.set_height(v)

def anim_img_zoom_cb(obj, v):
    obj.set_zoom(v)

def anim_img_rotate_cb(obj, v):
    obj.set_angle(v)

global_font_cache = {}
def test_font(font_family, font_size):
    global global_font_cache
    if font_family + str(font_size) in global_font_cache:
        return global_font_cache[font_family + str(font_size)]
    if font_size % 2:
        candidates = [
            (font_family, font_size),
            (font_family, font_size-font_size%2),
            (font_family, font_size+font_size%2),
            ("montserrat", font_size-font_size%2),
            ("montserrat", font_size+font_size%2),
            ("montserrat", 16)
        ]
    else:
        candidates = [
            (font_family, font_size),
            ("montserrat", font_size),
            ("montserrat", 16)
        ]
    for (family, size) in candidates:
        try:
            if eval(f'lv.font_{family}_{size}'):
                global_font_cache[font_family + str(font_size)] = eval(f'lv.font_{family}_{size}')
                if family != font_family or size != font_size:
                    print(f'WARNING: lv.font_{family}_{size} is used!')
                return eval(f'lv.font_{family}_{size}')
        except AttributeError:
            try:
                load_font = lv.font_load(f"Z:MicroPython/lv_font_{family}_{size}.fnt")
                global_font_cache[font_family + str(font_size)] = load_font
                return load_font
            except:
                if family == font_family and size == font_size:
                    print(f'WARNING: lv.font_{family}_{size} is NOT supported!')

global_image_cache = {}
def load_image(file):
    global global_image_cache
    if file in global_image_cache:
        return global_image_cache[file]
    try:
        with open(file,'rb') as f:
            data = f.read()
    except:
        print(f'Could not open {file}')
        sys.exit()

    img = lv.img_dsc_t({
        'data_size': len(data),
        'data': data
    })
    global_image_cache[file] = img
    return img

def calendar_event_handler(e,obj):
    code = e.get_code()

    if code == lv.EVENT.VALUE_CHANGED:
        source = e.get_current_target()
        date = lv.calendar_date_t()
        if source.get_pressed_date(date) == lv.RES.OK:
            source.set_highlighted_dates([date], 1)

def spinbox_increment_event_cb(e, obj):
    code = e.get_code()
    if code == lv.EVENT.SHORT_CLICKED or code == lv.EVENT.LONG_PRESSED_REPEAT:
        obj.increment()
def spinbox_decrement_event_cb(e, obj):
    code = e.get_code()
    if code == lv.EVENT.SHORT_CLICKED or code == lv.EVENT.LONG_PRESSED_REPEAT:
        obj.decrement()

def digital_clock_cb(timer, obj, current_time, show_second, use_ampm):
    hour = int(current_time[0])
    minute = int(current_time[1])
    second = int(current_time[2])
    ampm = current_time[3]
    second = second + 1
    if second == 60:
        second = 0
        minute = minute + 1
        if minute == 60:
            minute = 0
            hour = hour + 1
            if use_ampm:
                if hour == 12:
                    if ampm == 'AM':
                        ampm = 'PM'
                    elif ampm == 'PM':
                        ampm = 'AM'
                if hour > 12:
                    hour = hour % 12
    hour = hour % 24
    if use_ampm:
        if show_second:
            obj.set_text("%d:%02d:%02d %s" %(hour, minute, second, ampm))
        else:
            obj.set_text("%d:%02d %s" %(hour, minute, ampm))
    else:
        if show_second:
            obj.set_text("%d:%02d:%02d" %(hour, minute, second))
        else:
            obj.set_text("%d:%02d" %(hour, minute))
    current_time[0] = hour
    current_time[1] = minute
    current_time[2] = second
    current_time[3] = ampm

def analog_clock_cb(timer, obj):
    datetime = time.localtime()
    hour = datetime[3]
    if hour >= 12: hour = hour - 12
    obj.set_time(hour, datetime[4], datetime[5])

def datetext_event_handler(e, obj):
    code = e.get_code()
    target = e.get_target()
    if code == lv.EVENT.FOCUSED:
        if obj is None:
            bg = lv.layer_top()
            bg.add_flag(lv.obj.FLAG.CLICKABLE)
            obj = lv.calendar(bg)
            scr = target.get_screen()
            scr_height = scr.get_height()
            scr_width = scr.get_width()
            obj.set_size(int(scr_width * 0.8), int(scr_height * 0.8))
            datestring = target.get_text()
            year = int(datestring.split('/')[0])
            month = int(datestring.split('/')[1])
            day = int(datestring.split('/')[2])
            obj.set_showed_date(year, month)
            highlighted_days=[lv.calendar_date_t({'year':year, 'month':month, 'day':day})]
            obj.set_highlighted_dates(highlighted_days, 1)
            obj.align(lv.ALIGN.CENTER, 0, 0)
            lv.calendar_header_arrow(obj)
            obj.add_event_cb(lambda e: datetext_calendar_event_handler(e, target), lv.EVENT.ALL, None)
            scr.update_layout()

def datetext_calendar_event_handler(e, obj):
    code = e.get_code()
    target = e.get_current_target()
    if code == lv.EVENT.VALUE_CHANGED:
        date = lv.calendar_date_t()
        if target.get_pressed_date(date) == lv.RES.OK:
            obj.set_text(f"{date.year}/{date.month}/{date.day}")
            bg = lv.layer_top()
            bg.clear_flag(lv.obj.FLAG.CLICKABLE)
            bg.set_style_bg_opa(lv.OPA.TRANSP, 0)
            target.delete()

def ta_event_cb(e,kb):
    code = e.get_code()
    ta = e.get_target()
    if code == lv.EVENT.FOCUSED:
        kb.set_textarea(ta)
        kb.move_foreground()
        kb.clear_flag(lv.obj.FLAG.HIDDEN)

    if code == lv.EVENT.DEFOCUSED:
        kb.set_textarea(None)
        kb.move_background()
        kb.add_flag(lv.obj.FLAG.HIDDEN)

# Create screen_reppo
screen_reppo = lv.obj()
g_kb_screen_reppo = lv.keyboard(screen_reppo)
g_kb_screen_reppo.add_event_cb(lambda e: ta_event_cb(e, g_kb_screen_reppo), lv.EVENT.ALL, None)
g_kb_screen_reppo.add_flag(lv.obj.FLAG.HIDDEN)
g_kb_screen_reppo.set_style_text_font(test_font("Acme_Regular", 12), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_reppo.set_size(320, 170)
screen_reppo.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
# Set style for screen_reppo, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_reppo.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_reppo.set_style_bg_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_reppo.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_reppo_reppo_gif
screen_reppo_reppo_gif = lv.animimg(screen_reppo)
screen_reppo_reppo_gif_imgs = [None]*24
screen_reppo_reppo_gif_imgs[0] = "B:MicroPython/screen_reppo_reppo_gifAAA_reppo_start_00_320x170.bin"
screen_reppo_reppo_gif_imgs[1] = "B:MicroPython/screen_reppo_reppo_gifAAA_reppo_start_01_320x170.bin"
screen_reppo_reppo_gif_imgs[2] = "B:MicroPython/screen_reppo_reppo_gifAAA_reppo_start_02_320x170.bin"
screen_reppo_reppo_gif_imgs[3] = "B:MicroPython/screen_reppo_reppo_gifAAA_reppo_start_03_320x170.bin"
screen_reppo_reppo_gif_imgs[4] = "B:MicroPython/screen_reppo_reppo_gifAAA_reppo_start_04_320x170.bin"
screen_reppo_reppo_gif_imgs[5] = "B:MicroPython/screen_reppo_reppo_gifAAA_reppo_start_05_320x170.bin"
screen_reppo_reppo_gif_imgs[6] = "B:MicroPython/screen_reppo_reppo_gifAAA_reppo_start_06_320x170.bin"
screen_reppo_reppo_gif_imgs[7] = "B:MicroPython/screen_reppo_reppo_gifAAA_reppo_start_07_320x170.bin"
screen_reppo_reppo_gif_imgs[8] = "B:MicroPython/screen_reppo_reppo_gifAAA_reppo_start_08_320x170.bin"
screen_reppo_reppo_gif_imgs[9] = "B:MicroPython/screen_reppo_reppo_gifAAA_reppo_start_09_320x170.bin"
screen_reppo_reppo_gif_imgs[10] = "B:MicroPython/screen_reppo_reppo_gifAAA_reppo_start_10_320x170.bin"
screen_reppo_reppo_gif_imgs[11] = "B:MicroPython/screen_reppo_reppo_gifAAA_reppo_start_11_320x170.bin"
screen_reppo_reppo_gif_imgs[12] = "B:MicroPython/screen_reppo_reppo_gifAAA_reppo_start_12_320x170.bin"
screen_reppo_reppo_gif_imgs[13] = "B:MicroPython/screen_reppo_reppo_gifAAA_reppo_start_13_320x170.bin"
screen_reppo_reppo_gif_imgs[14] = "B:MicroPython/screen_reppo_reppo_gifAAA_reppo_start_14_320x170.bin"
screen_reppo_reppo_gif_imgs[15] = "B:MicroPython/screen_reppo_reppo_gifAAA_reppo_start_15_320x170.bin"
screen_reppo_reppo_gif_imgs[16] = "B:MicroPython/screen_reppo_reppo_gifAAA_reppo_start_16_320x170.bin"
screen_reppo_reppo_gif_imgs[17] = "B:MicroPython/screen_reppo_reppo_gifAAA_reppo_start_17_320x170.bin"
screen_reppo_reppo_gif_imgs[18] = "B:MicroPython/screen_reppo_reppo_gifAAA_reppo_start_18_320x170.bin"
screen_reppo_reppo_gif_imgs[19] = "B:MicroPython/screen_reppo_reppo_gifAAA_reppo_start_19_320x170.bin"
screen_reppo_reppo_gif_imgs[20] = "B:MicroPython/screen_reppo_reppo_gifAAA_reppo_start_20_320x170.bin"
screen_reppo_reppo_gif_imgs[21] = "B:MicroPython/screen_reppo_reppo_gifAAA_reppo_start_21_320x170.bin"
screen_reppo_reppo_gif_imgs[22] = "B:MicroPython/screen_reppo_reppo_gifAAA_reppo_start_22_320x170.bin"
screen_reppo_reppo_gif_imgs[23] = "B:MicroPython/screen_reppo_reppo_gifAAA_reppo_start_23_320x170.bin"
screen_reppo_reppo_gif.set_src(screen_reppo_reppo_gif_imgs, 24, False)
screen_reppo_reppo_gif.set_duration(50*24)
screen_reppo_reppo_gif.set_repeat_count(1)
screen_reppo_reppo_gif.start()
screen_reppo_reppo_gif.set_pos(0, 0)
screen_reppo_reppo_gif.set_size(320, 170)

screen_reppo.update_layout()
# Create screen_main
screen_main = lv.obj()
g_kb_screen_main = lv.keyboard(screen_main)
g_kb_screen_main.add_event_cb(lambda e: ta_event_cb(e, g_kb_screen_main), lv.EVENT.ALL, None)
g_kb_screen_main.add_flag(lv.obj.FLAG.HIDDEN)
g_kb_screen_main.set_style_text_font(test_font("Acme_Regular", 12), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_main.set_size(320, 170)
screen_main.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
# Set style for screen_main, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_main.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_main.set_style_bg_color(lv.color_hex(0x0e1b28), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_main.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_main.set_style_bg_img_src("B:MicroPython/_AAA_main_bg_320x170.bin", lv.PART.MAIN|lv.STATE.DEFAULT)
screen_main.set_style_bg_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_main.set_style_bg_img_recolor_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_main_cont_main
screen_main_cont_main = lv.obj(screen_main)
screen_main_cont_main.set_pos(0, 28)
screen_main_cont_main.set_size(320, 100)
screen_main_cont_main.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
# Set style for screen_main_cont_main, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_main_cont_main.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_main_cont_main.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_main_cont_main.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_main_cont_main.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_main_cont_main.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_main_cont_main.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_main_cont_main.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_main_cont_main.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
# Create screen_main_btn_todolist
screen_main_btn_todolist = lv.btn(screen_main_cont_main)
screen_main_btn_todolist_label = lv.label(screen_main_btn_todolist)
screen_main_btn_todolist_label.set_text("")
screen_main_btn_todolist_label.set_long_mode(lv.label.LONG.WRAP)
screen_main_btn_todolist_label.set_width(lv.pct(100))
screen_main_btn_todolist_label.align(lv.ALIGN.CENTER, 0, 0)
screen_main_btn_todolist.set_style_pad_all(0, lv.STATE.DEFAULT)
screen_main_btn_todolist.set_pos(5, 32)
screen_main_btn_todolist.set_size(39, 54)
# Set style for screen_main_btn_todolist, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_main_btn_todolist.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_main_btn_todolist.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_main_btn_todolist.set_style_radius(5, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_main_btn_todolist.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_main_btn_todolist.set_style_bg_img_src("B:MicroPython/_AAA_main_todo_0905_39x54.bin", lv.PART.MAIN|lv.STATE.DEFAULT)
screen_main_btn_todolist.set_style_bg_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_main_btn_todolist.set_style_bg_img_recolor_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_main_btn_todolist.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_main_btn_todolist.set_style_text_font(test_font("montserratMedium", 16), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_main_btn_todolist.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_main_btn_todolist.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
# Set style for screen_main_btn_todolist, Part: lv.PART.MAIN, State: lv.STATE.FOCUSED.
screen_main_btn_todolist.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_main_btn_todolist.set_style_border_width(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_main_btn_todolist.set_style_radius(5, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_main_btn_todolist.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_main_btn_todolist.set_style_bg_img_src("B:MicroPython/_AAA_main_todo_0905_39x54.bin", lv.PART.MAIN|lv.STATE.FOCUSED)
screen_main_btn_todolist.set_style_bg_img_opa(255, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_main_btn_todolist.set_style_bg_img_recolor_opa(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_main_btn_todolist.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.FOCUSED)
screen_main_btn_todolist.set_style_text_font(test_font("montserratMedium", 16), lv.PART.MAIN|lv.STATE.FOCUSED)
screen_main_btn_todolist.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.FOCUSED)
# Set style for screen_main_btn_todolist, Part: lv.PART.MAIN, State: lv.STATE.PRESSED.
screen_main_btn_todolist.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.PRESSED)
screen_main_btn_todolist.set_style_border_width(0, lv.PART.MAIN|lv.STATE.PRESSED)
screen_main_btn_todolist.set_style_radius(0, lv.PART.MAIN|lv.STATE.PRESSED)
screen_main_btn_todolist.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.PRESSED)
screen_main_btn_todolist.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.PRESSED)
screen_main_btn_todolist.set_style_text_font(test_font("montserratMedium", 16), lv.PART.MAIN|lv.STATE.PRESSED)
screen_main_btn_todolist.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.PRESSED)
# Set style for screen_main_btn_todolist, Part: lv.PART.MAIN, State: lv.STATE.CHECKED.
screen_main_btn_todolist.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.CHECKED)
screen_main_btn_todolist.set_style_bg_color(lv.color_hex(0x2195f6), lv.PART.MAIN|lv.STATE.CHECKED)
screen_main_btn_todolist.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.CHECKED)
screen_main_btn_todolist.set_style_border_width(0, lv.PART.MAIN|lv.STATE.CHECKED)
screen_main_btn_todolist.set_style_radius(5, lv.PART.MAIN|lv.STATE.CHECKED)
screen_main_btn_todolist.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.CHECKED)
screen_main_btn_todolist.set_style_text_color(lv.color_hex(0x000000), lv.PART.MAIN|lv.STATE.CHECKED)
screen_main_btn_todolist.set_style_text_font(test_font("montserratMedium", 12), lv.PART.MAIN|lv.STATE.CHECKED)
screen_main_btn_todolist.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.CHECKED)
# Set style for screen_main_btn_todolist, Part: lv.PART.MAIN, State: lv.STATE.DISABLED.
screen_main_btn_todolist.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.DISABLED)
screen_main_btn_todolist.set_style_bg_color(lv.color_hex(0x2195f6), lv.PART.MAIN|lv.STATE.DISABLED)
screen_main_btn_todolist.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DISABLED)
screen_main_btn_todolist.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DISABLED)
screen_main_btn_todolist.set_style_radius(5, lv.PART.MAIN|lv.STATE.DISABLED)
screen_main_btn_todolist.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DISABLED)
screen_main_btn_todolist.set_style_text_color(lv.color_hex(0x000000), lv.PART.MAIN|lv.STATE.DISABLED)
screen_main_btn_todolist.set_style_text_font(test_font("montserratMedium", 12), lv.PART.MAIN|lv.STATE.DISABLED)
screen_main_btn_todolist.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DISABLED)

# Create screen_main_btn_done
screen_main_btn_done = lv.btn(screen_main_cont_main)
screen_main_btn_done_label = lv.label(screen_main_btn_done)
screen_main_btn_done_label.set_text("")
screen_main_btn_done_label.set_long_mode(lv.label.LONG.WRAP)
screen_main_btn_done_label.set_width(lv.pct(100))
screen_main_btn_done_label.align(lv.ALIGN.CENTER, 0, 0)
screen_main_btn_done.set_style_pad_all(0, lv.STATE.DEFAULT)
screen_main_btn_done.set_pos(33, 30)
screen_main_btn_done.set_size(45, 54)
# Set style for screen_main_btn_done, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_main_btn_done.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_main_btn_done.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_main_btn_done.set_style_radius(5, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_main_btn_done.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_main_btn_done.set_style_bg_img_src("B:MicroPython/_AAA_main_donelist_0905_45x54.bin", lv.PART.MAIN|lv.STATE.DEFAULT)
screen_main_btn_done.set_style_bg_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_main_btn_done.set_style_bg_img_recolor_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_main_btn_done.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_main_btn_done.set_style_text_font(test_font("montserratMedium", 16), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_main_btn_done.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_main_btn_done.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
# Set style for screen_main_btn_done, Part: lv.PART.MAIN, State: lv.STATE.FOCUSED.
screen_main_btn_done.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_main_btn_done.set_style_border_width(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_main_btn_done.set_style_radius(5, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_main_btn_done.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_main_btn_done.set_style_bg_img_src("B:MicroPython/_AAA_main_donelist_0905_45x54.bin", lv.PART.MAIN|lv.STATE.FOCUSED)
screen_main_btn_done.set_style_bg_img_opa(255, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_main_btn_done.set_style_bg_img_recolor_opa(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_main_btn_done.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.FOCUSED)
screen_main_btn_done.set_style_text_font(test_font("montserratMedium", 16), lv.PART.MAIN|lv.STATE.FOCUSED)
screen_main_btn_done.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.FOCUSED)
# Set style for screen_main_btn_done, Part: lv.PART.MAIN, State: lv.STATE.PRESSED.
screen_main_btn_done.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.PRESSED)
screen_main_btn_done.set_style_border_width(0, lv.PART.MAIN|lv.STATE.PRESSED)
screen_main_btn_done.set_style_radius(5, lv.PART.MAIN|lv.STATE.PRESSED)
screen_main_btn_done.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.PRESSED)
screen_main_btn_done.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.PRESSED)
screen_main_btn_done.set_style_text_font(test_font("montserratMedium", 16), lv.PART.MAIN|lv.STATE.PRESSED)
screen_main_btn_done.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.PRESSED)
# Set style for screen_main_btn_done, Part: lv.PART.MAIN, State: lv.STATE.CHECKED.
screen_main_btn_done.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.CHECKED)
screen_main_btn_done.set_style_bg_color(lv.color_hex(0x2195f6), lv.PART.MAIN|lv.STATE.CHECKED)
screen_main_btn_done.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.CHECKED)
screen_main_btn_done.set_style_border_width(0, lv.PART.MAIN|lv.STATE.CHECKED)
screen_main_btn_done.set_style_radius(5, lv.PART.MAIN|lv.STATE.CHECKED)
screen_main_btn_done.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.CHECKED)
screen_main_btn_done.set_style_text_color(lv.color_hex(0x000000), lv.PART.MAIN|lv.STATE.CHECKED)
screen_main_btn_done.set_style_text_font(test_font("montserratMedium", 12), lv.PART.MAIN|lv.STATE.CHECKED)
screen_main_btn_done.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.CHECKED)
# Set style for screen_main_btn_done, Part: lv.PART.MAIN, State: lv.STATE.DISABLED.
screen_main_btn_done.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.DISABLED)
screen_main_btn_done.set_style_bg_color(lv.color_hex(0x2195f6), lv.PART.MAIN|lv.STATE.DISABLED)
screen_main_btn_done.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DISABLED)
screen_main_btn_done.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DISABLED)
screen_main_btn_done.set_style_radius(5, lv.PART.MAIN|lv.STATE.DISABLED)
screen_main_btn_done.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DISABLED)
screen_main_btn_done.set_style_text_color(lv.color_hex(0x000000), lv.PART.MAIN|lv.STATE.DISABLED)
screen_main_btn_done.set_style_text_font(test_font("montserratMedium", 12), lv.PART.MAIN|lv.STATE.DISABLED)
screen_main_btn_done.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DISABLED)

# Create screen_main_btn_timer
screen_main_btn_timer = lv.btn(screen_main_cont_main)
screen_main_btn_timer_label = lv.label(screen_main_btn_timer)
screen_main_btn_timer_label.set_text("")
screen_main_btn_timer_label.set_long_mode(lv.label.LONG.WRAP)
screen_main_btn_timer_label.set_width(lv.pct(100))
screen_main_btn_timer_label.align(lv.ALIGN.CENTER, 0, 0)
screen_main_btn_timer.set_style_pad_all(0, lv.STATE.DEFAULT)
screen_main_btn_timer.set_pos(66, 26)
screen_main_btn_timer.set_size(43, 55)
# Set style for screen_main_btn_timer, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_main_btn_timer.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_main_btn_timer.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_main_btn_timer.set_style_radius(5, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_main_btn_timer.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_main_btn_timer.set_style_bg_img_src("B:MicroPython/_AAA_main_time_0905_43x55.bin", lv.PART.MAIN|lv.STATE.DEFAULT)
screen_main_btn_timer.set_style_bg_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_main_btn_timer.set_style_bg_img_recolor_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_main_btn_timer.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_main_btn_timer.set_style_text_font(test_font("montserratMedium", 16), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_main_btn_timer.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_main_btn_timer.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
# Set style for screen_main_btn_timer, Part: lv.PART.MAIN, State: lv.STATE.FOCUSED.
screen_main_btn_timer.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_main_btn_timer.set_style_border_width(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_main_btn_timer.set_style_radius(5, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_main_btn_timer.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_main_btn_timer.set_style_bg_img_src("B:MicroPython/_AAA_main_time_0905_43x55.bin", lv.PART.MAIN|lv.STATE.FOCUSED)
screen_main_btn_timer.set_style_bg_img_opa(255, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_main_btn_timer.set_style_bg_img_recolor_opa(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_main_btn_timer.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.FOCUSED)
screen_main_btn_timer.set_style_text_font(test_font("montserratMedium", 16), lv.PART.MAIN|lv.STATE.FOCUSED)
screen_main_btn_timer.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.FOCUSED)

# Create screen_main_btn_ai
screen_main_btn_ai = lv.btn(screen_main_cont_main)
screen_main_btn_ai_label = lv.label(screen_main_btn_ai)
screen_main_btn_ai_label.set_text("")
screen_main_btn_ai_label.set_long_mode(lv.label.LONG.WRAP)
screen_main_btn_ai_label.set_width(lv.pct(100))
screen_main_btn_ai_label.align(lv.ALIGN.CENTER, 0, 0)
screen_main_btn_ai.set_style_pad_all(0, lv.STATE.DEFAULT)
screen_main_btn_ai.set_pos(128, 32)
screen_main_btn_ai.set_size(36, 58)
# Set style for screen_main_btn_ai, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_main_btn_ai.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_main_btn_ai.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_main_btn_ai.set_style_radius(5, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_main_btn_ai.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_main_btn_ai.set_style_bg_img_src("B:MicroPython/_AAA_main_keep_0905_36x58.bin", lv.PART.MAIN|lv.STATE.DEFAULT)
screen_main_btn_ai.set_style_bg_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_main_btn_ai.set_style_bg_img_recolor_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_main_btn_ai.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_main_btn_ai.set_style_text_font(test_font("montserratMedium", 16), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_main_btn_ai.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_main_btn_ai.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
# Set style for screen_main_btn_ai, Part: lv.PART.MAIN, State: lv.STATE.FOCUSED.
screen_main_btn_ai.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_main_btn_ai.set_style_border_width(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_main_btn_ai.set_style_radius(5, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_main_btn_ai.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_main_btn_ai.set_style_bg_img_src("B:MicroPython/_AAA_main_keep_0905_36x58.bin", lv.PART.MAIN|lv.STATE.FOCUSED)
screen_main_btn_ai.set_style_bg_img_opa(255, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_main_btn_ai.set_style_bg_img_recolor_opa(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_main_btn_ai.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.FOCUSED)
screen_main_btn_ai.set_style_text_font(test_font("montserratMedium", 16), lv.PART.MAIN|lv.STATE.FOCUSED)
screen_main_btn_ai.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.FOCUSED)

# Create screen_main_btn_gashapon
screen_main_btn_gashapon = lv.btn(screen_main_cont_main)
screen_main_btn_gashapon_label = lv.label(screen_main_btn_gashapon)
screen_main_btn_gashapon_label.set_text("")
screen_main_btn_gashapon_label.set_long_mode(lv.label.LONG.WRAP)
screen_main_btn_gashapon_label.set_width(lv.pct(100))
screen_main_btn_gashapon_label.align(lv.ALIGN.CENTER, 0, 0)
screen_main_btn_gashapon.set_style_pad_all(0, lv.STATE.DEFAULT)
screen_main_btn_gashapon.set_pos(263, 33)
screen_main_btn_gashapon.set_size(38, 58)
# Set style for screen_main_btn_gashapon, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_main_btn_gashapon.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_main_btn_gashapon.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_main_btn_gashapon.set_style_radius(5, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_main_btn_gashapon.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_main_btn_gashapon.set_style_bg_img_src("B:MicroPython/_AAA_main_egg_0905_38x58.bin", lv.PART.MAIN|lv.STATE.DEFAULT)
screen_main_btn_gashapon.set_style_bg_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_main_btn_gashapon.set_style_bg_img_recolor_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_main_btn_gashapon.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_main_btn_gashapon.set_style_text_font(test_font("montserratMedium", 16), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_main_btn_gashapon.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_main_btn_gashapon.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
# Set style for screen_main_btn_gashapon, Part: lv.PART.MAIN, State: lv.STATE.FOCUSED.
screen_main_btn_gashapon.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_main_btn_gashapon.set_style_border_width(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_main_btn_gashapon.set_style_radius(5, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_main_btn_gashapon.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_main_btn_gashapon.set_style_bg_img_src("B:MicroPython/_AAA_main_egg_0905_38x58.bin", lv.PART.MAIN|lv.STATE.FOCUSED)
screen_main_btn_gashapon.set_style_bg_img_opa(255, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_main_btn_gashapon.set_style_bg_img_recolor_opa(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_main_btn_gashapon.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.FOCUSED)
screen_main_btn_gashapon.set_style_text_font(test_font("montserratMedium", 16), lv.PART.MAIN|lv.STATE.FOCUSED)
screen_main_btn_gashapon.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.FOCUSED)

# Create screen_main_btn_word
screen_main_btn_word = lv.btn(screen_main_cont_main)
screen_main_btn_word_label = lv.label(screen_main_btn_word)
screen_main_btn_word_label.set_text("")
screen_main_btn_word_label.set_long_mode(lv.label.LONG.WRAP)
screen_main_btn_word_label.set_width(lv.pct(100))
screen_main_btn_word_label.align(lv.ALIGN.CENTER, 0, 0)
screen_main_btn_word.set_style_pad_all(0, lv.STATE.DEFAULT)
screen_main_btn_word.set_pos(94, 32)
screen_main_btn_word.set_size(69, 51)
# Set style for screen_main_btn_word, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_main_btn_word.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_main_btn_word.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_main_btn_word.set_style_radius(5, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_main_btn_word.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_main_btn_word.set_style_bg_img_src("B:MicroPython/_AAA_main_book_0905_69x51.bin", lv.PART.MAIN|lv.STATE.DEFAULT)
screen_main_btn_word.set_style_bg_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_main_btn_word.set_style_bg_img_recolor_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_main_btn_word.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_main_btn_word.set_style_text_font(test_font("montserratMedium", 16), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_main_btn_word.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_main_btn_word.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
# Set style for screen_main_btn_word, Part: lv.PART.MAIN, State: lv.STATE.FOCUSED.
screen_main_btn_word.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_main_btn_word.set_style_border_width(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_main_btn_word.set_style_radius(5, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_main_btn_word.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_main_btn_word.set_style_bg_img_src("B:MicroPython/_AAA_main_book_0905_69x51.bin", lv.PART.MAIN|lv.STATE.FOCUSED)
screen_main_btn_word.set_style_bg_img_opa(255, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_main_btn_word.set_style_bg_img_recolor_opa(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_main_btn_word.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.FOCUSED)
screen_main_btn_word.set_style_text_font(test_font("montserratMedium", 16), lv.PART.MAIN|lv.STATE.FOCUSED)
screen_main_btn_word.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.FOCUSED)

# Create screen_main_btn_fun
screen_main_btn_fun = lv.btn(screen_main_cont_main)
screen_main_btn_fun_label = lv.label(screen_main_btn_fun)
screen_main_btn_fun_label.set_text("")
screen_main_btn_fun_label.set_long_mode(lv.label.LONG.WRAP)
screen_main_btn_fun_label.set_width(lv.pct(100))
screen_main_btn_fun_label.align(lv.ALIGN.CENTER, 0, 0)
screen_main_btn_fun.set_style_pad_all(0, lv.STATE.DEFAULT)
screen_main_btn_fun.set_pos(171, 33)
screen_main_btn_fun.set_size(55, 54)
# Set style for screen_main_btn_fun, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_main_btn_fun.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_main_btn_fun.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_main_btn_fun.set_style_radius(5, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_main_btn_fun.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_main_btn_fun.set_style_bg_img_src("B:MicroPython/_AAA_main_innovationbox_0905_55x54.bin", lv.PART.MAIN|lv.STATE.DEFAULT)
screen_main_btn_fun.set_style_bg_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_main_btn_fun.set_style_bg_img_recolor_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_main_btn_fun.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_main_btn_fun.set_style_text_font(test_font("montserratMedium", 16), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_main_btn_fun.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_main_btn_fun.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
# Set style for screen_main_btn_fun, Part: lv.PART.MAIN, State: lv.STATE.FOCUSED.
screen_main_btn_fun.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_main_btn_fun.set_style_border_width(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_main_btn_fun.set_style_radius(5, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_main_btn_fun.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_main_btn_fun.set_style_bg_img_src("B:MicroPython/_AAA_main_innovationbox_0905_55x54.bin", lv.PART.MAIN|lv.STATE.FOCUSED)
screen_main_btn_fun.set_style_bg_img_opa(255, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_main_btn_fun.set_style_bg_img_recolor_opa(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_main_btn_fun.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.FOCUSED)
screen_main_btn_fun.set_style_text_font(test_font("montserratMedium", 16), lv.PART.MAIN|lv.STATE.FOCUSED)
screen_main_btn_fun.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.FOCUSED)

# Create screen_main_btn_photo
screen_main_btn_photo = lv.btn(screen_main_cont_main)
screen_main_btn_photo_label = lv.label(screen_main_btn_photo)
screen_main_btn_photo_label.set_text("")
screen_main_btn_photo_label.set_long_mode(lv.label.LONG.WRAP)
screen_main_btn_photo_label.set_width(lv.pct(100))
screen_main_btn_photo_label.align(lv.ALIGN.CENTER, 0, 0)
screen_main_btn_photo.set_style_pad_all(0, lv.STATE.DEFAULT)
screen_main_btn_photo.set_pos(251, 32)
screen_main_btn_photo.set_size(43, 55)
# Set style for screen_main_btn_photo, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_main_btn_photo.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_main_btn_photo.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_main_btn_photo.set_style_radius(5, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_main_btn_photo.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_main_btn_photo.set_style_bg_img_src("B:MicroPython/_AAA_main_photo_0905_43x55.bin", lv.PART.MAIN|lv.STATE.DEFAULT)
screen_main_btn_photo.set_style_bg_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_main_btn_photo.set_style_bg_img_recolor_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_main_btn_photo.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_main_btn_photo.set_style_text_font(test_font("montserratMedium", 16), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_main_btn_photo.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_main_btn_photo.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
# Set style for screen_main_btn_photo, Part: lv.PART.MAIN, State: lv.STATE.FOCUSED.
screen_main_btn_photo.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_main_btn_photo.set_style_border_width(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_main_btn_photo.set_style_radius(5, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_main_btn_photo.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_main_btn_photo.set_style_bg_img_src("B:MicroPython/_AAA_main_photo_0905_43x55.bin", lv.PART.MAIN|lv.STATE.FOCUSED)
screen_main_btn_photo.set_style_bg_img_opa(255, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_main_btn_photo.set_style_bg_img_recolor_opa(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_main_btn_photo.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.FOCUSED)
screen_main_btn_photo.set_style_text_font(test_font("montserratMedium", 16), lv.PART.MAIN|lv.STATE.FOCUSED)
screen_main_btn_photo.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.FOCUSED)

# Create screen_main_btn_game
screen_main_btn_game = lv.btn(screen_main_cont_main)
screen_main_btn_game_label = lv.label(screen_main_btn_game)
screen_main_btn_game_label.set_text("")
screen_main_btn_game_label.set_long_mode(lv.label.LONG.WRAP)
screen_main_btn_game_label.set_width(lv.pct(100))
screen_main_btn_game_label.align(lv.ALIGN.CENTER, 0, 0)
screen_main_btn_game.set_style_pad_all(0, lv.STATE.DEFAULT)
screen_main_btn_game.set_pos(223, -4)
screen_main_btn_game.set_size(46, 54)
# Set style for screen_main_btn_game, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_main_btn_game.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_main_btn_game.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_main_btn_game.set_style_radius(5, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_main_btn_game.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_main_btn_game.set_style_bg_img_src("B:MicroPython/_AAA_main_play_0905_46x54.bin", lv.PART.MAIN|lv.STATE.DEFAULT)
screen_main_btn_game.set_style_bg_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_main_btn_game.set_style_bg_img_recolor_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_main_btn_game.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_main_btn_game.set_style_text_font(test_font("montserratMedium", 16), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_main_btn_game.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_main_btn_game.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
# Set style for screen_main_btn_game, Part: lv.PART.MAIN, State: lv.STATE.FOCUSED.
screen_main_btn_game.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_main_btn_game.set_style_border_width(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_main_btn_game.set_style_radius(5, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_main_btn_game.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_main_btn_game.set_style_bg_img_src("B:MicroPython/_AAA_main_play_0905_46x54.bin", lv.PART.MAIN|lv.STATE.FOCUSED)
screen_main_btn_game.set_style_bg_img_opa(255, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_main_btn_game.set_style_bg_img_recolor_opa(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_main_btn_game.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.FOCUSED)
screen_main_btn_game.set_style_text_font(test_font("montserratMedium", 16), lv.PART.MAIN|lv.STATE.FOCUSED)
screen_main_btn_game.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.FOCUSED)

# Create screen_main_btn_setting
screen_main_btn_setting = lv.btn(screen_main_cont_main)
screen_main_btn_setting_label = lv.label(screen_main_btn_setting)
screen_main_btn_setting_label.set_text("")
screen_main_btn_setting_label.set_long_mode(lv.label.LONG.WRAP)
screen_main_btn_setting_label.set_width(lv.pct(100))
screen_main_btn_setting_label.align(lv.ALIGN.CENTER, 0, 0)
screen_main_btn_setting.set_style_pad_all(0, lv.STATE.DEFAULT)
screen_main_btn_setting.set_pos(212, 35)
screen_main_btn_setting.set_size(46, 54)
# Set style for screen_main_btn_setting, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_main_btn_setting.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_main_btn_setting.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_main_btn_setting.set_style_radius(5, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_main_btn_setting.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_main_btn_setting.set_style_bg_img_src("B:MicroPython/_AAA_main_setting_0905_46x54.bin", lv.PART.MAIN|lv.STATE.DEFAULT)
screen_main_btn_setting.set_style_bg_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_main_btn_setting.set_style_bg_img_recolor_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_main_btn_setting.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_main_btn_setting.set_style_text_font(test_font("montserratMedium", 16), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_main_btn_setting.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_main_btn_setting.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
# Set style for screen_main_btn_setting, Part: lv.PART.MAIN, State: lv.STATE.FOCUSED.
screen_main_btn_setting.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_main_btn_setting.set_style_border_width(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_main_btn_setting.set_style_radius(5, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_main_btn_setting.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_main_btn_setting.set_style_bg_img_src("B:MicroPython/_AAA_main_setting_0905_46x54.bin", lv.PART.MAIN|lv.STATE.FOCUSED)
screen_main_btn_setting.set_style_bg_img_opa(255, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_main_btn_setting.set_style_bg_img_recolor_opa(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_main_btn_setting.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.FOCUSED)
screen_main_btn_setting.set_style_text_font(test_font("montserratMedium", 16), lv.PART.MAIN|lv.STATE.FOCUSED)
screen_main_btn_setting.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.FOCUSED)

# Create screen_main_label_donenum
screen_main_label_donenum = lv.label(screen_main)
screen_main_label_donenum.set_text("0")
screen_main_label_donenum.set_long_mode(lv.label.LONG.WRAP)
screen_main_label_donenum.set_width(lv.pct(100))
screen_main_label_donenum.set_pos(46, 133)
screen_main_label_donenum.set_size(22, 12)
# Set style for screen_main_label_donenum, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_main_label_donenum.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_main_label_donenum.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_main_label_donenum.set_style_text_color(lv.color_hex(0x2b66a7), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_main_label_donenum.set_style_text_font(test_font("Acme_Regular", 12), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_main_label_donenum.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_main_label_donenum.set_style_text_letter_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_main_label_donenum.set_style_text_line_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_main_label_donenum.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_main_label_donenum.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_main_label_donenum.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_main_label_donenum.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_main_label_donenum.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_main_label_donenum.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_main_label_donenum.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_main_label_todonum
screen_main_label_todonum = lv.label(screen_main)
screen_main_label_todonum.set_text("0")
screen_main_label_todonum.set_long_mode(lv.label.LONG.WRAP)
screen_main_label_todonum.set_width(lv.pct(100))
screen_main_label_todonum.set_pos(46, 151)
screen_main_label_todonum.set_size(22, 12)
# Set style for screen_main_label_todonum, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_main_label_todonum.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_main_label_todonum.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_main_label_todonum.set_style_text_color(lv.color_hex(0x2b66a7), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_main_label_todonum.set_style_text_font(test_font("Acme_Regular", 12), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_main_label_todonum.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_main_label_todonum.set_style_text_letter_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_main_label_todonum.set_style_text_line_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_main_label_todonum.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_main_label_todonum.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_main_label_todonum.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_main_label_todonum.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_main_label_todonum.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_main_label_todonum.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_main_label_todonum.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_main_btn_realtime
screen_main_btn_realtime = lv.btn(screen_main)
screen_main_btn_realtime_label = lv.label(screen_main_btn_realtime)
screen_main_btn_realtime_label.set_text("10:35")
screen_main_btn_realtime_label.set_long_mode(lv.label.LONG.WRAP)
screen_main_btn_realtime_label.set_width(lv.pct(100))
screen_main_btn_realtime_label.align(lv.ALIGN.CENTER, 0, 0)
screen_main_btn_realtime.set_style_pad_all(0, lv.STATE.DEFAULT)
screen_main_btn_realtime.set_pos(272, 145)
screen_main_btn_realtime.set_size(45, 16)
# Set style for screen_main_btn_realtime, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_main_btn_realtime.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_main_btn_realtime.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_main_btn_realtime.set_style_radius(5, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_main_btn_realtime.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_main_btn_realtime.set_style_text_color(lv.color_hex(0x0a2a4c), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_main_btn_realtime.set_style_text_font(test_font("SourceHanSansSC_Medium", 14), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_main_btn_realtime.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_main_btn_realtime.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
# Set style for screen_main_btn_realtime, Part: lv.PART.MAIN, State: lv.STATE.FOCUSED.
screen_main_btn_realtime.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_main_btn_realtime.set_style_border_width(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_main_btn_realtime.set_style_radius(10, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_main_btn_realtime.set_style_shadow_width(11, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_main_btn_realtime.set_style_shadow_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.FOCUSED)
screen_main_btn_realtime.set_style_shadow_opa(255, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_main_btn_realtime.set_style_shadow_spread(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_main_btn_realtime.set_style_shadow_ofs_x(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_main_btn_realtime.set_style_shadow_ofs_y(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_main_btn_realtime.set_style_text_color(lv.color_hex(0x0a2a4c), lv.PART.MAIN|lv.STATE.FOCUSED)
screen_main_btn_realtime.set_style_text_font(test_font("SourceHanSansSC_Medium", 16), lv.PART.MAIN|lv.STATE.FOCUSED)
screen_main_btn_realtime.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.FOCUSED)

# Create screen_main_img_todo_icon
screen_main_img_todo_icon = lv.img(screen_main)
screen_main_img_todo_icon.set_src("B:MicroPython/_AAA_small_todo_alpha_16x14.bin")
screen_main_img_todo_icon.add_flag(lv.obj.FLAG.CLICKABLE)
screen_main_img_todo_icon.set_pivot(50,50)
screen_main_img_todo_icon.set_angle(0)
screen_main_img_todo_icon.set_pos(75, 133)
screen_main_img_todo_icon.set_size(16, 14)
# Set style for screen_main_img_todo_icon, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_main_img_todo_icon.set_style_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_main_img_todo_icon.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_main_img_todo_icon.set_style_clip_corner(True, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_main_img_fun_icon
screen_main_img_fun_icon = lv.img(screen_main)
screen_main_img_fun_icon.set_src("B:MicroPython/_AAA_smallfun_alpha_19x18.bin")
screen_main_img_fun_icon.add_flag(lv.obj.FLAG.CLICKABLE)
screen_main_img_fun_icon.set_pivot(50,50)
screen_main_img_fun_icon.set_angle(0)
screen_main_img_fun_icon.set_pos(185, 131)
screen_main_img_fun_icon.set_size(19, 18)
# Set style for screen_main_img_fun_icon, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_main_img_fun_icon.set_style_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_main_img_fun_icon.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_main_img_fun_icon.set_style_clip_corner(True, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_main_img_setting_icon
screen_main_img_setting_icon = lv.img(screen_main)
screen_main_img_setting_icon.set_src("B:MicroPython/_AAA_small_setting_alpha_15x15.bin")
screen_main_img_setting_icon.add_flag(lv.obj.FLAG.CLICKABLE)
screen_main_img_setting_icon.set_pivot(50,50)
screen_main_img_setting_icon.set_angle(0)
screen_main_img_setting_icon.set_pos(240, 133)
screen_main_img_setting_icon.set_size(15, 15)
# Set style for screen_main_img_setting_icon, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_main_img_setting_icon.set_style_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_main_img_setting_icon.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_main_img_setting_icon.set_style_clip_corner(True, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_main_img_ai_icon
screen_main_img_ai_icon = lv.img(screen_main)
screen_main_img_ai_icon.set_src("B:MicroPython/_AAA_small_keep_0905_alpha_20x16.bin")
screen_main_img_ai_icon.add_flag(lv.obj.FLAG.CLICKABLE)
screen_main_img_ai_icon.set_pivot(50,50)
screen_main_img_ai_icon.set_angle(0)
screen_main_img_ai_icon.set_pos(130, 133)
screen_main_img_ai_icon.set_size(20, 16)
# Set style for screen_main_img_ai_icon, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_main_img_ai_icon.set_style_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_main_img_ai_icon.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_main_img_ai_icon.set_style_clip_corner(True, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_main_img_timer_icon
screen_main_img_timer_icon = lv.img(screen_main)
screen_main_img_timer_icon.set_src("B:MicroPython/_AAA_small_timer_alpha_15x15.bin")
screen_main_img_timer_icon.add_flag(lv.obj.FLAG.CLICKABLE)
screen_main_img_timer_icon.set_pivot(50,50)
screen_main_img_timer_icon.set_angle(0)
screen_main_img_timer_icon.set_pos(113, 133)
screen_main_img_timer_icon.set_size(15, 15)
# Set style for screen_main_img_timer_icon, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_main_img_timer_icon.set_style_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_main_img_timer_icon.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_main_img_timer_icon.set_style_clip_corner(True, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_main_img_book_icon
screen_main_img_book_icon = lv.img(screen_main)
screen_main_img_book_icon.set_src("B:MicroPython/_AAA_small_words_alpha_17x15.bin")
screen_main_img_book_icon.add_flag(lv.obj.FLAG.CLICKABLE)
screen_main_img_book_icon.set_pivot(50,50)
screen_main_img_book_icon.set_angle(0)
screen_main_img_book_icon.set_pos(169, 133)
screen_main_img_book_icon.set_size(17, 15)
# Set style for screen_main_img_book_icon, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_main_img_book_icon.set_style_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_main_img_book_icon.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_main_img_book_icon.set_style_clip_corner(True, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_main_img_done_icon
screen_main_img_done_icon = lv.img(screen_main)
screen_main_img_done_icon.set_src("B:MicroPython/_AAA_small_Done_alpha_15x15.bin")
screen_main_img_done_icon.add_flag(lv.obj.FLAG.CLICKABLE)
screen_main_img_done_icon.set_pivot(50,50)
screen_main_img_done_icon.set_angle(0)
screen_main_img_done_icon.set_pos(93, 133)
screen_main_img_done_icon.set_size(15, 15)
# Set style for screen_main_img_done_icon, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_main_img_done_icon.set_style_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_main_img_done_icon.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_main_img_done_icon.set_style_clip_corner(True, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_main_img_point_icon
screen_main_img_point_icon = lv.img(screen_main)
screen_main_img_point_icon.set_src("B:MicroPython/_AAA_main_dian_alpha_6x6.bin")
screen_main_img_point_icon.add_flag(lv.obj.FLAG.CLICKABLE)
screen_main_img_point_icon.set_pivot(50,50)
screen_main_img_point_icon.set_angle(0)
screen_main_img_point_icon.set_pos(79, 155)
screen_main_img_point_icon.set_size(6, 6)
# Set style for screen_main_img_point_icon, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_main_img_point_icon.set_style_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_main_img_point_icon.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_main_img_point_icon.set_style_clip_corner(True, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_main_label_chosen
screen_main_label_chosen = lv.label(screen_main)
screen_main_label_chosen.set_text("TODO")
screen_main_label_chosen.set_long_mode(lv.label.LONG.WRAP)
screen_main_label_chosen.set_width(lv.pct(100))
screen_main_label_chosen.set_pos(104, 10)
screen_main_label_chosen.set_size(122, 18)
# Set style for screen_main_label_chosen, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_main_label_chosen.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_main_label_chosen.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_main_label_chosen.set_style_text_color(lv.color_hex(0x0a2a4c), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_main_label_chosen.set_style_text_font(test_font("SourceHanSansSC_Medium", 14), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_main_label_chosen.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_main_label_chosen.set_style_text_letter_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_main_label_chosen.set_style_text_line_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_main_label_chosen.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_main_label_chosen.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_main_label_chosen.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_main_label_chosen.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_main_label_chosen.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_main_label_chosen.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_main_label_chosen.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_main_arc_battery
screen_main_arc_battery = lv.arc(screen_main)
screen_main_arc_battery.set_mode(lv.arc.MODE.NORMAL)
screen_main_arc_battery.set_range(0, 100)
screen_main_arc_battery.set_bg_angles(135, 45)
screen_main_arc_battery.set_value(70)
screen_main_arc_battery.set_rotation(0)
screen_main_arc_battery.set_style_arc_rounded(0,  lv.PART.INDICATOR|lv.STATE.DEFAULT)
screen_main_arc_battery.set_style_arc_rounded(0, lv.STATE.DEFAULT)
screen_main_arc_battery.set_pos(261, -6)
screen_main_arc_battery.set_size(73, 54)
# Set style for screen_main_arc_battery, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_main_arc_battery.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_main_arc_battery.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_main_arc_battery.set_style_arc_width(2, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_main_arc_battery.set_style_arc_opa(113, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_main_arc_battery.set_style_arc_color(lv.color_hex(0x8fb0d3), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_main_arc_battery.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_main_arc_battery.set_style_pad_top(15, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_main_arc_battery.set_style_pad_bottom(20, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_main_arc_battery.set_style_pad_left(29, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_main_arc_battery.set_style_pad_right(15, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_main_arc_battery.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
# Set style for screen_main_arc_battery, Part: lv.PART.MAIN, State: lv.STATE.FOCUSED.
screen_main_arc_battery.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_main_arc_battery.set_style_bg_color(lv.color_hex(0xf6f6f6), lv.PART.MAIN|lv.STATE.FOCUSED)
screen_main_arc_battery.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_main_arc_battery.set_style_border_width(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_main_arc_battery.set_style_arc_width(12, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_main_arc_battery.set_style_arc_opa(255, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_main_arc_battery.set_style_arc_color(lv.color_hex(0xe6e6e6), lv.PART.MAIN|lv.STATE.FOCUSED)
screen_main_arc_battery.set_style_radius(6, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_main_arc_battery.set_style_pad_top(20, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_main_arc_battery.set_style_pad_bottom(20, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_main_arc_battery.set_style_pad_left(20, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_main_arc_battery.set_style_pad_right(20, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_main_arc_battery.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.FOCUSED)

# Set style for screen_main_arc_battery, Part: lv.PART.INDICATOR, State: lv.STATE.DEFAULT.
screen_main_arc_battery.set_style_arc_width(2, lv.PART.INDICATOR|lv.STATE.DEFAULT)
screen_main_arc_battery.set_style_arc_opa(255, lv.PART.INDICATOR|lv.STATE.DEFAULT)
screen_main_arc_battery.set_style_arc_color(lv.color_hex(0x0a2a4c), lv.PART.INDICATOR|lv.STATE.DEFAULT)

# Set style for screen_main_arc_battery, Part: lv.PART.KNOB, State: lv.STATE.DEFAULT.
screen_main_arc_battery.set_style_bg_opa(0, lv.PART.KNOB|lv.STATE.DEFAULT)
screen_main_arc_battery.set_style_pad_all(0, lv.PART.KNOB|lv.STATE.DEFAULT)

# Create screen_main_img_select_timing
screen_main_img_select_timing = lv.img(screen_main)
screen_main_img_select_timing.set_src("B:MicroPython/_AAA_select_alpha_46x14.bin")
screen_main_img_select_timing.add_flag(lv.obj.FLAG.CLICKABLE)
screen_main_img_select_timing.set_pivot(50,50)
screen_main_img_select_timing.set_angle(0)
screen_main_img_select_timing.set_pos(269, 148)
screen_main_img_select_timing.set_size(46, 14)
# Set style for screen_main_img_select_timing, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_main_img_select_timing.set_style_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_main_img_select_timing.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_main_img_select_timing.set_style_clip_corner(True, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_main_img_select
screen_main_img_select = lv.img(screen_main)
screen_main_img_select.set_src("B:MicroPython/_AAA_select_alpha_56x23.bin")
screen_main_img_select.add_flag(lv.obj.FLAG.CLICKABLE)
screen_main_img_select.set_pivot(50,50)
screen_main_img_select.set_angle(0)
screen_main_img_select.set_pos(132, 82)
screen_main_img_select.set_size(56, 23)
# Set style for screen_main_img_select, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_main_img_select.set_style_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_main_img_select.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_main_img_select.set_style_clip_corner(True, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_main_img_photo_icon
screen_main_img_photo_icon = lv.img(screen_main)
screen_main_img_photo_icon.set_src("B:MicroPython/_AAA_small_photo_alpha_15x14.bin")
screen_main_img_photo_icon.add_flag(lv.obj.FLAG.CLICKABLE)
screen_main_img_photo_icon.set_pivot(50,50)
screen_main_img_photo_icon.set_angle(0)
screen_main_img_photo_icon.set_pos(205, 133)
screen_main_img_photo_icon.set_size(15, 14)
# Set style for screen_main_img_photo_icon, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_main_img_photo_icon.set_style_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_main_img_photo_icon.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_main_img_photo_icon.set_style_clip_corner(True, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_main_cat_left
screen_main_cat_left = lv.animimg(screen_main)
screen_main_cat_left_imgs = [None]*30
screen_main_cat_left_imgs[0] = "B:MicroPython/screen_main_cat_leftAAA_yuexin_cat_left_00_55x55.bin"
screen_main_cat_left_imgs[1] = "B:MicroPython/screen_main_cat_leftAAA_yuexin_cat_left_01_55x55.bin"
screen_main_cat_left_imgs[2] = "B:MicroPython/screen_main_cat_leftAAA_yuexin_cat_left_02_55x55.bin"
screen_main_cat_left_imgs[3] = "B:MicroPython/screen_main_cat_leftAAA_yuexin_cat_left_03_55x55.bin"
screen_main_cat_left_imgs[4] = "B:MicroPython/screen_main_cat_leftAAA_yuexin_cat_left_04_55x55.bin"
screen_main_cat_left_imgs[5] = "B:MicroPython/screen_main_cat_leftAAA_yuexin_cat_left_05_55x55.bin"
screen_main_cat_left_imgs[6] = "B:MicroPython/screen_main_cat_leftAAA_yuexin_cat_left_06_55x55.bin"
screen_main_cat_left_imgs[7] = "B:MicroPython/screen_main_cat_leftAAA_yuexin_cat_left_07_55x55.bin"
screen_main_cat_left_imgs[8] = "B:MicroPython/screen_main_cat_leftAAA_yuexin_cat_left_08_55x55.bin"
screen_main_cat_left_imgs[9] = "B:MicroPython/screen_main_cat_leftAAA_yuexin_cat_left_09_55x55.bin"
screen_main_cat_left_imgs[10] = "B:MicroPython/screen_main_cat_leftAAA_yuexin_cat_left_10_55x55.bin"
screen_main_cat_left_imgs[11] = "B:MicroPython/screen_main_cat_leftAAA_yuexin_cat_left_11_55x55.bin"
screen_main_cat_left_imgs[12] = "B:MicroPython/screen_main_cat_leftAAA_yuexin_cat_left_12_55x55.bin"
screen_main_cat_left_imgs[13] = "B:MicroPython/screen_main_cat_leftAAA_yuexin_cat_left_13_55x55.bin"
screen_main_cat_left_imgs[14] = "B:MicroPython/screen_main_cat_leftAAA_yuexin_cat_left_14_55x55.bin"
screen_main_cat_left_imgs[15] = "B:MicroPython/screen_main_cat_leftAAA_yuexin_cat_left_15_55x55.bin"
screen_main_cat_left_imgs[16] = "B:MicroPython/screen_main_cat_leftAAA_yuexin_cat_left_16_55x55.bin"
screen_main_cat_left_imgs[17] = "B:MicroPython/screen_main_cat_leftAAA_yuexin_cat_left_17_55x55.bin"
screen_main_cat_left_imgs[18] = "B:MicroPython/screen_main_cat_leftAAA_yuexin_cat_left_18_55x55.bin"
screen_main_cat_left_imgs[19] = "B:MicroPython/screen_main_cat_leftAAA_yuexin_cat_left_19_55x55.bin"
screen_main_cat_left_imgs[20] = "B:MicroPython/screen_main_cat_leftAAA_yuexin_cat_left_20_55x55.bin"
screen_main_cat_left_imgs[21] = "B:MicroPython/screen_main_cat_leftAAA_yuexin_cat_left_21_55x55.bin"
screen_main_cat_left_imgs[22] = "B:MicroPython/screen_main_cat_leftAAA_yuexin_cat_left_22_55x55.bin"
screen_main_cat_left_imgs[23] = "B:MicroPython/screen_main_cat_leftAAA_yuexin_cat_left_23_55x55.bin"
screen_main_cat_left_imgs[24] = "B:MicroPython/screen_main_cat_leftAAA_yuexin_cat_left_24_55x55.bin"
screen_main_cat_left_imgs[25] = "B:MicroPython/screen_main_cat_leftAAA_yuexin_cat_left_25_55x55.bin"
screen_main_cat_left_imgs[26] = "B:MicroPython/screen_main_cat_leftAAA_yuexin_cat_left_26_55x55.bin"
screen_main_cat_left_imgs[27] = "B:MicroPython/screen_main_cat_leftAAA_yuexin_cat_left_27_55x55.bin"
screen_main_cat_left_imgs[28] = "B:MicroPython/screen_main_cat_leftAAA_yuexin_cat_left_28_55x55.bin"
screen_main_cat_left_imgs[29] = "B:MicroPython/screen_main_cat_leftAAA_yuexin_cat_left_29_55x55.bin"
screen_main_cat_left.set_src(screen_main_cat_left_imgs, 30, False)
screen_main_cat_left.set_duration(70*30)
screen_main_cat_left.set_repeat_count(lv.ANIM_REPEAT.INFINITE)
screen_main_cat_left.start()
screen_main_cat_left.set_pos(0, 50)
screen_main_cat_left.set_size(55, 55)
screen_main_cat_left.add_flag(lv.obj.FLAG.HIDDEN)

# Create screen_main_cat_right
screen_main_cat_right = lv.animimg(screen_main)
screen_main_cat_right_imgs = [None]*30
screen_main_cat_right_imgs[0] = "B:MicroPython/screen_main_cat_rightAAA_yuexin_cat_right_00_55x55.bin"
screen_main_cat_right_imgs[1] = "B:MicroPython/screen_main_cat_rightAAA_yuexin_cat_right_01_55x55.bin"
screen_main_cat_right_imgs[2] = "B:MicroPython/screen_main_cat_rightAAA_yuexin_cat_right_02_55x55.bin"
screen_main_cat_right_imgs[3] = "B:MicroPython/screen_main_cat_rightAAA_yuexin_cat_right_03_55x55.bin"
screen_main_cat_right_imgs[4] = "B:MicroPython/screen_main_cat_rightAAA_yuexin_cat_right_04_55x55.bin"
screen_main_cat_right_imgs[5] = "B:MicroPython/screen_main_cat_rightAAA_yuexin_cat_right_05_55x55.bin"
screen_main_cat_right_imgs[6] = "B:MicroPython/screen_main_cat_rightAAA_yuexin_cat_right_06_55x55.bin"
screen_main_cat_right_imgs[7] = "B:MicroPython/screen_main_cat_rightAAA_yuexin_cat_right_07_55x55.bin"
screen_main_cat_right_imgs[8] = "B:MicroPython/screen_main_cat_rightAAA_yuexin_cat_right_08_55x55.bin"
screen_main_cat_right_imgs[9] = "B:MicroPython/screen_main_cat_rightAAA_yuexin_cat_right_09_55x55.bin"
screen_main_cat_right_imgs[10] = "B:MicroPython/screen_main_cat_rightAAA_yuexin_cat_right_10_55x55.bin"
screen_main_cat_right_imgs[11] = "B:MicroPython/screen_main_cat_rightAAA_yuexin_cat_right_11_55x55.bin"
screen_main_cat_right_imgs[12] = "B:MicroPython/screen_main_cat_rightAAA_yuexin_cat_right_12_55x55.bin"
screen_main_cat_right_imgs[13] = "B:MicroPython/screen_main_cat_rightAAA_yuexin_cat_right_13_55x55.bin"
screen_main_cat_right_imgs[14] = "B:MicroPython/screen_main_cat_rightAAA_yuexin_cat_right_14_55x55.bin"
screen_main_cat_right_imgs[15] = "B:MicroPython/screen_main_cat_rightAAA_yuexin_cat_right_15_55x55.bin"
screen_main_cat_right_imgs[16] = "B:MicroPython/screen_main_cat_rightAAA_yuexin_cat_right_16_55x55.bin"
screen_main_cat_right_imgs[17] = "B:MicroPython/screen_main_cat_rightAAA_yuexin_cat_right_17_55x55.bin"
screen_main_cat_right_imgs[18] = "B:MicroPython/screen_main_cat_rightAAA_yuexin_cat_right_18_55x55.bin"
screen_main_cat_right_imgs[19] = "B:MicroPython/screen_main_cat_rightAAA_yuexin_cat_right_19_55x55.bin"
screen_main_cat_right_imgs[20] = "B:MicroPython/screen_main_cat_rightAAA_yuexin_cat_right_20_55x55.bin"
screen_main_cat_right_imgs[21] = "B:MicroPython/screen_main_cat_rightAAA_yuexin_cat_right_21_55x55.bin"
screen_main_cat_right_imgs[22] = "B:MicroPython/screen_main_cat_rightAAA_yuexin_cat_right_22_55x55.bin"
screen_main_cat_right_imgs[23] = "B:MicroPython/screen_main_cat_rightAAA_yuexin_cat_right_23_55x55.bin"
screen_main_cat_right_imgs[24] = "B:MicroPython/screen_main_cat_rightAAA_yuexin_cat_right_24_55x55.bin"
screen_main_cat_right_imgs[25] = "B:MicroPython/screen_main_cat_rightAAA_yuexin_cat_right_25_55x55.bin"
screen_main_cat_right_imgs[26] = "B:MicroPython/screen_main_cat_rightAAA_yuexin_cat_right_26_55x55.bin"
screen_main_cat_right_imgs[27] = "B:MicroPython/screen_main_cat_rightAAA_yuexin_cat_right_27_55x55.bin"
screen_main_cat_right_imgs[28] = "B:MicroPython/screen_main_cat_rightAAA_yuexin_cat_right_28_55x55.bin"
screen_main_cat_right_imgs[29] = "B:MicroPython/screen_main_cat_rightAAA_yuexin_cat_right_29_55x55.bin"
screen_main_cat_right.set_src(screen_main_cat_right_imgs, 30, False)
screen_main_cat_right.set_duration(70*30)
screen_main_cat_right.set_repeat_count(lv.ANIM_REPEAT.INFINITE)
screen_main_cat_right.start()
screen_main_cat_right.set_pos(265, 50)
screen_main_cat_right.set_size(55, 55)
screen_main_cat_right.add_flag(lv.obj.FLAG.HIDDEN)

# Create screen_main_img_gash_icon
screen_main_img_gash_icon = lv.img(screen_main)
screen_main_img_gash_icon.set_src("B:MicroPython/_AAA_small_egg_0905_alpha_16x14.bin")
screen_main_img_gash_icon.add_flag(lv.obj.FLAG.CLICKABLE)
screen_main_img_gash_icon.set_pivot(50,50)
screen_main_img_gash_icon.set_angle(0)
screen_main_img_gash_icon.set_pos(151, 133)
screen_main_img_gash_icon.set_size(16, 14)
# Set style for screen_main_img_gash_icon, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_main_img_gash_icon.set_style_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_main_img_gash_icon.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_main_img_gash_icon.set_style_clip_corner(True, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_main_img_game_icon
screen_main_img_game_icon = lv.img(screen_main)
screen_main_img_game_icon.set_src("B:MicroPython/_AAA_small_game_0905_alpha_16x14.bin")
screen_main_img_game_icon.add_flag(lv.obj.FLAG.CLICKABLE)
screen_main_img_game_icon.set_pivot(50,50)
screen_main_img_game_icon.set_angle(0)
screen_main_img_game_icon.set_pos(223, 133)
screen_main_img_game_icon.set_size(16, 14)
# Set style for screen_main_img_game_icon, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_main_img_game_icon.set_style_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_main_img_game_icon.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_main_img_game_icon.set_style_clip_corner(True, lv.PART.MAIN|lv.STATE.DEFAULT)

screen_main.update_layout()
# Create screen_todolist
screen_todolist = lv.obj()
g_kb_screen_todolist = lv.keyboard(screen_todolist)
g_kb_screen_todolist.add_event_cb(lambda e: ta_event_cb(e, g_kb_screen_todolist), lv.EVENT.ALL, None)
g_kb_screen_todolist.add_flag(lv.obj.FLAG.HIDDEN)
g_kb_screen_todolist.set_style_text_font(test_font("Acme_Regular", 12), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_todolist.set_size(320, 170)
screen_todolist.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
# Set style for screen_todolist, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_todolist.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_todolist.set_style_bg_img_src("B:MicroPython/_AAA_todolist_bg_320x170.bin", lv.PART.MAIN|lv.STATE.DEFAULT)
screen_todolist.set_style_bg_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_todolist.set_style_bg_img_recolor_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_todolist_btn_back
screen_todolist_btn_back = lv.btn(screen_todolist)
screen_todolist_btn_back_label = lv.label(screen_todolist_btn_back)
screen_todolist_btn_back_label.set_text("")
screen_todolist_btn_back_label.set_long_mode(lv.label.LONG.WRAP)
screen_todolist_btn_back_label.set_width(lv.pct(100))
screen_todolist_btn_back_label.align(lv.ALIGN.CENTER, 0, 0)
screen_todolist_btn_back.set_style_pad_all(0, lv.STATE.DEFAULT)
screen_todolist_btn_back.set_pos(296, 12)
screen_todolist_btn_back.set_size(17, 18)
# Set style for screen_todolist_btn_back, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_todolist_btn_back.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_todolist_btn_back.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_todolist_btn_back.set_style_radius(5, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_todolist_btn_back.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_todolist_btn_back.set_style_text_color(lv.color_hex(0x2f6098), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_todolist_btn_back.set_style_text_font(test_font("montserratMedium", 20), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_todolist_btn_back.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_todolist_btn_back.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
# Set style for screen_todolist_btn_back, Part: lv.PART.MAIN, State: lv.STATE.FOCUSED.
screen_todolist_btn_back.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_todolist_btn_back.set_style_border_width(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_todolist_btn_back.set_style_radius(5, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_todolist_btn_back.set_style_shadow_width(5, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_todolist_btn_back.set_style_shadow_color(lv.color_hex(0x274a6e), lv.PART.MAIN|lv.STATE.FOCUSED)
screen_todolist_btn_back.set_style_shadow_opa(255, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_todolist_btn_back.set_style_shadow_spread(1, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_todolist_btn_back.set_style_shadow_ofs_x(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_todolist_btn_back.set_style_shadow_ofs_y(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_todolist_btn_back.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.FOCUSED)
screen_todolist_btn_back.set_style_text_font(test_font("montserratMedium", 20), lv.PART.MAIN|lv.STATE.FOCUSED)
screen_todolist_btn_back.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.FOCUSED)

# Create screen_todolist_btn_todo1
screen_todolist_btn_todo1 = lv.btn(screen_todolist)
screen_todolist_btn_todo1_label = lv.label(screen_todolist_btn_todo1)
screen_todolist_btn_todo1_label.set_text("reppo")
screen_todolist_btn_todo1_label.set_long_mode(lv.label.LONG.CLIP)
screen_todolist_btn_todo1_label.set_width(lv.pct(100))
screen_todolist_btn_todo1_label.align(lv.ALIGN.LEFT_MID, 0, 0)
screen_todolist_btn_todo1.set_style_pad_all(0, lv.STATE.DEFAULT)
screen_todolist_btn_todo1.set_pos(19, 51)
screen_todolist_btn_todo1.set_size(130, 20)
# Set style for screen_todolist_btn_todo1, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_todolist_btn_todo1.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_todolist_btn_todo1.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_todolist_btn_todo1.set_style_radius(5, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_todolist_btn_todo1.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_todolist_btn_todo1.set_style_text_color(lv.color_hex(0x2f6098), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_todolist_btn_todo1.set_style_text_font(test_font("SourceHanSansSC_Medium", 14), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_todolist_btn_todo1.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_todolist_btn_todo1.set_style_text_align(lv.TEXT_ALIGN.LEFT, lv.PART.MAIN|lv.STATE.DEFAULT)
# Set style for screen_todolist_btn_todo1, Part: lv.PART.MAIN, State: lv.STATE.FOCUSED.
screen_todolist_btn_todo1.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_todolist_btn_todo1.set_style_border_width(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_todolist_btn_todo1.set_style_radius(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_todolist_btn_todo1.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_todolist_btn_todo1.set_style_text_color(lv.color_hex(0x022243), lv.PART.MAIN|lv.STATE.FOCUSED)
screen_todolist_btn_todo1.set_style_text_font(test_font("SourceHanSansSC_Medium", 14), lv.PART.MAIN|lv.STATE.FOCUSED)
screen_todolist_btn_todo1.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.FOCUSED)

# Create screen_todolist_btn_todo2
screen_todolist_btn_todo2 = lv.btn(screen_todolist)
screen_todolist_btn_todo2_label = lv.label(screen_todolist_btn_todo2)
screen_todolist_btn_todo2_label.set_text("hello")
screen_todolist_btn_todo2_label.set_long_mode(lv.label.LONG.CLIP)
screen_todolist_btn_todo2_label.set_width(lv.pct(100))
screen_todolist_btn_todo2_label.align(lv.ALIGN.LEFT_MID, 0, 0)
screen_todolist_btn_todo2.set_style_pad_all(0, lv.STATE.DEFAULT)
screen_todolist_btn_todo2.set_pos(177, 51)
screen_todolist_btn_todo2.set_size(130, 20)
# Set style for screen_todolist_btn_todo2, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_todolist_btn_todo2.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_todolist_btn_todo2.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_todolist_btn_todo2.set_style_radius(5, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_todolist_btn_todo2.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_todolist_btn_todo2.set_style_text_color(lv.color_hex(0x2f6098), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_todolist_btn_todo2.set_style_text_font(test_font("SourceHanSansSC_Medium", 14), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_todolist_btn_todo2.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_todolist_btn_todo2.set_style_text_align(lv.TEXT_ALIGN.LEFT, lv.PART.MAIN|lv.STATE.DEFAULT)
# Set style for screen_todolist_btn_todo2, Part: lv.PART.MAIN, State: lv.STATE.FOCUSED.
screen_todolist_btn_todo2.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_todolist_btn_todo2.set_style_border_width(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_todolist_btn_todo2.set_style_radius(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_todolist_btn_todo2.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_todolist_btn_todo2.set_style_text_color(lv.color_hex(0x022243), lv.PART.MAIN|lv.STATE.FOCUSED)
screen_todolist_btn_todo2.set_style_text_font(test_font("SourceHanSansSC_Medium", 14), lv.PART.MAIN|lv.STATE.FOCUSED)
screen_todolist_btn_todo2.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.FOCUSED)

# Create screen_todolist_btn_todo3
screen_todolist_btn_todo3 = lv.btn(screen_todolist)
screen_todolist_btn_todo3_label = lv.label(screen_todolist_btn_todo3)
screen_todolist_btn_todo3_label.set_text("world")
screen_todolist_btn_todo3_label.set_long_mode(lv.label.LONG.CLIP)
screen_todolist_btn_todo3_label.set_width(lv.pct(100))
screen_todolist_btn_todo3_label.align(lv.ALIGN.LEFT_MID, 0, 0)
screen_todolist_btn_todo3.set_style_pad_all(0, lv.STATE.DEFAULT)
screen_todolist_btn_todo3.set_pos(19, 104)
screen_todolist_btn_todo3.set_size(130, 20)
# Set style for screen_todolist_btn_todo3, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_todolist_btn_todo3.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_todolist_btn_todo3.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_todolist_btn_todo3.set_style_radius(5, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_todolist_btn_todo3.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_todolist_btn_todo3.set_style_text_color(lv.color_hex(0x2f6098), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_todolist_btn_todo3.set_style_text_font(test_font("SourceHanSansSC_Medium", 14), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_todolist_btn_todo3.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_todolist_btn_todo3.set_style_text_align(lv.TEXT_ALIGN.LEFT, lv.PART.MAIN|lv.STATE.DEFAULT)
# Set style for screen_todolist_btn_todo3, Part: lv.PART.MAIN, State: lv.STATE.FOCUSED.
screen_todolist_btn_todo3.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_todolist_btn_todo3.set_style_border_width(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_todolist_btn_todo3.set_style_radius(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_todolist_btn_todo3.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_todolist_btn_todo3.set_style_text_color(lv.color_hex(0x022243), lv.PART.MAIN|lv.STATE.FOCUSED)
screen_todolist_btn_todo3.set_style_text_font(test_font("SourceHanSansSC_Medium", 14), lv.PART.MAIN|lv.STATE.FOCUSED)
screen_todolist_btn_todo3.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.FOCUSED)

# Create screen_todolist_btn_todo4
screen_todolist_btn_todo4 = lv.btn(screen_todolist)
screen_todolist_btn_todo4_label = lv.label(screen_todolist_btn_todo4)
screen_todolist_btn_todo4_label.set_text("thanks")
screen_todolist_btn_todo4_label.set_long_mode(lv.label.LONG.CLIP)
screen_todolist_btn_todo4_label.set_width(lv.pct(100))
screen_todolist_btn_todo4_label.align(lv.ALIGN.LEFT_MID, 0, 0)
screen_todolist_btn_todo4.set_style_pad_all(0, lv.STATE.DEFAULT)
screen_todolist_btn_todo4.set_pos(177, 104)
screen_todolist_btn_todo4.set_size(130, 20)
# Set style for screen_todolist_btn_todo4, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_todolist_btn_todo4.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_todolist_btn_todo4.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_todolist_btn_todo4.set_style_radius(5, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_todolist_btn_todo4.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_todolist_btn_todo4.set_style_text_color(lv.color_hex(0x2f6098), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_todolist_btn_todo4.set_style_text_font(test_font("SourceHanSansSC_Medium", 14), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_todolist_btn_todo4.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_todolist_btn_todo4.set_style_text_align(lv.TEXT_ALIGN.LEFT, lv.PART.MAIN|lv.STATE.DEFAULT)
# Set style for screen_todolist_btn_todo4, Part: lv.PART.MAIN, State: lv.STATE.FOCUSED.
screen_todolist_btn_todo4.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_todolist_btn_todo4.set_style_border_width(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_todolist_btn_todo4.set_style_radius(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_todolist_btn_todo4.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_todolist_btn_todo4.set_style_text_color(lv.color_hex(0x022243), lv.PART.MAIN|lv.STATE.FOCUSED)
screen_todolist_btn_todo4.set_style_text_font(test_font("SourceHanSansSC_Medium", 14), lv.PART.MAIN|lv.STATE.FOCUSED)
screen_todolist_btn_todo4.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.FOCUSED)

# Create screen_todolist_label_indexcut
screen_todolist_label_indexcut = lv.label(screen_todolist)
screen_todolist_label_indexcut.set_text("/")
screen_todolist_label_indexcut.set_long_mode(lv.label.LONG.WRAP)
screen_todolist_label_indexcut.set_width(lv.pct(100))
screen_todolist_label_indexcut.set_pos(152, 146)
screen_todolist_label_indexcut.set_size(17, 14)
# Set style for screen_todolist_label_indexcut, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_todolist_label_indexcut.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_todolist_label_indexcut.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_todolist_label_indexcut.set_style_text_color(lv.color_hex(0x274a6e), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_todolist_label_indexcut.set_style_text_font(test_font("SourceHanSansSC_Medium", 14), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_todolist_label_indexcut.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_todolist_label_indexcut.set_style_text_letter_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_todolist_label_indexcut.set_style_text_line_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_todolist_label_indexcut.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_todolist_label_indexcut.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_todolist_label_indexcut.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_todolist_label_indexcut.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_todolist_label_indexcut.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_todolist_label_indexcut.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_todolist_label_indexcut.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_todolist_label_indexcur
screen_todolist_label_indexcur = lv.label(screen_todolist)
screen_todolist_label_indexcur.set_text("1")
screen_todolist_label_indexcur.set_long_mode(lv.label.LONG.WRAP)
screen_todolist_label_indexcur.set_width(lv.pct(100))
screen_todolist_label_indexcur.set_pos(138, 146)
screen_todolist_label_indexcur.set_size(20, 15)
# Set style for screen_todolist_label_indexcur, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_todolist_label_indexcur.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_todolist_label_indexcur.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_todolist_label_indexcur.set_style_text_color(lv.color_hex(0x274a6e), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_todolist_label_indexcur.set_style_text_font(test_font("SourceHanSansSC_Medium", 14), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_todolist_label_indexcur.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_todolist_label_indexcur.set_style_text_letter_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_todolist_label_indexcur.set_style_text_line_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_todolist_label_indexcur.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_todolist_label_indexcur.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_todolist_label_indexcur.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_todolist_label_indexcur.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_todolist_label_indexcur.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_todolist_label_indexcur.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_todolist_label_indexcur.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_todolist_label_indexall
screen_todolist_label_indexall = lv.label(screen_todolist)
screen_todolist_label_indexall.set_text("1")
screen_todolist_label_indexall.set_long_mode(lv.label.LONG.WRAP)
screen_todolist_label_indexall.set_width(lv.pct(100))
screen_todolist_label_indexall.set_pos(163, 146)
screen_todolist_label_indexall.set_size(20, 15)
# Set style for screen_todolist_label_indexall, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_todolist_label_indexall.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_todolist_label_indexall.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_todolist_label_indexall.set_style_text_color(lv.color_hex(0x274a6e), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_todolist_label_indexall.set_style_text_font(test_font("SourceHanSansSC_Medium", 14), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_todolist_label_indexall.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_todolist_label_indexall.set_style_text_letter_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_todolist_label_indexall.set_style_text_line_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_todolist_label_indexall.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_todolist_label_indexall.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_todolist_label_indexall.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_todolist_label_indexall.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_todolist_label_indexall.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_todolist_label_indexall.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_todolist_label_indexall.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_todolist_label_donenum
screen_todolist_label_donenum = lv.label(screen_todolist)
screen_todolist_label_donenum.set_text("0")
screen_todolist_label_donenum.set_long_mode(lv.label.LONG.WRAP)
screen_todolist_label_donenum.set_width(lv.pct(100))
screen_todolist_label_donenum.set_pos(43, 8)
screen_todolist_label_donenum.set_size(25, 15)
# Set style for screen_todolist_label_donenum, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_todolist_label_donenum.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_todolist_label_donenum.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_todolist_label_donenum.set_style_text_color(lv.color_hex(0x274a6e), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_todolist_label_donenum.set_style_text_font(test_font("Acme_Regular", 14), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_todolist_label_donenum.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_todolist_label_donenum.set_style_text_letter_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_todolist_label_donenum.set_style_text_line_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_todolist_label_donenum.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_todolist_label_donenum.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_todolist_label_donenum.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_todolist_label_donenum.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_todolist_label_donenum.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_todolist_label_donenum.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_todolist_label_donenum.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_todolist_label_todonum
screen_todolist_label_todonum = lv.label(screen_todolist)
screen_todolist_label_todonum.set_text("0")
screen_todolist_label_todonum.set_long_mode(lv.label.LONG.WRAP)
screen_todolist_label_todonum.set_width(lv.pct(100))
screen_todolist_label_todonum.set_pos(43, 20)
screen_todolist_label_todonum.set_size(25, 15)
# Set style for screen_todolist_label_todonum, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_todolist_label_todonum.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_todolist_label_todonum.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_todolist_label_todonum.set_style_text_color(lv.color_hex(0x274a6e), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_todolist_label_todonum.set_style_text_font(test_font("Acme_Regular", 14), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_todolist_label_todonum.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_todolist_label_todonum.set_style_text_letter_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_todolist_label_todonum.set_style_text_line_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_todolist_label_todonum.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_todolist_label_todonum.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_todolist_label_todonum.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_todolist_label_todonum.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_todolist_label_todonum.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_todolist_label_todonum.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_todolist_label_todonum.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_todolist_btn_lastpage
screen_todolist_btn_lastpage = lv.btn(screen_todolist)
screen_todolist_btn_lastpage_label = lv.label(screen_todolist_btn_lastpage)
screen_todolist_btn_lastpage_label.set_text("")
screen_todolist_btn_lastpage_label.set_long_mode(lv.label.LONG.WRAP)
screen_todolist_btn_lastpage_label.set_width(lv.pct(100))
screen_todolist_btn_lastpage_label.align(lv.ALIGN.CENTER, 0, 0)
screen_todolist_btn_lastpage.set_style_pad_all(0, lv.STATE.DEFAULT)
screen_todolist_btn_lastpage.set_pos(7, 143)
screen_todolist_btn_lastpage.set_size(95, 19)
# Set style for screen_todolist_btn_lastpage, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_todolist_btn_lastpage.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_todolist_btn_lastpage.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_todolist_btn_lastpage.set_style_radius(5, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_todolist_btn_lastpage.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_todolist_btn_lastpage.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_todolist_btn_lastpage.set_style_text_font(test_font("montserratMedium", 16), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_todolist_btn_lastpage.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_todolist_btn_lastpage.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
# Set style for screen_todolist_btn_lastpage, Part: lv.PART.MAIN, State: lv.STATE.FOCUSED.
screen_todolist_btn_lastpage.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_todolist_btn_lastpage.set_style_border_width(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_todolist_btn_lastpage.set_style_radius(5, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_todolist_btn_lastpage.set_style_shadow_width(6, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_todolist_btn_lastpage.set_style_shadow_color(lv.color_hex(0x274a6e), lv.PART.MAIN|lv.STATE.FOCUSED)
screen_todolist_btn_lastpage.set_style_shadow_opa(255, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_todolist_btn_lastpage.set_style_shadow_spread(1, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_todolist_btn_lastpage.set_style_shadow_ofs_x(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_todolist_btn_lastpage.set_style_shadow_ofs_y(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_todolist_btn_lastpage.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.FOCUSED)
screen_todolist_btn_lastpage.set_style_text_font(test_font("montserratMedium", 16), lv.PART.MAIN|lv.STATE.FOCUSED)
screen_todolist_btn_lastpage.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.FOCUSED)

# Create screen_todolist_btn_nxtpage
screen_todolist_btn_nxtpage = lv.btn(screen_todolist)
screen_todolist_btn_nxtpage_label = lv.label(screen_todolist_btn_nxtpage)
screen_todolist_btn_nxtpage_label.set_text("")
screen_todolist_btn_nxtpage_label.set_long_mode(lv.label.LONG.WRAP)
screen_todolist_btn_nxtpage_label.set_width(lv.pct(100))
screen_todolist_btn_nxtpage_label.align(lv.ALIGN.CENTER, 0, 0)
screen_todolist_btn_nxtpage.set_style_pad_all(0, lv.STATE.DEFAULT)
screen_todolist_btn_nxtpage.set_pos(214, 144)
screen_todolist_btn_nxtpage.set_size(97, 18)
# Set style for screen_todolist_btn_nxtpage, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_todolist_btn_nxtpage.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_todolist_btn_nxtpage.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_todolist_btn_nxtpage.set_style_radius(5, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_todolist_btn_nxtpage.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_todolist_btn_nxtpage.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_todolist_btn_nxtpage.set_style_text_font(test_font("montserratMedium", 16), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_todolist_btn_nxtpage.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_todolist_btn_nxtpage.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
# Set style for screen_todolist_btn_nxtpage, Part: lv.PART.MAIN, State: lv.STATE.FOCUSED.
screen_todolist_btn_nxtpage.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_todolist_btn_nxtpage.set_style_border_width(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_todolist_btn_nxtpage.set_style_radius(5, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_todolist_btn_nxtpage.set_style_shadow_width(6, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_todolist_btn_nxtpage.set_style_shadow_color(lv.color_hex(0x274a6e), lv.PART.MAIN|lv.STATE.FOCUSED)
screen_todolist_btn_nxtpage.set_style_shadow_opa(255, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_todolist_btn_nxtpage.set_style_shadow_spread(1, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_todolist_btn_nxtpage.set_style_shadow_ofs_x(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_todolist_btn_nxtpage.set_style_shadow_ofs_y(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_todolist_btn_nxtpage.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.FOCUSED)
screen_todolist_btn_nxtpage.set_style_text_font(test_font("montserratMedium", 16), lv.PART.MAIN|lv.STATE.FOCUSED)
screen_todolist_btn_nxtpage.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.FOCUSED)

# Create screen_todolist_line_1
screen_todolist_line_1 = lv.line(screen_todolist)
screen_todolist_line_1_line_points = [{"x":0, "y":0},{"x":300, "y":0},]
screen_todolist_line_1.set_points(screen_todolist_line_1_line_points, 2)
screen_todolist_line_1.set_pos(16, 62)
screen_todolist_line_1.set_size(129, 1)
# Set style for screen_todolist_line_1, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_todolist_line_1.set_style_line_width(1, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_todolist_line_1.set_style_line_color(lv.color_hex(0x7698bc), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_todolist_line_1.set_style_line_opa(169, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_todolist_line_2
screen_todolist_line_2 = lv.line(screen_todolist)
screen_todolist_line_2_line_points = [{"x":0, "y":0},{"x":300, "y":0},]
screen_todolist_line_2.set_points(screen_todolist_line_2_line_points, 2)
screen_todolist_line_2.set_pos(173, 62)
screen_todolist_line_2.set_size(127, 1)
# Set style for screen_todolist_line_2, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_todolist_line_2.set_style_line_width(1, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_todolist_line_2.set_style_line_color(lv.color_hex(0x7698bc), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_todolist_line_2.set_style_line_opa(169, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_todolist_line_3
screen_todolist_line_3 = lv.line(screen_todolist)
screen_todolist_line_3_line_points = [{"x":0, "y":0},{"x":300, "y":0},]
screen_todolist_line_3.set_points(screen_todolist_line_3_line_points, 2)
screen_todolist_line_3.set_pos(16, 115)
screen_todolist_line_3.set_size(129, 1)
# Set style for screen_todolist_line_3, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_todolist_line_3.set_style_line_width(1, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_todolist_line_3.set_style_line_color(lv.color_hex(0x7698bc), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_todolist_line_3.set_style_line_opa(169, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_todolist_line_4
screen_todolist_line_4 = lv.line(screen_todolist)
screen_todolist_line_4_line_points = [{"x":0, "y":0},{"x":300, "y":0},]
screen_todolist_line_4.set_points(screen_todolist_line_4_line_points, 2)
screen_todolist_line_4.set_pos(173, 115)
screen_todolist_line_4.set_size(127, 1)
# Set style for screen_todolist_line_4, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_todolist_line_4.set_style_line_width(1, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_todolist_line_4.set_style_line_color(lv.color_hex(0x7698bc), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_todolist_line_4.set_style_line_opa(176, lv.PART.MAIN|lv.STATE.DEFAULT)

screen_todolist.update_layout()
# Create screen_todosel
screen_todosel = lv.obj()
g_kb_screen_todosel = lv.keyboard(screen_todosel)
g_kb_screen_todosel.add_event_cb(lambda e: ta_event_cb(e, g_kb_screen_todosel), lv.EVENT.ALL, None)
g_kb_screen_todosel.add_flag(lv.obj.FLAG.HIDDEN)
g_kb_screen_todosel.set_style_text_font(test_font("Acme_Regular", 12), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_todosel.set_size(320, 170)
screen_todosel.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
# Set style for screen_todosel, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_todosel.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_todosel.set_style_bg_img_src("B:MicroPython/_AAA_todosel_bg_320x170.bin", lv.PART.MAIN|lv.STATE.DEFAULT)
screen_todosel.set_style_bg_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_todosel.set_style_bg_img_recolor_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_todosel_btn_complete
screen_todosel_btn_complete = lv.btn(screen_todosel)
screen_todosel_btn_complete_label = lv.label(screen_todosel_btn_complete)
screen_todosel_btn_complete_label.set_text("")
screen_todosel_btn_complete_label.set_long_mode(lv.label.LONG.WRAP)
screen_todosel_btn_complete_label.set_width(lv.pct(100))
screen_todosel_btn_complete_label.align(lv.ALIGN.CENTER, 0, 0)
screen_todosel_btn_complete.set_style_pad_all(0, lv.STATE.DEFAULT)
screen_todosel_btn_complete.set_pos(18, 85)
screen_todosel_btn_complete.set_size(137, 28)
# Set style for screen_todosel_btn_complete, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_todosel_btn_complete.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_todosel_btn_complete.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_todosel_btn_complete.set_style_radius(5, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_todosel_btn_complete.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_todosel_btn_complete.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_todosel_btn_complete.set_style_text_font(test_font("montserratMedium", 16), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_todosel_btn_complete.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_todosel_btn_complete.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
# Set style for screen_todosel_btn_complete, Part: lv.PART.MAIN, State: lv.STATE.FOCUSED.
screen_todosel_btn_complete.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_todosel_btn_complete.set_style_border_width(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_todosel_btn_complete.set_style_radius(5, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_todosel_btn_complete.set_style_shadow_width(10, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_todosel_btn_complete.set_style_shadow_color(lv.color_hex(0x274a6e), lv.PART.MAIN|lv.STATE.FOCUSED)
screen_todosel_btn_complete.set_style_shadow_opa(255, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_todosel_btn_complete.set_style_shadow_spread(1, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_todosel_btn_complete.set_style_shadow_ofs_x(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_todosel_btn_complete.set_style_shadow_ofs_y(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_todosel_btn_complete.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.FOCUSED)
screen_todosel_btn_complete.set_style_text_font(test_font("montserratMedium", 16), lv.PART.MAIN|lv.STATE.FOCUSED)
screen_todosel_btn_complete.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.FOCUSED)

# Create screen_todosel_btn_focus
screen_todosel_btn_focus = lv.btn(screen_todosel)
screen_todosel_btn_focus_label = lv.label(screen_todosel_btn_focus)
screen_todosel_btn_focus_label.set_text("")
screen_todosel_btn_focus_label.set_long_mode(lv.label.LONG.WRAP)
screen_todosel_btn_focus_label.set_width(lv.pct(100))
screen_todosel_btn_focus_label.align(lv.ALIGN.CENTER, 0, 0)
screen_todosel_btn_focus.set_style_pad_all(0, lv.STATE.DEFAULT)
screen_todosel_btn_focus.set_pos(162, 86)
screen_todosel_btn_focus.set_size(138, 26)
# Set style for screen_todosel_btn_focus, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_todosel_btn_focus.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_todosel_btn_focus.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_todosel_btn_focus.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_todosel_btn_focus.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_todosel_btn_focus.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_todosel_btn_focus.set_style_text_font(test_font("montserratMedium", 16), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_todosel_btn_focus.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_todosel_btn_focus.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
# Set style for screen_todosel_btn_focus, Part: lv.PART.MAIN, State: lv.STATE.FOCUSED.
screen_todosel_btn_focus.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_todosel_btn_focus.set_style_border_width(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_todosel_btn_focus.set_style_radius(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_todosel_btn_focus.set_style_shadow_width(10, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_todosel_btn_focus.set_style_shadow_color(lv.color_hex(0x274a6e), lv.PART.MAIN|lv.STATE.FOCUSED)
screen_todosel_btn_focus.set_style_shadow_opa(255, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_todosel_btn_focus.set_style_shadow_spread(1, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_todosel_btn_focus.set_style_shadow_ofs_x(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_todosel_btn_focus.set_style_shadow_ofs_y(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_todosel_btn_focus.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.FOCUSED)
screen_todosel_btn_focus.set_style_text_font(test_font("montserratMedium", 16), lv.PART.MAIN|lv.STATE.FOCUSED)
screen_todosel_btn_focus.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.FOCUSED)

# Create screen_todosel_btn_cancel
screen_todosel_btn_cancel = lv.btn(screen_todosel)
screen_todosel_btn_cancel_label = lv.label(screen_todosel_btn_cancel)
screen_todosel_btn_cancel_label.set_text("")
screen_todosel_btn_cancel_label.set_long_mode(lv.label.LONG.WRAP)
screen_todosel_btn_cancel_label.set_width(lv.pct(100))
screen_todosel_btn_cancel_label.align(lv.ALIGN.CENTER, 0, 0)
screen_todosel_btn_cancel.set_style_pad_all(0, lv.STATE.DEFAULT)
screen_todosel_btn_cancel.set_pos(137, 128)
screen_todosel_btn_cancel.set_size(43, 15)
# Set style for screen_todosel_btn_cancel, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_todosel_btn_cancel.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_todosel_btn_cancel.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_todosel_btn_cancel.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_todosel_btn_cancel.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_todosel_btn_cancel.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_todosel_btn_cancel.set_style_text_font(test_font("montserratMedium", 16), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_todosel_btn_cancel.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_todosel_btn_cancel.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
# Set style for screen_todosel_btn_cancel, Part: lv.PART.MAIN, State: lv.STATE.FOCUSED.
screen_todosel_btn_cancel.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_todosel_btn_cancel.set_style_border_width(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_todosel_btn_cancel.set_style_radius(5, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_todosel_btn_cancel.set_style_shadow_width(10, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_todosel_btn_cancel.set_style_shadow_color(lv.color_hex(0x274a6e), lv.PART.MAIN|lv.STATE.FOCUSED)
screen_todosel_btn_cancel.set_style_shadow_opa(255, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_todosel_btn_cancel.set_style_shadow_spread(1, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_todosel_btn_cancel.set_style_shadow_ofs_x(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_todosel_btn_cancel.set_style_shadow_ofs_y(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_todosel_btn_cancel.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.FOCUSED)
screen_todosel_btn_cancel.set_style_text_font(test_font("montserratMedium", 16), lv.PART.MAIN|lv.STATE.FOCUSED)
screen_todosel_btn_cancel.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.FOCUSED)

# Create screen_todosel_label_task
screen_todosel_label_task = lv.label(screen_todosel)
screen_todosel_label_task.set_text("Reading for one hour")
screen_todosel_label_task.set_long_mode(lv.label.LONG.SCROLL)
screen_todosel_label_task.set_width(lv.pct(100))
screen_todosel_label_task.set_pos(84, 59)
screen_todosel_label_task.set_size(150, 16)
# Set style for screen_todosel_label_task, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_todosel_label_task.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_todosel_label_task.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_todosel_label_task.set_style_text_color(lv.color_hex(0x274a6e), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_todosel_label_task.set_style_text_font(test_font("SourceHanSansSC_Medium", 14), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_todosel_label_task.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_todosel_label_task.set_style_text_letter_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_todosel_label_task.set_style_text_line_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_todosel_label_task.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_todosel_label_task.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_todosel_label_task.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_todosel_label_task.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_todosel_label_task.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_todosel_label_task.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_todosel_label_task.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

screen_todosel.update_layout()
# Create screen_todoback
screen_todoback = lv.obj()
g_kb_screen_todoback = lv.keyboard(screen_todoback)
g_kb_screen_todoback.add_event_cb(lambda e: ta_event_cb(e, g_kb_screen_todoback), lv.EVENT.ALL, None)
g_kb_screen_todoback.add_flag(lv.obj.FLAG.HIDDEN)
g_kb_screen_todoback.set_style_text_font(test_font("Acme_Regular", 12), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_todoback.set_size(320, 170)
screen_todoback.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
# Set style for screen_todoback, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_todoback.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_todoback.set_style_bg_img_src("B:MicroPython/_AAA_todo_fanqie_bg2_320x170.bin", lv.PART.MAIN|lv.STATE.DEFAULT)
screen_todoback.set_style_bg_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_todoback.set_style_bg_img_recolor_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_todoback_label_name
screen_todoback_label_name = lv.label(screen_todoback)
screen_todoback_label_name.set_text("Reading for one hour")
screen_todoback_label_name.set_long_mode(lv.label.LONG.SCROLL)
screen_todoback_label_name.set_width(lv.pct(100))
screen_todoback_label_name.set_pos(84, 67)
screen_todoback_label_name.set_size(150, 16)
# Set style for screen_todoback_label_name, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_todoback_label_name.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_todoback_label_name.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_todoback_label_name.set_style_text_color(lv.color_hex(0x000000), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_todoback_label_name.set_style_text_font(test_font("SourceHanSansSC_Medium", 14), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_todoback_label_name.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_todoback_label_name.set_style_text_letter_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_todoback_label_name.set_style_text_line_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_todoback_label_name.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_todoback_label_name.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_todoback_label_name.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_todoback_label_name.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_todoback_label_name.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_todoback_label_name.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_todoback_label_name.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_todoback_btn_cancel
screen_todoback_btn_cancel = lv.btn(screen_todoback)
screen_todoback_btn_cancel_label = lv.label(screen_todoback_btn_cancel)
screen_todoback_btn_cancel_label.set_text("")
screen_todoback_btn_cancel_label.set_long_mode(lv.label.LONG.WRAP)
screen_todoback_btn_cancel_label.set_width(lv.pct(100))
screen_todoback_btn_cancel_label.align(lv.ALIGN.CENTER, 0, 0)
screen_todoback_btn_cancel.set_style_pad_all(0, lv.STATE.DEFAULT)
screen_todoback_btn_cancel.set_pos(165, 102)
screen_todoback_btn_cancel.set_size(127, 23)
# Set style for screen_todoback_btn_cancel, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_todoback_btn_cancel.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_todoback_btn_cancel.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_todoback_btn_cancel.set_style_radius(5, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_todoback_btn_cancel.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_todoback_btn_cancel.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_todoback_btn_cancel.set_style_text_font(test_font("montserratMedium", 16), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_todoback_btn_cancel.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_todoback_btn_cancel.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
# Set style for screen_todoback_btn_cancel, Part: lv.PART.MAIN, State: lv.STATE.FOCUSED.
screen_todoback_btn_cancel.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_todoback_btn_cancel.set_style_border_width(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_todoback_btn_cancel.set_style_radius(5, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_todoback_btn_cancel.set_style_shadow_width(10, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_todoback_btn_cancel.set_style_shadow_color(lv.color_hex(0x274a6e), lv.PART.MAIN|lv.STATE.FOCUSED)
screen_todoback_btn_cancel.set_style_shadow_opa(255, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_todoback_btn_cancel.set_style_shadow_spread(1, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_todoback_btn_cancel.set_style_shadow_ofs_x(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_todoback_btn_cancel.set_style_shadow_ofs_y(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_todoback_btn_cancel.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.FOCUSED)
screen_todoback_btn_cancel.set_style_text_font(test_font("montserratMedium", 16), lv.PART.MAIN|lv.STATE.FOCUSED)
screen_todoback_btn_cancel.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.FOCUSED)

# Create screen_todoback_btn_recover
screen_todoback_btn_recover = lv.btn(screen_todoback)
screen_todoback_btn_recover_label = lv.label(screen_todoback_btn_recover)
screen_todoback_btn_recover_label.set_text("")
screen_todoback_btn_recover_label.set_long_mode(lv.label.LONG.WRAP)
screen_todoback_btn_recover_label.set_width(lv.pct(100))
screen_todoback_btn_recover_label.align(lv.ALIGN.CENTER, 0, 0)
screen_todoback_btn_recover.set_style_pad_all(0, lv.STATE.DEFAULT)
screen_todoback_btn_recover.set_pos(20, 100)
screen_todoback_btn_recover.set_size(136, 26)
# Set style for screen_todoback_btn_recover, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_todoback_btn_recover.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_todoback_btn_recover.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_todoback_btn_recover.set_style_radius(5, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_todoback_btn_recover.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_todoback_btn_recover.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_todoback_btn_recover.set_style_text_font(test_font("montserratMedium", 16), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_todoback_btn_recover.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_todoback_btn_recover.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
# Set style for screen_todoback_btn_recover, Part: lv.PART.MAIN, State: lv.STATE.FOCUSED.
screen_todoback_btn_recover.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_todoback_btn_recover.set_style_border_width(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_todoback_btn_recover.set_style_radius(5, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_todoback_btn_recover.set_style_shadow_width(10, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_todoback_btn_recover.set_style_shadow_color(lv.color_hex(0x274a6e), lv.PART.MAIN|lv.STATE.FOCUSED)
screen_todoback_btn_recover.set_style_shadow_opa(255, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_todoback_btn_recover.set_style_shadow_spread(1, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_todoback_btn_recover.set_style_shadow_ofs_x(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_todoback_btn_recover.set_style_shadow_ofs_y(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_todoback_btn_recover.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.FOCUSED)
screen_todoback_btn_recover.set_style_text_font(test_font("SourceHanSansSC_Medium", 16), lv.PART.MAIN|lv.STATE.FOCUSED)
screen_todoback_btn_recover.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.FOCUSED)

screen_todoback.update_layout()
# Create screen_wordcard
screen_wordcard = lv.obj()
g_kb_screen_wordcard = lv.keyboard(screen_wordcard)
g_kb_screen_wordcard.add_event_cb(lambda e: ta_event_cb(e, g_kb_screen_wordcard), lv.EVENT.ALL, None)
g_kb_screen_wordcard.add_flag(lv.obj.FLAG.HIDDEN)
g_kb_screen_wordcard.set_style_text_font(test_font("Acme_Regular", 12), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wordcard.set_size(320, 170)
screen_wordcard.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
# Set style for screen_wordcard, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_wordcard.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wordcard.set_style_bg_img_src("B:MicroPython/_AAA_flashcards_learn_bg_320x170.bin", lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wordcard.set_style_bg_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wordcard.set_style_bg_img_recolor_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_wordcard_label_index
screen_wordcard_label_index = lv.label(screen_wordcard)
screen_wordcard_label_index.set_text("0/50")
screen_wordcard_label_index.set_long_mode(lv.label.LONG.WRAP)
screen_wordcard_label_index.set_width(lv.pct(100))
screen_wordcard_label_index.set_pos(222, 10)
screen_wordcard_label_index.set_size(69, 14)
# Set style for screen_wordcard_label_index, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_wordcard_label_index.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wordcard_label_index.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wordcard_label_index.set_style_text_color(lv.color_hex(0x274a61), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wordcard_label_index.set_style_text_font(test_font("SourceHanSansSC_Medium", 14), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wordcard_label_index.set_style_text_opa(207, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wordcard_label_index.set_style_text_letter_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wordcard_label_index.set_style_text_line_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wordcard_label_index.set_style_text_align(lv.TEXT_ALIGN.RIGHT, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wordcard_label_index.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wordcard_label_index.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wordcard_label_index.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wordcard_label_index.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wordcard_label_index.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wordcard_label_index.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_wordcard_label_phonetic
screen_wordcard_label_phonetic = lv.label(screen_wordcard)
screen_wordcard_label_phonetic.set_text("ɑ ɒ ɔ ə ɛ ɜ ɪ ʊ ʌ æ e ɪ a ɪ ɔ ɪ a ʊ ə ʊ ɪ ə e ə ʊ ə p t k f θ s ʃ h tʃ ʧ tr ts b d v ð z ʒ r dʒ ʤ dr dz m n ŋ l j w ˈ ˌ ː . [ ] / ( ) ɡ ，。ɚ ɝ̃ “”” ” -")
screen_wordcard_label_phonetic.set_long_mode(lv.label.LONG.CLIP)
screen_wordcard_label_phonetic.set_width(lv.pct(100))
screen_wordcard_label_phonetic.set_pos(32, 69)
screen_wordcard_label_phonetic.set_size(255, 16)
# Set style for screen_wordcard_label_phonetic, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_wordcard_label_phonetic.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wordcard_label_phonetic.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wordcard_label_phonetic.set_style_text_color(lv.color_hex(0x274a61), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wordcard_label_phonetic.set_style_text_font(test_font("arial", 14), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wordcard_label_phonetic.set_style_text_opa(143, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wordcard_label_phonetic.set_style_text_letter_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wordcard_label_phonetic.set_style_text_line_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wordcard_label_phonetic.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wordcard_label_phonetic.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wordcard_label_phonetic.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wordcard_label_phonetic.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wordcard_label_phonetic.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wordcard_label_phonetic.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wordcard_label_phonetic.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_wordcard_label_meaning
screen_wordcard_label_meaning = lv.label(screen_wordcard)
screen_wordcard_label_meaning.set_text("v.获得;学到")
screen_wordcard_label_meaning.set_long_mode(lv.label.LONG.CLIP)
screen_wordcard_label_meaning.set_width(lv.pct(100))
screen_wordcard_label_meaning.set_pos(32, 91)
screen_wordcard_label_meaning.set_size(255, 18)
# Set style for screen_wordcard_label_meaning, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_wordcard_label_meaning.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wordcard_label_meaning.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wordcard_label_meaning.set_style_text_color(lv.color_hex(0x274a61), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wordcard_label_meaning.set_style_text_font(test_font("SourceHanSansSC_Medium", 14), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wordcard_label_meaning.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wordcard_label_meaning.set_style_text_letter_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wordcard_label_meaning.set_style_text_line_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wordcard_label_meaning.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wordcard_label_meaning.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wordcard_label_meaning.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wordcard_label_meaning.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wordcard_label_meaning.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wordcard_label_meaning.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wordcard_label_meaning.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_wordcard_btn_dui
screen_wordcard_btn_dui = lv.btn(screen_wordcard)
screen_wordcard_btn_dui_label = lv.label(screen_wordcard_btn_dui)
screen_wordcard_btn_dui_label.set_text("")
screen_wordcard_btn_dui_label.set_long_mode(lv.label.LONG.WRAP)
screen_wordcard_btn_dui_label.set_width(lv.pct(100))
screen_wordcard_btn_dui_label.align(lv.ALIGN.CENTER, 0, 0)
screen_wordcard_btn_dui.set_style_pad_all(0, lv.STATE.DEFAULT)
screen_wordcard_btn_dui.set_pos(52, 123)
screen_wordcard_btn_dui.set_size(20, 20)
# Set style for screen_wordcard_btn_dui, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_wordcard_btn_dui.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wordcard_btn_dui.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wordcard_btn_dui.set_style_radius(20, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wordcard_btn_dui.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wordcard_btn_dui.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wordcard_btn_dui.set_style_text_font(test_font("montserratMedium", 16), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wordcard_btn_dui.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wordcard_btn_dui.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
# Set style for screen_wordcard_btn_dui, Part: lv.PART.MAIN, State: lv.STATE.FOCUSED.
screen_wordcard_btn_dui.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_wordcard_btn_dui.set_style_border_width(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_wordcard_btn_dui.set_style_radius(20, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_wordcard_btn_dui.set_style_shadow_width(10, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_wordcard_btn_dui.set_style_shadow_color(lv.color_hex(0x274a6e), lv.PART.MAIN|lv.STATE.FOCUSED)
screen_wordcard_btn_dui.set_style_shadow_opa(255, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_wordcard_btn_dui.set_style_shadow_spread(1, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_wordcard_btn_dui.set_style_shadow_ofs_x(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_wordcard_btn_dui.set_style_shadow_ofs_y(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_wordcard_btn_dui.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.FOCUSED)
screen_wordcard_btn_dui.set_style_text_font(test_font("montserratMedium", 16), lv.PART.MAIN|lv.STATE.FOCUSED)
screen_wordcard_btn_dui.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.FOCUSED)

# Set style for screen_wordcard_btn_dui, Part: lv.PART.MAIN, State: lv.STATE.DISABLED.
screen_wordcard_btn_dui.set_style_bg_opa(169, lv.PART.MAIN|lv.STATE.DISABLED)
screen_wordcard_btn_dui.set_style_bg_color(lv.color_hex(0x7b7b7b), lv.PART.MAIN|lv.STATE.DISABLED)
screen_wordcard_btn_dui.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DISABLED)
screen_wordcard_btn_dui.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DISABLED)
screen_wordcard_btn_dui.set_style_radius(20, lv.PART.MAIN|lv.STATE.DISABLED)
screen_wordcard_btn_dui.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DISABLED)
screen_wordcard_btn_dui.set_style_text_color(lv.color_hex(0x000000), lv.PART.MAIN|lv.STATE.DISABLED)
screen_wordcard_btn_dui.set_style_text_font(test_font("montserratMedium", 12), lv.PART.MAIN|lv.STATE.DISABLED)
screen_wordcard_btn_dui.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DISABLED)

# Create screen_wordcard_btn_cuo
screen_wordcard_btn_cuo = lv.btn(screen_wordcard)
screen_wordcard_btn_cuo_label = lv.label(screen_wordcard_btn_cuo)
screen_wordcard_btn_cuo_label.set_text("")
screen_wordcard_btn_cuo_label.set_long_mode(lv.label.LONG.WRAP)
screen_wordcard_btn_cuo_label.set_width(lv.pct(100))
screen_wordcard_btn_cuo_label.align(lv.ALIGN.CENTER, 0, 0)
screen_wordcard_btn_cuo.set_style_pad_all(0, lv.STATE.DEFAULT)
screen_wordcard_btn_cuo.set_pos(248, 123)
screen_wordcard_btn_cuo.set_size(20, 20)
# Set style for screen_wordcard_btn_cuo, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_wordcard_btn_cuo.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wordcard_btn_cuo.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wordcard_btn_cuo.set_style_radius(20, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wordcard_btn_cuo.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wordcard_btn_cuo.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wordcard_btn_cuo.set_style_text_font(test_font("SourceHanSansSC_Medium", 14), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wordcard_btn_cuo.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wordcard_btn_cuo.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
# Set style for screen_wordcard_btn_cuo, Part: lv.PART.MAIN, State: lv.STATE.FOCUSED.
screen_wordcard_btn_cuo.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_wordcard_btn_cuo.set_style_border_width(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_wordcard_btn_cuo.set_style_radius(20, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_wordcard_btn_cuo.set_style_shadow_width(10, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_wordcard_btn_cuo.set_style_shadow_color(lv.color_hex(0x274a6e), lv.PART.MAIN|lv.STATE.FOCUSED)
screen_wordcard_btn_cuo.set_style_shadow_opa(255, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_wordcard_btn_cuo.set_style_shadow_spread(1, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_wordcard_btn_cuo.set_style_shadow_ofs_x(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_wordcard_btn_cuo.set_style_shadow_ofs_y(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_wordcard_btn_cuo.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.FOCUSED)
screen_wordcard_btn_cuo.set_style_text_font(test_font("SourceHanSansSC_Medium", 14), lv.PART.MAIN|lv.STATE.FOCUSED)
screen_wordcard_btn_cuo.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.FOCUSED)

# Set style for screen_wordcard_btn_cuo, Part: lv.PART.MAIN, State: lv.STATE.DISABLED.
screen_wordcard_btn_cuo.set_style_bg_opa(172, lv.PART.MAIN|lv.STATE.DISABLED)
screen_wordcard_btn_cuo.set_style_bg_color(lv.color_hex(0x555555), lv.PART.MAIN|lv.STATE.DISABLED)
screen_wordcard_btn_cuo.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DISABLED)
screen_wordcard_btn_cuo.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DISABLED)
screen_wordcard_btn_cuo.set_style_radius(20, lv.PART.MAIN|lv.STATE.DISABLED)
screen_wordcard_btn_cuo.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DISABLED)
screen_wordcard_btn_cuo.set_style_text_color(lv.color_hex(0x000000), lv.PART.MAIN|lv.STATE.DISABLED)
screen_wordcard_btn_cuo.set_style_text_font(test_font("montserratMedium", 12), lv.PART.MAIN|lv.STATE.DISABLED)
screen_wordcard_btn_cuo.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DISABLED)

# Create screen_wordcard_btn_back
screen_wordcard_btn_back = lv.btn(screen_wordcard)
screen_wordcard_btn_back_label = lv.label(screen_wordcard_btn_back)
screen_wordcard_btn_back_label.set_text("")
screen_wordcard_btn_back_label.set_long_mode(lv.label.LONG.WRAP)
screen_wordcard_btn_back_label.set_width(lv.pct(100))
screen_wordcard_btn_back_label.align(lv.ALIGN.CENTER, 0, 0)
screen_wordcard_btn_back.set_style_pad_all(0, lv.STATE.DEFAULT)
screen_wordcard_btn_back.set_pos(8, 10)
screen_wordcard_btn_back.set_size(15, 15)
# Set style for screen_wordcard_btn_back, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_wordcard_btn_back.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wordcard_btn_back.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wordcard_btn_back.set_style_radius(5, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wordcard_btn_back.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wordcard_btn_back.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wordcard_btn_back.set_style_text_font(test_font("montserratMedium", 16), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wordcard_btn_back.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wordcard_btn_back.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
# Set style for screen_wordcard_btn_back, Part: lv.PART.MAIN, State: lv.STATE.FOCUSED.
screen_wordcard_btn_back.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_wordcard_btn_back.set_style_border_width(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_wordcard_btn_back.set_style_radius(5, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_wordcard_btn_back.set_style_shadow_width(5, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_wordcard_btn_back.set_style_shadow_color(lv.color_hex(0x49607b), lv.PART.MAIN|lv.STATE.FOCUSED)
screen_wordcard_btn_back.set_style_shadow_opa(255, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_wordcard_btn_back.set_style_shadow_spread(1, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_wordcard_btn_back.set_style_shadow_ofs_x(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_wordcard_btn_back.set_style_shadow_ofs_y(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_wordcard_btn_back.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.FOCUSED)
screen_wordcard_btn_back.set_style_text_font(test_font("montserratMedium", 16), lv.PART.MAIN|lv.STATE.FOCUSED)
screen_wordcard_btn_back.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.FOCUSED)

# Create screen_wordcard_label_spelling
screen_wordcard_label_spelling = lv.label(screen_wordcard)
screen_wordcard_label_spelling.set_text("Acquire")
screen_wordcard_label_spelling.set_long_mode(lv.label.LONG.SCROLL_CIRCULAR)
screen_wordcard_label_spelling.set_width(lv.pct(100))
screen_wordcard_label_spelling.set_pos(32, 28)
screen_wordcard_label_spelling.set_size(256, 41)
# Set style for screen_wordcard_label_spelling, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_wordcard_label_spelling.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wordcard_label_spelling.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wordcard_label_spelling.set_style_text_color(lv.color_hex(0x274a61), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wordcard_label_spelling.set_style_text_font(test_font("SourceHanSansSC_Medium", 28), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wordcard_label_spelling.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wordcard_label_spelling.set_style_text_letter_space(2, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wordcard_label_spelling.set_style_text_line_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wordcard_label_spelling.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wordcard_label_spelling.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wordcard_label_spelling.set_style_pad_top(8, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wordcard_label_spelling.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wordcard_label_spelling.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wordcard_label_spelling.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wordcard_label_spelling.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_wordcard_dog_cuo
screen_wordcard_dog_cuo = lv.animimg(screen_wordcard)
screen_wordcard_dog_cuo_imgs = [None]*6
screen_wordcard_dog_cuo_imgs[0] = "B:MicroPython/screen_wordcard_dog_cuoAAA_dog_shake_0_34x34.bin"
screen_wordcard_dog_cuo_imgs[1] = "B:MicroPython/screen_wordcard_dog_cuoAAA_dog_shake_1_34x34.bin"
screen_wordcard_dog_cuo_imgs[2] = "B:MicroPython/screen_wordcard_dog_cuoAAA_dog_shake_2_34x34.bin"
screen_wordcard_dog_cuo_imgs[3] = "B:MicroPython/screen_wordcard_dog_cuoAAA_dog_shake_3_34x34.bin"
screen_wordcard_dog_cuo_imgs[4] = "B:MicroPython/screen_wordcard_dog_cuoAAA_dog_shake_4_34x34.bin"
screen_wordcard_dog_cuo_imgs[5] = "B:MicroPython/screen_wordcard_dog_cuoAAA_dog_shake_5_34x34.bin"
screen_wordcard_dog_cuo.set_src(screen_wordcard_dog_cuo_imgs, 6, False)
screen_wordcard_dog_cuo.set_duration(50*6)
screen_wordcard_dog_cuo.set_repeat_count(lv.ANIM_REPEAT.INFINITE)
screen_wordcard_dog_cuo.start()
screen_wordcard_dog_cuo.set_pos(241, 85)
screen_wordcard_dog_cuo.set_size(34, 34)

# Create screen_wordcard_dog_dui
screen_wordcard_dog_dui = lv.animimg(screen_wordcard)
screen_wordcard_dog_dui_imgs = [None]*3
screen_wordcard_dog_dui_imgs[0] = "B:MicroPython/screen_wordcard_dog_duiAAA_dog_love_0_34x34.bin"
screen_wordcard_dog_dui_imgs[1] = "B:MicroPython/screen_wordcard_dog_duiAAA_dog_love_1_34x34.bin"
screen_wordcard_dog_dui_imgs[2] = "B:MicroPython/screen_wordcard_dog_duiAAA_dog_love_2_34x34.bin"
screen_wordcard_dog_dui.set_src(screen_wordcard_dog_dui_imgs, 3, False)
screen_wordcard_dog_dui.set_duration(100*3)
screen_wordcard_dog_dui.set_repeat_count(lv.ANIM_REPEAT.INFINITE)
screen_wordcard_dog_dui.start()
screen_wordcard_dog_dui.set_pos(42, 85)
screen_wordcard_dog_dui.set_size(34, 34)

screen_wordcard.update_layout()
# Create screen_timing
screen_timing = lv.obj()
g_kb_screen_timing = lv.keyboard(screen_timing)
g_kb_screen_timing.add_event_cb(lambda e: ta_event_cb(e, g_kb_screen_timing), lv.EVENT.ALL, None)
g_kb_screen_timing.add_flag(lv.obj.FLAG.HIDDEN)
g_kb_screen_timing.set_style_text_font(test_font("Acme_Regular", 12), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_timing.set_size(320, 170)
screen_timing.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
# Set style for screen_timing, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_timing.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_timing.set_style_bg_color(lv.color_hex(0x000000), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_timing.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_timing.set_style_bg_img_src("B:MicroPython/_AAA_clock_bg_320x170.bin", lv.PART.MAIN|lv.STATE.DEFAULT)
screen_timing.set_style_bg_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_timing.set_style_bg_img_recolor_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_timing_btn_main
screen_timing_btn_main = lv.btn(screen_timing)
screen_timing_btn_main_label = lv.label(screen_timing_btn_main)
screen_timing_btn_main_label.set_text("")
screen_timing_btn_main_label.set_long_mode(lv.label.LONG.WRAP)
screen_timing_btn_main_label.set_width(lv.pct(100))
screen_timing_btn_main_label.align(lv.ALIGN.CENTER, 0, 0)
screen_timing_btn_main.set_style_pad_all(0, lv.STATE.DEFAULT)
screen_timing_btn_main.set_pos(0, 0)
screen_timing_btn_main.set_size(320, 170)
# Set style for screen_timing_btn_main, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_timing_btn_main.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_timing_btn_main.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_timing_btn_main.set_style_radius(5, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_timing_btn_main.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_timing_btn_main.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_timing_btn_main.set_style_text_font(test_font("montserratMedium", 16), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_timing_btn_main.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_timing_btn_main.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
# Set style for screen_timing_btn_main, Part: lv.PART.MAIN, State: lv.STATE.FOCUSED.
screen_timing_btn_main.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_timing_btn_main.set_style_border_width(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_timing_btn_main.set_style_radius(5, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_timing_btn_main.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_timing_btn_main.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.FOCUSED)
screen_timing_btn_main.set_style_text_font(test_font("montserratMedium", 16), lv.PART.MAIN|lv.STATE.FOCUSED)
screen_timing_btn_main.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.FOCUSED)

# Create screen_timing_label_realtime
screen_timing_label_realtime = lv.label(screen_timing)
screen_timing_label_realtime.set_text("10:32")
screen_timing_label_realtime.set_long_mode(lv.label.LONG.WRAP)
screen_timing_label_realtime.set_width(lv.pct(100))
screen_timing_label_realtime.set_pos(17, 43)
screen_timing_label_realtime.set_size(156, 56)
# Set style for screen_timing_label_realtime, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_timing_label_realtime.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_timing_label_realtime.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_timing_label_realtime.set_style_text_color(lv.color_hex(0x274a6e), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_timing_label_realtime.set_style_text_font(test_font("SourceHanSansSC_Medium", 52), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_timing_label_realtime.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_timing_label_realtime.set_style_text_letter_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_timing_label_realtime.set_style_text_line_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_timing_label_realtime.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_timing_label_realtime.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_timing_label_realtime.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_timing_label_realtime.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_timing_label_realtime.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_timing_label_realtime.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_timing_label_realtime.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_timing_label_day
screen_timing_label_day = lv.label(screen_timing)
screen_timing_label_day.set_text("Saturday")
screen_timing_label_day.set_long_mode(lv.label.LONG.WRAP)
screen_timing_label_day.set_width(lv.pct(100))
screen_timing_label_day.set_pos(198, 72)
screen_timing_label_day.set_size(97, 19)
# Set style for screen_timing_label_day, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_timing_label_day.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_timing_label_day.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_timing_label_day.set_style_text_color(lv.color_hex(0x274a6e), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_timing_label_day.set_style_text_font(test_font("SourceHanSansSC_Medium", 17), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_timing_label_day.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_timing_label_day.set_style_text_letter_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_timing_label_day.set_style_text_line_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_timing_label_day.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_timing_label_day.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_timing_label_day.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_timing_label_day.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_timing_label_day.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_timing_label_day.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_timing_label_day.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_timing_label_date
screen_timing_label_date = lv.label(screen_timing)
screen_timing_label_date.set_text("2025/12/25")
screen_timing_label_date.set_long_mode(lv.label.LONG.WRAP)
screen_timing_label_date.set_width(lv.pct(100))
screen_timing_label_date.set_pos(195, 49)
screen_timing_label_date.set_size(97, 16)
# Set style for screen_timing_label_date, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_timing_label_date.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_timing_label_date.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_timing_label_date.set_style_text_color(lv.color_hex(0x274a6e), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_timing_label_date.set_style_text_font(test_font("SourceHanSansSC_Medium", 17), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_timing_label_date.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_timing_label_date.set_style_text_letter_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_timing_label_date.set_style_text_line_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_timing_label_date.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_timing_label_date.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_timing_label_date.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_timing_label_date.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_timing_label_date.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_timing_label_date.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_timing_label_date.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_timing_bar_battery
screen_timing_bar_battery = lv.bar(screen_timing)
screen_timing_bar_battery.set_style_anim_time(1000, 0)
screen_timing_bar_battery.set_mode(lv.bar.MODE.NORMAL)
screen_timing_bar_battery.set_range(0, 100)
screen_timing_bar_battery.set_value(50, lv.ANIM.OFF)
screen_timing_bar_battery.set_pos(32, 127)
screen_timing_bar_battery.set_size(257, 2)
# Set style for screen_timing_bar_battery, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_timing_bar_battery.set_style_bg_opa(98, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_timing_bar_battery.set_style_bg_color(lv.color_hex(0x274a6e), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_timing_bar_battery.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_timing_bar_battery.set_style_radius(5, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_timing_bar_battery.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
# Set style for screen_timing_bar_battery, Part: lv.PART.INDICATOR, State: lv.STATE.DEFAULT.
screen_timing_bar_battery.set_style_bg_opa(255, lv.PART.INDICATOR|lv.STATE.DEFAULT)
screen_timing_bar_battery.set_style_bg_color(lv.color_hex(0x274a6e), lv.PART.INDICATOR|lv.STATE.DEFAULT)
screen_timing_bar_battery.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.INDICATOR|lv.STATE.DEFAULT)
screen_timing_bar_battery.set_style_radius(10, lv.PART.INDICATOR|lv.STATE.DEFAULT)

# Create screen_timing_img_baticon
screen_timing_img_baticon = lv.img(screen_timing)
screen_timing_img_baticon.set_src("B:MicroPython/_AAA_shandian_icon_alpha_9x13.bin")
screen_timing_img_baticon.add_flag(lv.obj.FLAG.CLICKABLE)
screen_timing_img_baticon.set_pivot(50,50)
screen_timing_img_baticon.set_angle(0)
screen_timing_img_baticon.set_pos(285, 109)
screen_timing_img_baticon.set_size(9, 13)
# Set style for screen_timing_img_baticon, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_timing_img_baticon.set_style_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_timing_img_baticon.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_timing_img_baticon.set_style_clip_corner(True, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_timing_img_wificonnect
screen_timing_img_wificonnect = lv.img(screen_timing)
screen_timing_img_wificonnect.set_src("B:MicroPython/_AAA_clock_wifi_icon_alpha_14x14.bin")
screen_timing_img_wificonnect.add_flag(lv.obj.FLAG.CLICKABLE)
screen_timing_img_wificonnect.set_pivot(50,50)
screen_timing_img_wificonnect.set_angle(0)
screen_timing_img_wificonnect.set_pos(288, 7)
screen_timing_img_wificonnect.set_size(14, 14)
# Set style for screen_timing_img_wificonnect, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_timing_img_wificonnect.set_style_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_timing_img_wificonnect.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_timing_img_wificonnect.set_style_clip_corner(True, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_timing_img_wifidis
screen_timing_img_wifidis = lv.img(screen_timing)
screen_timing_img_wifidis.set_src("B:MicroPython/_AAA_clock_wifiduan_icon_alpha_15x15.bin")
screen_timing_img_wifidis.add_flag(lv.obj.FLAG.CLICKABLE)
screen_timing_img_wifidis.set_pivot(50,50)
screen_timing_img_wifidis.set_angle(0)
screen_timing_img_wifidis.set_pos(288, 7)
screen_timing_img_wifidis.set_size(15, 15)
# Set style for screen_timing_img_wifidis, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_timing_img_wifidis.set_style_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_timing_img_wifidis.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_timing_img_wifidis.set_style_clip_corner(True, lv.PART.MAIN|lv.STATE.DEFAULT)

screen_timing.update_layout()
# Create screen_wordmeun
screen_wordmeun = lv.obj()
g_kb_screen_wordmeun = lv.keyboard(screen_wordmeun)
g_kb_screen_wordmeun.add_event_cb(lambda e: ta_event_cb(e, g_kb_screen_wordmeun), lv.EVENT.ALL, None)
g_kb_screen_wordmeun.add_flag(lv.obj.FLAG.HIDDEN)
g_kb_screen_wordmeun.set_style_text_font(test_font("Acme_Regular", 12), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wordmeun.set_size(320, 170)
screen_wordmeun.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
# Set style for screen_wordmeun, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_wordmeun.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wordmeun.set_style_bg_color(lv.color_hex(0x000000), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wordmeun.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wordmeun.set_style_bg_img_src("B:MicroPython/_AAA_cishu_320x170.bin", lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wordmeun.set_style_bg_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wordmeun.set_style_bg_img_recolor_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_wordmeun_bar_process
screen_wordmeun_bar_process = lv.bar(screen_wordmeun)
screen_wordmeun_bar_process.set_style_anim_time(1000, 0)
screen_wordmeun_bar_process.set_mode(lv.bar.MODE.NORMAL)
screen_wordmeun_bar_process.set_range(0, 100)
screen_wordmeun_bar_process.set_value(100, lv.ANIM.OFF)
screen_wordmeun_bar_process.set_pos(84, 90)
screen_wordmeun_bar_process.set_size(203, 5)
# Set style for screen_wordmeun_bar_process, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_wordmeun_bar_process.set_style_bg_opa(134, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wordmeun_bar_process.set_style_bg_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wordmeun_bar_process.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wordmeun_bar_process.set_style_radius(10, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wordmeun_bar_process.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
# Set style for screen_wordmeun_bar_process, Part: lv.PART.INDICATOR, State: lv.STATE.DEFAULT.
screen_wordmeun_bar_process.set_style_bg_opa(152, lv.PART.INDICATOR|lv.STATE.DEFAULT)
screen_wordmeun_bar_process.set_style_bg_color(lv.color_hex(0x2195f6), lv.PART.INDICATOR|lv.STATE.DEFAULT)
screen_wordmeun_bar_process.set_style_bg_grad_dir(lv.GRAD_DIR.HOR, lv.PART.INDICATOR|lv.STATE.DEFAULT)
screen_wordmeun_bar_process.set_style_bg_grad_color(lv.color_hex(0x0a3763), lv.PART.INDICATOR|lv.STATE.DEFAULT)
screen_wordmeun_bar_process.set_style_bg_main_stop(0, lv.PART.INDICATOR|lv.STATE.DEFAULT)
screen_wordmeun_bar_process.set_style_bg_grad_stop(0, lv.PART.INDICATOR|lv.STATE.DEFAULT)
screen_wordmeun_bar_process.set_style_radius(10, lv.PART.INDICATOR|lv.STATE.DEFAULT)

# Create screen_wordmeun_label_wordpercent
screen_wordmeun_label_wordpercent = lv.label(screen_wordmeun)
screen_wordmeun_label_wordpercent.set_text("25%")
screen_wordmeun_label_wordpercent.set_long_mode(lv.label.LONG.WRAP)
screen_wordmeun_label_wordpercent.set_width(lv.pct(100))
screen_wordmeun_label_wordpercent.set_pos(252, 104)
screen_wordmeun_label_wordpercent.set_size(36, 14)
# Set style for screen_wordmeun_label_wordpercent, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_wordmeun_label_wordpercent.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wordmeun_label_wordpercent.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wordmeun_label_wordpercent.set_style_text_color(lv.color_hex(0x274a61), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wordmeun_label_wordpercent.set_style_text_font(test_font("SourceHanSansSC_Medium", 12), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wordmeun_label_wordpercent.set_style_text_opa(180, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wordmeun_label_wordpercent.set_style_text_letter_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wordmeun_label_wordpercent.set_style_text_line_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wordmeun_label_wordpercent.set_style_text_align(lv.TEXT_ALIGN.RIGHT, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wordmeun_label_wordpercent.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wordmeun_label_wordpercent.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wordmeun_label_wordpercent.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wordmeun_label_wordpercent.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wordmeun_label_wordpercent.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wordmeun_label_wordpercent.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_wordmeun_label_reviewnum
screen_wordmeun_label_reviewnum = lv.label(screen_wordmeun)
screen_wordmeun_label_reviewnum.set_text("Review: 100")
screen_wordmeun_label_reviewnum.set_long_mode(lv.label.LONG.WRAP)
screen_wordmeun_label_reviewnum.set_width(lv.pct(100))
screen_wordmeun_label_reviewnum.set_pos(84, 104)
screen_wordmeun_label_reviewnum.set_size(91, 15)
# Set style for screen_wordmeun_label_reviewnum, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_wordmeun_label_reviewnum.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wordmeun_label_reviewnum.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wordmeun_label_reviewnum.set_style_text_color(lv.color_hex(0x274a61), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wordmeun_label_reviewnum.set_style_text_font(test_font("SourceHanSansSC_Medium", 12), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wordmeun_label_reviewnum.set_style_text_opa(186, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wordmeun_label_reviewnum.set_style_text_letter_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wordmeun_label_reviewnum.set_style_text_line_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wordmeun_label_reviewnum.set_style_text_align(lv.TEXT_ALIGN.LEFT, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wordmeun_label_reviewnum.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wordmeun_label_reviewnum.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wordmeun_label_reviewnum.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wordmeun_label_reviewnum.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wordmeun_label_reviewnum.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wordmeun_label_reviewnum.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_wordmeun_btn_learn
screen_wordmeun_btn_learn = lv.btn(screen_wordmeun)
screen_wordmeun_btn_learn_label = lv.label(screen_wordmeun_btn_learn)
screen_wordmeun_btn_learn_label.set_text("")
screen_wordmeun_btn_learn_label.set_long_mode(lv.label.LONG.WRAP)
screen_wordmeun_btn_learn_label.set_width(lv.pct(100))
screen_wordmeun_btn_learn_label.align(lv.ALIGN.CENTER, 0, 0)
screen_wordmeun_btn_learn.set_style_pad_all(0, lv.STATE.DEFAULT)
screen_wordmeun_btn_learn.set_pos(29, 136)
screen_wordmeun_btn_learn.set_size(127, 17)
# Set style for screen_wordmeun_btn_learn, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_wordmeun_btn_learn.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wordmeun_btn_learn.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wordmeun_btn_learn.set_style_radius(5, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wordmeun_btn_learn.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wordmeun_btn_learn.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wordmeun_btn_learn.set_style_text_font(test_font("montserratMedium", 16), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wordmeun_btn_learn.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wordmeun_btn_learn.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
# Set style for screen_wordmeun_btn_learn, Part: lv.PART.MAIN, State: lv.STATE.FOCUSED.
screen_wordmeun_btn_learn.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_wordmeun_btn_learn.set_style_border_width(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_wordmeun_btn_learn.set_style_radius(5, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_wordmeun_btn_learn.set_style_shadow_width(5, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_wordmeun_btn_learn.set_style_shadow_color(lv.color_hex(0x49607b), lv.PART.MAIN|lv.STATE.FOCUSED)
screen_wordmeun_btn_learn.set_style_shadow_opa(255, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_wordmeun_btn_learn.set_style_shadow_spread(1, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_wordmeun_btn_learn.set_style_shadow_ofs_x(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_wordmeun_btn_learn.set_style_shadow_ofs_y(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_wordmeun_btn_learn.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.FOCUSED)
screen_wordmeun_btn_learn.set_style_text_font(test_font("montserratMedium", 16), lv.PART.MAIN|lv.STATE.FOCUSED)
screen_wordmeun_btn_learn.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.FOCUSED)

# Create screen_wordmeun_btn_review
screen_wordmeun_btn_review = lv.btn(screen_wordmeun)
screen_wordmeun_btn_review_label = lv.label(screen_wordmeun_btn_review)
screen_wordmeun_btn_review_label.set_text("")
screen_wordmeun_btn_review_label.set_long_mode(lv.label.LONG.WRAP)
screen_wordmeun_btn_review_label.set_width(lv.pct(100))
screen_wordmeun_btn_review_label.align(lv.ALIGN.CENTER, 0, 0)
screen_wordmeun_btn_review.set_style_pad_all(0, lv.STATE.DEFAULT)
screen_wordmeun_btn_review.set_pos(164, 136)
screen_wordmeun_btn_review.set_size(126, 16)
# Set style for screen_wordmeun_btn_review, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_wordmeun_btn_review.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wordmeun_btn_review.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wordmeun_btn_review.set_style_radius(5, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wordmeun_btn_review.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wordmeun_btn_review.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wordmeun_btn_review.set_style_text_font(test_font("montserratMedium", 16), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wordmeun_btn_review.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wordmeun_btn_review.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
# Set style for screen_wordmeun_btn_review, Part: lv.PART.MAIN, State: lv.STATE.FOCUSED.
screen_wordmeun_btn_review.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_wordmeun_btn_review.set_style_border_width(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_wordmeun_btn_review.set_style_radius(5, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_wordmeun_btn_review.set_style_shadow_width(5, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_wordmeun_btn_review.set_style_shadow_color(lv.color_hex(0x49607b), lv.PART.MAIN|lv.STATE.FOCUSED)
screen_wordmeun_btn_review.set_style_shadow_opa(255, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_wordmeun_btn_review.set_style_shadow_spread(1, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_wordmeun_btn_review.set_style_shadow_ofs_x(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_wordmeun_btn_review.set_style_shadow_ofs_y(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_wordmeun_btn_review.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.FOCUSED)
screen_wordmeun_btn_review.set_style_text_font(test_font("montserratMedium", 16), lv.PART.MAIN|lv.STATE.FOCUSED)
screen_wordmeun_btn_review.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.FOCUSED)

# Create screen_wordmeun_btn_back
screen_wordmeun_btn_back = lv.btn(screen_wordmeun)
screen_wordmeun_btn_back_label = lv.label(screen_wordmeun_btn_back)
screen_wordmeun_btn_back_label.set_text("")
screen_wordmeun_btn_back_label.set_long_mode(lv.label.LONG.WRAP)
screen_wordmeun_btn_back_label.set_width(lv.pct(100))
screen_wordmeun_btn_back_label.align(lv.ALIGN.CENTER, 0, 0)
screen_wordmeun_btn_back.set_style_pad_all(0, lv.STATE.DEFAULT)
screen_wordmeun_btn_back.set_pos(8, 10)
screen_wordmeun_btn_back.set_size(15, 15)
# Set style for screen_wordmeun_btn_back, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_wordmeun_btn_back.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wordmeun_btn_back.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wordmeun_btn_back.set_style_radius(5, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wordmeun_btn_back.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wordmeun_btn_back.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wordmeun_btn_back.set_style_text_font(test_font("montserratMedium", 16), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wordmeun_btn_back.set_style_text_opa(165, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wordmeun_btn_back.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
# Set style for screen_wordmeun_btn_back, Part: lv.PART.MAIN, State: lv.STATE.FOCUSED.
screen_wordmeun_btn_back.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_wordmeun_btn_back.set_style_border_width(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_wordmeun_btn_back.set_style_radius(5, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_wordmeun_btn_back.set_style_shadow_width(5, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_wordmeun_btn_back.set_style_shadow_color(lv.color_hex(0x49607b), lv.PART.MAIN|lv.STATE.FOCUSED)
screen_wordmeun_btn_back.set_style_shadow_opa(255, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_wordmeun_btn_back.set_style_shadow_spread(1, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_wordmeun_btn_back.set_style_shadow_ofs_x(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_wordmeun_btn_back.set_style_shadow_ofs_y(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_wordmeun_btn_back.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.FOCUSED)
screen_wordmeun_btn_back.set_style_text_font(test_font("montserratMedium", 16), lv.PART.MAIN|lv.STATE.FOCUSED)
screen_wordmeun_btn_back.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.FOCUSED)

screen_wordmeun.update_layout()
# Create screen_reviewcard
screen_reviewcard = lv.obj()
g_kb_screen_reviewcard = lv.keyboard(screen_reviewcard)
g_kb_screen_reviewcard.add_event_cb(lambda e: ta_event_cb(e, g_kb_screen_reviewcard), lv.EVENT.ALL, None)
g_kb_screen_reviewcard.add_flag(lv.obj.FLAG.HIDDEN)
g_kb_screen_reviewcard.set_style_text_font(test_font("Acme_Regular", 12), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_reviewcard.set_size(320, 170)
screen_reviewcard.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
# Set style for screen_reviewcard, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_reviewcard.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_reviewcard.set_style_bg_color(lv.color_hex(0x000000), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_reviewcard.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_reviewcard.set_style_bg_img_src("B:MicroPython/_AAA_flashcards_review_bg_320x170.bin", lv.PART.MAIN|lv.STATE.DEFAULT)
screen_reviewcard.set_style_bg_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_reviewcard.set_style_bg_img_recolor_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_reviewcard_label_index
screen_reviewcard_label_index = lv.label(screen_reviewcard)
screen_reviewcard_label_index.set_text("1/100")
screen_reviewcard_label_index.set_long_mode(lv.label.LONG.WRAP)
screen_reviewcard_label_index.set_width(lv.pct(100))
screen_reviewcard_label_index.set_pos(219, 7)
screen_reviewcard_label_index.set_size(71, 14)
# Set style for screen_reviewcard_label_index, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_reviewcard_label_index.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_reviewcard_label_index.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_reviewcard_label_index.set_style_text_color(lv.color_hex(0x274a61), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_reviewcard_label_index.set_style_text_font(test_font("SourceHanSansSC_Medium", 14), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_reviewcard_label_index.set_style_text_opa(207, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_reviewcard_label_index.set_style_text_letter_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_reviewcard_label_index.set_style_text_line_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_reviewcard_label_index.set_style_text_align(lv.TEXT_ALIGN.RIGHT, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_reviewcard_label_index.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_reviewcard_label_index.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_reviewcard_label_index.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_reviewcard_label_index.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_reviewcard_label_index.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_reviewcard_label_index.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_reviewcard_label_spelling
screen_reviewcard_label_spelling = lv.label(screen_reviewcard)
screen_reviewcard_label_spelling.set_text("Abondon")
screen_reviewcard_label_spelling.set_long_mode(lv.label.LONG.SCROLL)
screen_reviewcard_label_spelling.set_width(lv.pct(100))
screen_reviewcard_label_spelling.set_pos(32, 26)
screen_reviewcard_label_spelling.set_size(257, 43)
# Set style for screen_reviewcard_label_spelling, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_reviewcard_label_spelling.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_reviewcard_label_spelling.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_reviewcard_label_spelling.set_style_text_color(lv.color_hex(0x274a61), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_reviewcard_label_spelling.set_style_text_font(test_font("SourceHanSansSC_Medium", 28), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_reviewcard_label_spelling.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_reviewcard_label_spelling.set_style_text_letter_space(2, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_reviewcard_label_spelling.set_style_text_line_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_reviewcard_label_spelling.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_reviewcard_label_spelling.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_reviewcard_label_spelling.set_style_pad_top(8, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_reviewcard_label_spelling.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_reviewcard_label_spelling.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_reviewcard_label_spelling.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_reviewcard_label_spelling.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_reviewcard_label_phonetic
screen_reviewcard_label_phonetic = lv.label(screen_reviewcard)
screen_reviewcard_label_phonetic.set_text("ɑ ɒ ɔ ə ɛ ɜ ɪ ʊ ʌ æ e ɪ a ɪ ɔ ɪ a ʊ ə ʊ ɪ ə e ə ʊ ə p t k f θ s ʃ h tʃ tr ts b d v ð z ʒ r dʒ dr dz m n ŋ l j w ˈ ˌ ː . [ ] / ( ) ɡ ，。ɚ ɝ̃")
screen_reviewcard_label_phonetic.set_long_mode(lv.label.LONG.WRAP)
screen_reviewcard_label_phonetic.set_width(lv.pct(100))
screen_reviewcard_label_phonetic.set_pos(32, 66)
screen_reviewcard_label_phonetic.set_size(282, 15)
# Set style for screen_reviewcard_label_phonetic, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_reviewcard_label_phonetic.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_reviewcard_label_phonetic.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_reviewcard_label_phonetic.set_style_text_color(lv.color_hex(0x274a61), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_reviewcard_label_phonetic.set_style_text_font(test_font("arial", 14), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_reviewcard_label_phonetic.set_style_text_opa(143, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_reviewcard_label_phonetic.set_style_text_letter_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_reviewcard_label_phonetic.set_style_text_line_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_reviewcard_label_phonetic.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_reviewcard_label_phonetic.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_reviewcard_label_phonetic.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_reviewcard_label_phonetic.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_reviewcard_label_phonetic.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_reviewcard_label_phonetic.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_reviewcard_label_phonetic.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_reviewcard_label_meaning
screen_reviewcard_label_meaning = lv.label(screen_reviewcard)
screen_reviewcard_label_meaning.set_text("v.放弃;抛弃;放纵")
screen_reviewcard_label_meaning.set_long_mode(lv.label.LONG.WRAP)
screen_reviewcard_label_meaning.set_width(lv.pct(100))
screen_reviewcard_label_meaning.set_pos(31, 92)
screen_reviewcard_label_meaning.set_size(257, 15)
# Set style for screen_reviewcard_label_meaning, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_reviewcard_label_meaning.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_reviewcard_label_meaning.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_reviewcard_label_meaning.set_style_text_color(lv.color_hex(0x274a61), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_reviewcard_label_meaning.set_style_text_font(test_font("SourceHanSansSC_Medium", 14), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_reviewcard_label_meaning.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_reviewcard_label_meaning.set_style_text_letter_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_reviewcard_label_meaning.set_style_text_line_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_reviewcard_label_meaning.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_reviewcard_label_meaning.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_reviewcard_label_meaning.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_reviewcard_label_meaning.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_reviewcard_label_meaning.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_reviewcard_label_meaning.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_reviewcard_label_meaning.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_reviewcard_btn_dui
screen_reviewcard_btn_dui = lv.btn(screen_reviewcard)
screen_reviewcard_btn_dui_label = lv.label(screen_reviewcard_btn_dui)
screen_reviewcard_btn_dui_label.set_text("")
screen_reviewcard_btn_dui_label.set_long_mode(lv.label.LONG.WRAP)
screen_reviewcard_btn_dui_label.set_width(lv.pct(100))
screen_reviewcard_btn_dui_label.align(lv.ALIGN.CENTER, 0, 0)
screen_reviewcard_btn_dui.set_style_pad_all(0, lv.STATE.DEFAULT)
screen_reviewcard_btn_dui.set_pos(130, 123)
screen_reviewcard_btn_dui.set_size(20, 20)
# Set style for screen_reviewcard_btn_dui, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_reviewcard_btn_dui.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_reviewcard_btn_dui.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_reviewcard_btn_dui.set_style_radius(20, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_reviewcard_btn_dui.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_reviewcard_btn_dui.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_reviewcard_btn_dui.set_style_text_font(test_font("montserratMedium", 16), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_reviewcard_btn_dui.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_reviewcard_btn_dui.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
# Set style for screen_reviewcard_btn_dui, Part: lv.PART.MAIN, State: lv.STATE.FOCUSED.
screen_reviewcard_btn_dui.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_reviewcard_btn_dui.set_style_border_width(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_reviewcard_btn_dui.set_style_radius(20, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_reviewcard_btn_dui.set_style_shadow_width(10, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_reviewcard_btn_dui.set_style_shadow_color(lv.color_hex(0x274a6e), lv.PART.MAIN|lv.STATE.FOCUSED)
screen_reviewcard_btn_dui.set_style_shadow_opa(255, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_reviewcard_btn_dui.set_style_shadow_spread(1, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_reviewcard_btn_dui.set_style_shadow_ofs_x(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_reviewcard_btn_dui.set_style_shadow_ofs_y(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_reviewcard_btn_dui.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.FOCUSED)
screen_reviewcard_btn_dui.set_style_text_font(test_font("montserratMedium", 16), lv.PART.MAIN|lv.STATE.FOCUSED)
screen_reviewcard_btn_dui.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.FOCUSED)

# Set style for screen_reviewcard_btn_dui, Part: lv.PART.MAIN, State: lv.STATE.DISABLED.
screen_reviewcard_btn_dui.set_style_bg_opa(131, lv.PART.MAIN|lv.STATE.DISABLED)
screen_reviewcard_btn_dui.set_style_bg_color(lv.color_hex(0x555555), lv.PART.MAIN|lv.STATE.DISABLED)
screen_reviewcard_btn_dui.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DISABLED)
screen_reviewcard_btn_dui.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DISABLED)
screen_reviewcard_btn_dui.set_style_radius(20, lv.PART.MAIN|lv.STATE.DISABLED)
screen_reviewcard_btn_dui.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DISABLED)
screen_reviewcard_btn_dui.set_style_text_color(lv.color_hex(0x000000), lv.PART.MAIN|lv.STATE.DISABLED)
screen_reviewcard_btn_dui.set_style_text_font(test_font("montserratMedium", 12), lv.PART.MAIN|lv.STATE.DISABLED)
screen_reviewcard_btn_dui.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DISABLED)

# Create screen_reviewcard_btn_back
screen_reviewcard_btn_back = lv.btn(screen_reviewcard)
screen_reviewcard_btn_back_label = lv.label(screen_reviewcard_btn_back)
screen_reviewcard_btn_back_label.set_text("")
screen_reviewcard_btn_back_label.set_long_mode(lv.label.LONG.WRAP)
screen_reviewcard_btn_back_label.set_width(lv.pct(100))
screen_reviewcard_btn_back_label.align(lv.ALIGN.CENTER, 0, 0)
screen_reviewcard_btn_back.set_style_pad_all(0, lv.STATE.DEFAULT)
screen_reviewcard_btn_back.set_pos(8, 10)
screen_reviewcard_btn_back.set_size(15, 15)
# Set style for screen_reviewcard_btn_back, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_reviewcard_btn_back.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_reviewcard_btn_back.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_reviewcard_btn_back.set_style_radius(5, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_reviewcard_btn_back.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_reviewcard_btn_back.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_reviewcard_btn_back.set_style_text_font(test_font("montserratMedium", 16), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_reviewcard_btn_back.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_reviewcard_btn_back.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
# Set style for screen_reviewcard_btn_back, Part: lv.PART.MAIN, State: lv.STATE.FOCUSED.
screen_reviewcard_btn_back.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_reviewcard_btn_back.set_style_border_width(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_reviewcard_btn_back.set_style_radius(5, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_reviewcard_btn_back.set_style_shadow_width(5, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_reviewcard_btn_back.set_style_shadow_color(lv.color_hex(0x274a6e), lv.PART.MAIN|lv.STATE.FOCUSED)
screen_reviewcard_btn_back.set_style_shadow_opa(255, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_reviewcard_btn_back.set_style_shadow_spread(1, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_reviewcard_btn_back.set_style_shadow_ofs_x(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_reviewcard_btn_back.set_style_shadow_ofs_y(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_reviewcard_btn_back.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.FOCUSED)
screen_reviewcard_btn_back.set_style_text_font(test_font("montserratMedium", 16), lv.PART.MAIN|lv.STATE.FOCUSED)
screen_reviewcard_btn_back.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.FOCUSED)

# Create screen_reviewcard_dog_dance
screen_reviewcard_dog_dance = lv.animimg(screen_reviewcard)
screen_reviewcard_dog_dance_imgs = [None]*3
screen_reviewcard_dog_dance_imgs[0] = "B:MicroPython/screen_reviewcard_dog_danceAAA_dog_dance_0_34x34.bin"
screen_reviewcard_dog_dance_imgs[1] = "B:MicroPython/screen_reviewcard_dog_danceAAA_dog_dance_1_34x34.bin"
screen_reviewcard_dog_dance_imgs[2] = "B:MicroPython/screen_reviewcard_dog_danceAAA_dog_dance_2_34x34.bin"
screen_reviewcard_dog_dance.set_src(screen_reviewcard_dog_dance_imgs, 3, False)
screen_reviewcard_dog_dance.set_duration(100*3)
screen_reviewcard_dog_dance.set_repeat_count(lv.ANIM_REPEAT.INFINITE)
screen_reviewcard_dog_dance.start()
screen_reviewcard_dog_dance.set_pos(245, 30)
screen_reviewcard_dog_dance.set_size(34, 34)

# Create screen_reviewcard_dog_dui
screen_reviewcard_dog_dui = lv.animimg(screen_reviewcard)
screen_reviewcard_dog_dui_imgs = [None]*3
screen_reviewcard_dog_dui_imgs[0] = "B:MicroPython/screen_reviewcard_dog_duiAAA_dog_love_2_0_34x34.bin"
screen_reviewcard_dog_dui_imgs[1] = "B:MicroPython/screen_reviewcard_dog_duiAAA_dog_love_2_1_34x34.bin"
screen_reviewcard_dog_dui_imgs[2] = "B:MicroPython/screen_reviewcard_dog_duiAAA_dog_love_2_2_34x34.bin"
screen_reviewcard_dog_dui.set_src(screen_reviewcard_dog_dui_imgs, 3, False)
screen_reviewcard_dog_dui.set_duration(100*3)
screen_reviewcard_dog_dui.set_repeat_count(lv.ANIM_REPEAT.INFINITE)
screen_reviewcard_dog_dui.start()
screen_reviewcard_dog_dui.set_pos(91, 114)
screen_reviewcard_dog_dui.set_size(34, 34)

screen_reviewcard.update_layout()
# Create screen_pomodoro
screen_pomodoro = lv.obj()
g_kb_screen_pomodoro = lv.keyboard(screen_pomodoro)
g_kb_screen_pomodoro.add_event_cb(lambda e: ta_event_cb(e, g_kb_screen_pomodoro), lv.EVENT.ALL, None)
g_kb_screen_pomodoro.add_flag(lv.obj.FLAG.HIDDEN)
g_kb_screen_pomodoro.set_style_text_font(test_font("Acme_Regular", 12), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_pomodoro.set_size(320, 170)
screen_pomodoro.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
# Set style for screen_pomodoro, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_pomodoro.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_pomodoro.set_style_bg_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_pomodoro.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_pomodoro.set_style_bg_img_src("B:MicroPython/_AAA_fanqie_bg_320x170.bin", lv.PART.MAIN|lv.STATE.DEFAULT)
screen_pomodoro.set_style_bg_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_pomodoro.set_style_bg_img_recolor_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_pomodoro_img_pomorect1
screen_pomodoro_img_pomorect1 = lv.img(screen_pomodoro)
screen_pomodoro_img_pomorect1.set_src("B:MicroPython/_pomorect_alpha_257x52.bin")
screen_pomodoro_img_pomorect1.add_flag(lv.obj.FLAG.CLICKABLE)
screen_pomodoro_img_pomorect1.set_pivot(50,50)
screen_pomodoro_img_pomorect1.set_angle(0)
screen_pomodoro_img_pomorect1.set_pos(37, 29)
screen_pomodoro_img_pomorect1.set_size(257, 52)
# Set style for screen_pomodoro_img_pomorect1, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_pomodoro_img_pomorect1.set_style_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_pomodoro_img_pomorect1.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_pomodoro_img_pomorect1.set_style_clip_corner(True, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_pomodoro_label_timer
screen_pomodoro_label_timer = lv.label(screen_pomodoro)
screen_pomodoro_label_timer.set_text("00:25:00")
screen_pomodoro_label_timer.set_long_mode(lv.label.LONG.WRAP)
screen_pomodoro_label_timer.set_width(lv.pct(100))
screen_pomodoro_label_timer.set_pos(94, 40)
screen_pomodoro_label_timer.set_size(141, 27)
# Set style for screen_pomodoro_label_timer, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_pomodoro_label_timer.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_pomodoro_label_timer.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_pomodoro_label_timer.set_style_text_color(lv.color_hex(0x274a61), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_pomodoro_label_timer.set_style_text_font(test_font("Abel_regular", 30), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_pomodoro_label_timer.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_pomodoro_label_timer.set_style_text_letter_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_pomodoro_label_timer.set_style_text_line_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_pomodoro_label_timer.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_pomodoro_label_timer.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_pomodoro_label_timer.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_pomodoro_label_timer.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_pomodoro_label_timer.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_pomodoro_label_timer.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_pomodoro_label_timer.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_pomodoro_label_pomodoro
screen_pomodoro_label_pomodoro = lv.label(screen_pomodoro)
screen_pomodoro_label_pomodoro.set_text("Pomodoro Focus")
screen_pomodoro_label_pomodoro.set_long_mode(lv.label.LONG.WRAP)
screen_pomodoro_label_pomodoro.set_width(lv.pct(100))
screen_pomodoro_label_pomodoro.set_pos(60, 5)
screen_pomodoro_label_pomodoro.set_size(210, 17)
# Set style for screen_pomodoro_label_pomodoro, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_pomodoro_label_pomodoro.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_pomodoro_label_pomodoro.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_pomodoro_label_pomodoro.set_style_text_color(lv.color_hex(0x274a61), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_pomodoro_label_pomodoro.set_style_text_font(test_font("SourceHanSansSC_Medium", 14), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_pomodoro_label_pomodoro.set_style_text_opa(229, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_pomodoro_label_pomodoro.set_style_text_letter_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_pomodoro_label_pomodoro.set_style_text_line_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_pomodoro_label_pomodoro.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_pomodoro_label_pomodoro.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_pomodoro_label_pomodoro.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_pomodoro_label_pomodoro.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_pomodoro_label_pomodoro.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_pomodoro_label_pomodoro.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_pomodoro_label_pomodoro.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_pomodoro_btn_stopwatch
screen_pomodoro_btn_stopwatch = lv.btn(screen_pomodoro)
screen_pomodoro_btn_stopwatch_label = lv.label(screen_pomodoro_btn_stopwatch)
screen_pomodoro_btn_stopwatch_label.set_text("")
screen_pomodoro_btn_stopwatch_label.set_long_mode(lv.label.LONG.WRAP)
screen_pomodoro_btn_stopwatch_label.set_width(lv.pct(100))
screen_pomodoro_btn_stopwatch_label.align(lv.ALIGN.CENTER, 0, 0)
screen_pomodoro_btn_stopwatch.set_style_pad_all(0, lv.STATE.DEFAULT)
screen_pomodoro_btn_stopwatch.set_pos(34, 87)
screen_pomodoro_btn_stopwatch.set_size(125, 15)
# Set style for screen_pomodoro_btn_stopwatch, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_pomodoro_btn_stopwatch.set_style_bg_opa(40, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_pomodoro_btn_stopwatch.set_style_bg_color(lv.color_hex(0xc3b5b5), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_pomodoro_btn_stopwatch.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_pomodoro_btn_stopwatch.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_pomodoro_btn_stopwatch.set_style_radius(5, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_pomodoro_btn_stopwatch.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_pomodoro_btn_stopwatch.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_pomodoro_btn_stopwatch.set_style_text_font(test_font("AdobeFangsongStdRegular", 14), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_pomodoro_btn_stopwatch.set_style_text_opa(221, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_pomodoro_btn_stopwatch.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
# Set style for screen_pomodoro_btn_stopwatch, Part: lv.PART.MAIN, State: lv.STATE.FOCUSED.
screen_pomodoro_btn_stopwatch.set_style_bg_opa(28, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_pomodoro_btn_stopwatch.set_style_bg_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.FOCUSED)
screen_pomodoro_btn_stopwatch.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_pomodoro_btn_stopwatch.set_style_border_width(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_pomodoro_btn_stopwatch.set_style_radius(5, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_pomodoro_btn_stopwatch.set_style_shadow_width(5, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_pomodoro_btn_stopwatch.set_style_shadow_color(lv.color_hex(0x2f67a4), lv.PART.MAIN|lv.STATE.FOCUSED)
screen_pomodoro_btn_stopwatch.set_style_shadow_opa(255, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_pomodoro_btn_stopwatch.set_style_shadow_spread(1, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_pomodoro_btn_stopwatch.set_style_shadow_ofs_x(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_pomodoro_btn_stopwatch.set_style_shadow_ofs_y(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_pomodoro_btn_stopwatch.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.FOCUSED)
screen_pomodoro_btn_stopwatch.set_style_text_font(test_font("SourceHanSerifSC_Regular", 13), lv.PART.MAIN|lv.STATE.FOCUSED)
screen_pomodoro_btn_stopwatch.set_style_text_opa(122, lv.PART.MAIN|lv.STATE.FOCUSED)

# Create screen_pomodoro_btn_countdown
screen_pomodoro_btn_countdown = lv.btn(screen_pomodoro)
screen_pomodoro_btn_countdown_label = lv.label(screen_pomodoro_btn_countdown)
screen_pomodoro_btn_countdown_label.set_text("")
screen_pomodoro_btn_countdown_label.set_long_mode(lv.label.LONG.WRAP)
screen_pomodoro_btn_countdown_label.set_width(lv.pct(100))
screen_pomodoro_btn_countdown_label.align(lv.ALIGN.CENTER, 0, 0)
screen_pomodoro_btn_countdown.set_style_pad_all(0, lv.STATE.DEFAULT)
screen_pomodoro_btn_countdown.set_pos(163, 87)
screen_pomodoro_btn_countdown.set_size(125, 15)
# Set style for screen_pomodoro_btn_countdown, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_pomodoro_btn_countdown.set_style_bg_opa(38, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_pomodoro_btn_countdown.set_style_bg_color(lv.color_hex(0xbebdbd), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_pomodoro_btn_countdown.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_pomodoro_btn_countdown.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_pomodoro_btn_countdown.set_style_radius(5, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_pomodoro_btn_countdown.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_pomodoro_btn_countdown.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_pomodoro_btn_countdown.set_style_text_font(test_font("AdobeFangsongStdRegular", 14), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_pomodoro_btn_countdown.set_style_text_opa(226, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_pomodoro_btn_countdown.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
# Set style for screen_pomodoro_btn_countdown, Part: lv.PART.MAIN, State: lv.STATE.FOCUSED.
screen_pomodoro_btn_countdown.set_style_bg_opa(19, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_pomodoro_btn_countdown.set_style_bg_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.FOCUSED)
screen_pomodoro_btn_countdown.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_pomodoro_btn_countdown.set_style_border_width(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_pomodoro_btn_countdown.set_style_radius(5, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_pomodoro_btn_countdown.set_style_shadow_width(5, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_pomodoro_btn_countdown.set_style_shadow_color(lv.color_hex(0x2f67a4), lv.PART.MAIN|lv.STATE.FOCUSED)
screen_pomodoro_btn_countdown.set_style_shadow_opa(255, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_pomodoro_btn_countdown.set_style_shadow_spread(1, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_pomodoro_btn_countdown.set_style_shadow_ofs_x(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_pomodoro_btn_countdown.set_style_shadow_ofs_y(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_pomodoro_btn_countdown.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.FOCUSED)
screen_pomodoro_btn_countdown.set_style_text_font(test_font("SourceHanSerifSC_Regular", 13), lv.PART.MAIN|lv.STATE.FOCUSED)
screen_pomodoro_btn_countdown.set_style_text_opa(129, lv.PART.MAIN|lv.STATE.FOCUSED)

# Create screen_pomodoro_btn_back
screen_pomodoro_btn_back = lv.btn(screen_pomodoro)
screen_pomodoro_btn_back_label = lv.label(screen_pomodoro_btn_back)
screen_pomodoro_btn_back_label.set_text("")
screen_pomodoro_btn_back_label.set_long_mode(lv.label.LONG.WRAP)
screen_pomodoro_btn_back_label.set_width(lv.pct(100))
screen_pomodoro_btn_back_label.align(lv.ALIGN.CENTER, 0, 0)
screen_pomodoro_btn_back.set_style_pad_all(0, lv.STATE.DEFAULT)
screen_pomodoro_btn_back.set_pos(7, 6)
screen_pomodoro_btn_back.set_size(23, 17)
# Set style for screen_pomodoro_btn_back, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_pomodoro_btn_back.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_pomodoro_btn_back.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_pomodoro_btn_back.set_style_radius(5, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_pomodoro_btn_back.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_pomodoro_btn_back.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_pomodoro_btn_back.set_style_text_font(test_font("montserratMedium", 16), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_pomodoro_btn_back.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_pomodoro_btn_back.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
# Set style for screen_pomodoro_btn_back, Part: lv.PART.MAIN, State: lv.STATE.FOCUSED.
screen_pomodoro_btn_back.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_pomodoro_btn_back.set_style_border_width(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_pomodoro_btn_back.set_style_radius(5, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_pomodoro_btn_back.set_style_shadow_width(5, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_pomodoro_btn_back.set_style_shadow_color(lv.color_hex(0x2f67a4), lv.PART.MAIN|lv.STATE.FOCUSED)
screen_pomodoro_btn_back.set_style_shadow_opa(255, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_pomodoro_btn_back.set_style_shadow_spread(1, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_pomodoro_btn_back.set_style_shadow_ofs_x(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_pomodoro_btn_back.set_style_shadow_ofs_y(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_pomodoro_btn_back.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.FOCUSED)
screen_pomodoro_btn_back.set_style_text_font(test_font("montserratMedium", 16), lv.PART.MAIN|lv.STATE.FOCUSED)
screen_pomodoro_btn_back.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.FOCUSED)

# Create screen_pomodoro_btn_start
screen_pomodoro_btn_start = lv.btn(screen_pomodoro)
screen_pomodoro_btn_start_label = lv.label(screen_pomodoro_btn_start)
screen_pomodoro_btn_start_label.set_text("")
screen_pomodoro_btn_start_label.set_long_mode(lv.label.LONG.WRAP)
screen_pomodoro_btn_start_label.set_width(lv.pct(100))
screen_pomodoro_btn_start_label.align(lv.ALIGN.CENTER, 0, 0)
screen_pomodoro_btn_start.set_style_pad_all(0, lv.STATE.DEFAULT)
screen_pomodoro_btn_start.set_pos(94, 144)
screen_pomodoro_btn_start.set_size(66, 16)
# Set style for screen_pomodoro_btn_start, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_pomodoro_btn_start.set_style_bg_opa(34, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_pomodoro_btn_start.set_style_bg_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_pomodoro_btn_start.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_pomodoro_btn_start.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_pomodoro_btn_start.set_style_radius(10, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_pomodoro_btn_start.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_pomodoro_btn_start.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_pomodoro_btn_start.set_style_text_font(test_font("AdobeFangsongStdRegular", 16), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_pomodoro_btn_start.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_pomodoro_btn_start.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
# Set style for screen_pomodoro_btn_start, Part: lv.PART.MAIN, State: lv.STATE.FOCUSED.
screen_pomodoro_btn_start.set_style_bg_opa(34, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_pomodoro_btn_start.set_style_bg_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.FOCUSED)
screen_pomodoro_btn_start.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_pomodoro_btn_start.set_style_border_width(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_pomodoro_btn_start.set_style_radius(10, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_pomodoro_btn_start.set_style_shadow_width(10, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_pomodoro_btn_start.set_style_shadow_color(lv.color_hex(0x2f67a4), lv.PART.MAIN|lv.STATE.FOCUSED)
screen_pomodoro_btn_start.set_style_shadow_opa(255, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_pomodoro_btn_start.set_style_shadow_spread(1, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_pomodoro_btn_start.set_style_shadow_ofs_x(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_pomodoro_btn_start.set_style_shadow_ofs_y(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_pomodoro_btn_start.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.FOCUSED)
screen_pomodoro_btn_start.set_style_text_font(test_font("SourceHanSerifSC_Regular", 16), lv.PART.MAIN|lv.STATE.FOCUSED)
screen_pomodoro_btn_start.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.FOCUSED)

# Create screen_pomodoro_btn_reset
screen_pomodoro_btn_reset = lv.btn(screen_pomodoro)
screen_pomodoro_btn_reset_label = lv.label(screen_pomodoro_btn_reset)
screen_pomodoro_btn_reset_label.set_text("")
screen_pomodoro_btn_reset_label.set_long_mode(lv.label.LONG.WRAP)
screen_pomodoro_btn_reset_label.set_width(lv.pct(100))
screen_pomodoro_btn_reset_label.align(lv.ALIGN.CENTER, 0, 0)
screen_pomodoro_btn_reset.set_style_pad_all(0, lv.STATE.DEFAULT)
screen_pomodoro_btn_reset.set_pos(166, 144)
screen_pomodoro_btn_reset.set_size(56, 14)
# Set style for screen_pomodoro_btn_reset, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_pomodoro_btn_reset.set_style_bg_opa(34, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_pomodoro_btn_reset.set_style_bg_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_pomodoro_btn_reset.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_pomodoro_btn_reset.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_pomodoro_btn_reset.set_style_radius(10, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_pomodoro_btn_reset.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_pomodoro_btn_reset.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_pomodoro_btn_reset.set_style_text_font(test_font("AdobeFangsongStdRegular", 16), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_pomodoro_btn_reset.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_pomodoro_btn_reset.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
# Set style for screen_pomodoro_btn_reset, Part: lv.PART.MAIN, State: lv.STATE.FOCUSED.
screen_pomodoro_btn_reset.set_style_bg_opa(43, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_pomodoro_btn_reset.set_style_bg_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.FOCUSED)
screen_pomodoro_btn_reset.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_pomodoro_btn_reset.set_style_border_width(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_pomodoro_btn_reset.set_style_radius(10, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_pomodoro_btn_reset.set_style_shadow_width(10, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_pomodoro_btn_reset.set_style_shadow_color(lv.color_hex(0x2f67a4), lv.PART.MAIN|lv.STATE.FOCUSED)
screen_pomodoro_btn_reset.set_style_shadow_opa(255, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_pomodoro_btn_reset.set_style_shadow_spread(1, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_pomodoro_btn_reset.set_style_shadow_ofs_x(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_pomodoro_btn_reset.set_style_shadow_ofs_y(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_pomodoro_btn_reset.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.FOCUSED)
screen_pomodoro_btn_reset.set_style_text_font(test_font("SourceHanSerifSC_Regular", 16), lv.PART.MAIN|lv.STATE.FOCUSED)
screen_pomodoro_btn_reset.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.FOCUSED)

# Create screen_pomodoro_btn_addnum
screen_pomodoro_btn_addnum = lv.btn(screen_pomodoro)
screen_pomodoro_btn_addnum_label = lv.label(screen_pomodoro_btn_addnum)
screen_pomodoro_btn_addnum_label.set_text("1")
screen_pomodoro_btn_addnum_label.set_long_mode(lv.label.LONG.WRAP)
screen_pomodoro_btn_addnum_label.set_width(lv.pct(100))
screen_pomodoro_btn_addnum_label.align(lv.ALIGN.CENTER, 0, 0)
screen_pomodoro_btn_addnum.set_style_pad_all(0, lv.STATE.DEFAULT)
screen_pomodoro_btn_addnum.set_pos(133, 113)
screen_pomodoro_btn_addnum.set_size(55, 20)
# Set style for screen_pomodoro_btn_addnum, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_pomodoro_btn_addnum.set_style_bg_opa(25, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_pomodoro_btn_addnum.set_style_bg_color(lv.color_hex(0xe2ecf4), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_pomodoro_btn_addnum.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_pomodoro_btn_addnum.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_pomodoro_btn_addnum.set_style_radius(5, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_pomodoro_btn_addnum.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_pomodoro_btn_addnum.set_style_text_color(lv.color_hex(0x2f67a4), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_pomodoro_btn_addnum.set_style_text_font(test_font("montserratMedium", 16), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_pomodoro_btn_addnum.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_pomodoro_btn_addnum.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
# Set style for screen_pomodoro_btn_addnum, Part: lv.PART.MAIN, State: lv.STATE.FOCUSED.
screen_pomodoro_btn_addnum.set_style_bg_opa(29, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_pomodoro_btn_addnum.set_style_bg_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.FOCUSED)
screen_pomodoro_btn_addnum.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_pomodoro_btn_addnum.set_style_border_width(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_pomodoro_btn_addnum.set_style_radius(5, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_pomodoro_btn_addnum.set_style_shadow_width(5, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_pomodoro_btn_addnum.set_style_shadow_color(lv.color_hex(0x2f67a4), lv.PART.MAIN|lv.STATE.FOCUSED)
screen_pomodoro_btn_addnum.set_style_shadow_opa(255, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_pomodoro_btn_addnum.set_style_shadow_spread(1, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_pomodoro_btn_addnum.set_style_shadow_ofs_x(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_pomodoro_btn_addnum.set_style_shadow_ofs_y(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_pomodoro_btn_addnum.set_style_text_color(lv.color_hex(0x2f67a4), lv.PART.MAIN|lv.STATE.FOCUSED)
screen_pomodoro_btn_addnum.set_style_text_font(test_font("montserratMedium", 16), lv.PART.MAIN|lv.STATE.FOCUSED)
screen_pomodoro_btn_addnum.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.FOCUSED)

# Create screen_pomodoro_img_timeout
screen_pomodoro_img_timeout = lv.img(screen_pomodoro)
screen_pomodoro_img_timeout.set_src("B:MicroPython/_AAA_fanqie_xiangling_alpha_288x146.bin")
screen_pomodoro_img_timeout.add_flag(lv.obj.FLAG.CLICKABLE)
screen_pomodoro_img_timeout.set_pivot(50,50)
screen_pomodoro_img_timeout.set_angle(0)
screen_pomodoro_img_timeout.set_pos(20, 22)
screen_pomodoro_img_timeout.set_size(288, 146)
# Set style for screen_pomodoro_img_timeout, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_pomodoro_img_timeout.set_style_img_recolor_opa(10, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_pomodoro_img_timeout.set_style_img_recolor(lv.color_hex(0x000000), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_pomodoro_img_timeout.set_style_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_pomodoro_img_timeout.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_pomodoro_img_timeout.set_style_clip_corner(True, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_pomodoro_btn_complete
screen_pomodoro_btn_complete = lv.btn(screen_pomodoro)
screen_pomodoro_btn_complete_label = lv.label(screen_pomodoro_btn_complete)
screen_pomodoro_btn_complete_label.set_text("")
screen_pomodoro_btn_complete_label.set_long_mode(lv.label.LONG.WRAP)
screen_pomodoro_btn_complete_label.set_width(lv.pct(100))
screen_pomodoro_btn_complete_label.align(lv.ALIGN.CENTER, 0, 0)
screen_pomodoro_btn_complete.set_style_pad_all(0, lv.STATE.DEFAULT)
screen_pomodoro_btn_complete.set_pos(34, 123)
screen_pomodoro_btn_complete.set_size(121, 18)
# Set style for screen_pomodoro_btn_complete, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_pomodoro_btn_complete.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_pomodoro_btn_complete.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_pomodoro_btn_complete.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_pomodoro_btn_complete.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_pomodoro_btn_complete.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_pomodoro_btn_complete.set_style_text_font(test_font("montserratMedium", 16), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_pomodoro_btn_complete.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_pomodoro_btn_complete.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
# Set style for screen_pomodoro_btn_complete, Part: lv.PART.MAIN, State: lv.STATE.FOCUSED.
screen_pomodoro_btn_complete.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_pomodoro_btn_complete.set_style_border_width(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_pomodoro_btn_complete.set_style_radius(5, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_pomodoro_btn_complete.set_style_shadow_width(5, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_pomodoro_btn_complete.set_style_shadow_color(lv.color_hex(0x2f67a4), lv.PART.MAIN|lv.STATE.FOCUSED)
screen_pomodoro_btn_complete.set_style_shadow_opa(255, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_pomodoro_btn_complete.set_style_shadow_spread(3, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_pomodoro_btn_complete.set_style_shadow_ofs_x(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_pomodoro_btn_complete.set_style_shadow_ofs_y(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_pomodoro_btn_complete.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.FOCUSED)
screen_pomodoro_btn_complete.set_style_text_font(test_font("montserratMedium", 16), lv.PART.MAIN|lv.STATE.FOCUSED)
screen_pomodoro_btn_complete.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.FOCUSED)

# Set style for screen_pomodoro_btn_complete, Part: lv.PART.MAIN, State: lv.STATE.DISABLED.
screen_pomodoro_btn_complete.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DISABLED)
screen_pomodoro_btn_complete.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DISABLED)
screen_pomodoro_btn_complete.set_style_radius(0, lv.PART.MAIN|lv.STATE.DISABLED)
screen_pomodoro_btn_complete.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DISABLED)
screen_pomodoro_btn_complete.set_style_text_color(lv.color_hex(0x000000), lv.PART.MAIN|lv.STATE.DISABLED)
screen_pomodoro_btn_complete.set_style_text_font(test_font("montserratMedium", 12), lv.PART.MAIN|lv.STATE.DISABLED)
screen_pomodoro_btn_complete.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DISABLED)

# Create screen_pomodoro_btn_continue
screen_pomodoro_btn_continue = lv.btn(screen_pomodoro)
screen_pomodoro_btn_continue_label = lv.label(screen_pomodoro_btn_continue)
screen_pomodoro_btn_continue_label.set_text("")
screen_pomodoro_btn_continue_label.set_long_mode(lv.label.LONG.WRAP)
screen_pomodoro_btn_continue_label.set_width(lv.pct(100))
screen_pomodoro_btn_continue_label.align(lv.ALIGN.CENTER, 0, 0)
screen_pomodoro_btn_continue.set_style_pad_all(0, lv.STATE.DEFAULT)
screen_pomodoro_btn_continue.set_pos(166, 123)
screen_pomodoro_btn_continue.set_size(121, 18)
# Set style for screen_pomodoro_btn_continue, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_pomodoro_btn_continue.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_pomodoro_btn_continue.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_pomodoro_btn_continue.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_pomodoro_btn_continue.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_pomodoro_btn_continue.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_pomodoro_btn_continue.set_style_text_font(test_font("montserratMedium", 16), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_pomodoro_btn_continue.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_pomodoro_btn_continue.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
# Set style for screen_pomodoro_btn_continue, Part: lv.PART.MAIN, State: lv.STATE.FOCUSED.
screen_pomodoro_btn_continue.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_pomodoro_btn_continue.set_style_border_width(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_pomodoro_btn_continue.set_style_radius(5, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_pomodoro_btn_continue.set_style_shadow_width(5, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_pomodoro_btn_continue.set_style_shadow_color(lv.color_hex(0x2f67a4), lv.PART.MAIN|lv.STATE.FOCUSED)
screen_pomodoro_btn_continue.set_style_shadow_opa(255, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_pomodoro_btn_continue.set_style_shadow_spread(3, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_pomodoro_btn_continue.set_style_shadow_ofs_x(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_pomodoro_btn_continue.set_style_shadow_ofs_y(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_pomodoro_btn_continue.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.FOCUSED)
screen_pomodoro_btn_continue.set_style_text_font(test_font("montserratMedium", 16), lv.PART.MAIN|lv.STATE.FOCUSED)
screen_pomodoro_btn_continue.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.FOCUSED)

# Set style for screen_pomodoro_btn_continue, Part: lv.PART.MAIN, State: lv.STATE.DISABLED.
screen_pomodoro_btn_continue.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DISABLED)
screen_pomodoro_btn_continue.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DISABLED)
screen_pomodoro_btn_continue.set_style_radius(0, lv.PART.MAIN|lv.STATE.DISABLED)
screen_pomodoro_btn_continue.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DISABLED)
screen_pomodoro_btn_continue.set_style_text_color(lv.color_hex(0x000000), lv.PART.MAIN|lv.STATE.DISABLED)
screen_pomodoro_btn_continue.set_style_text_font(test_font("montserratMedium", 12), lv.PART.MAIN|lv.STATE.DISABLED)
screen_pomodoro_btn_continue.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DISABLED)

# Create screen_pomodoro_btn_plus
screen_pomodoro_btn_plus = lv.btn(screen_pomodoro)
screen_pomodoro_btn_plus_label = lv.label(screen_pomodoro_btn_plus)
screen_pomodoro_btn_plus_label.set_text("")
screen_pomodoro_btn_plus_label.set_long_mode(lv.label.LONG.WRAP)
screen_pomodoro_btn_plus_label.set_width(lv.pct(100))
screen_pomodoro_btn_plus_label.align(lv.ALIGN.CENTER, 0, 0)
screen_pomodoro_btn_plus.set_style_pad_all(0, lv.STATE.DEFAULT)
screen_pomodoro_btn_plus.set_pos(270, 114)
screen_pomodoro_btn_plus.set_size(24, 18)
# Set style for screen_pomodoro_btn_plus, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_pomodoro_btn_plus.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_pomodoro_btn_plus.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_pomodoro_btn_plus.set_style_radius(5, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_pomodoro_btn_plus.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_pomodoro_btn_plus.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_pomodoro_btn_plus.set_style_text_font(test_font("montserratMedium", 25), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_pomodoro_btn_plus.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_pomodoro_btn_plus.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
# Set style for screen_pomodoro_btn_plus, Part: lv.PART.MAIN, State: lv.STATE.FOCUSED.
screen_pomodoro_btn_plus.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_pomodoro_btn_plus.set_style_border_width(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_pomodoro_btn_plus.set_style_radius(5, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_pomodoro_btn_plus.set_style_shadow_width(10, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_pomodoro_btn_plus.set_style_shadow_color(lv.color_hex(0x2f67a4), lv.PART.MAIN|lv.STATE.FOCUSED)
screen_pomodoro_btn_plus.set_style_shadow_opa(255, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_pomodoro_btn_plus.set_style_shadow_spread(1, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_pomodoro_btn_plus.set_style_shadow_ofs_x(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_pomodoro_btn_plus.set_style_shadow_ofs_y(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_pomodoro_btn_plus.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.FOCUSED)
screen_pomodoro_btn_plus.set_style_text_font(test_font("montserratMedium", 25), lv.PART.MAIN|lv.STATE.FOCUSED)
screen_pomodoro_btn_plus.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.FOCUSED)

# Create screen_pomodoro_btn_minus
screen_pomodoro_btn_minus = lv.btn(screen_pomodoro)
screen_pomodoro_btn_minus_label = lv.label(screen_pomodoro_btn_minus)
screen_pomodoro_btn_minus_label.set_text("")
screen_pomodoro_btn_minus_label.set_long_mode(lv.label.LONG.WRAP)
screen_pomodoro_btn_minus_label.set_width(lv.pct(100))
screen_pomodoro_btn_minus_label.align(lv.ALIGN.CENTER, 0, 0)
screen_pomodoro_btn_minus.set_style_pad_all(0, lv.STATE.DEFAULT)
screen_pomodoro_btn_minus.set_pos(26, 114)
screen_pomodoro_btn_minus.set_size(24, 18)
# Set style for screen_pomodoro_btn_minus, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_pomodoro_btn_minus.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_pomodoro_btn_minus.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_pomodoro_btn_minus.set_style_radius(5, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_pomodoro_btn_minus.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_pomodoro_btn_minus.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_pomodoro_btn_minus.set_style_text_font(test_font("montserratMedium", 25), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_pomodoro_btn_minus.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_pomodoro_btn_minus.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
# Set style for screen_pomodoro_btn_minus, Part: lv.PART.MAIN, State: lv.STATE.FOCUSED.
screen_pomodoro_btn_minus.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_pomodoro_btn_minus.set_style_border_width(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_pomodoro_btn_minus.set_style_radius(5, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_pomodoro_btn_minus.set_style_shadow_width(5, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_pomodoro_btn_minus.set_style_shadow_color(lv.color_hex(0x2f67a4), lv.PART.MAIN|lv.STATE.FOCUSED)
screen_pomodoro_btn_minus.set_style_shadow_opa(255, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_pomodoro_btn_minus.set_style_shadow_spread(1, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_pomodoro_btn_minus.set_style_shadow_ofs_x(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_pomodoro_btn_minus.set_style_shadow_ofs_y(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_pomodoro_btn_minus.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.FOCUSED)
screen_pomodoro_btn_minus.set_style_text_font(test_font("montserratMedium", 25), lv.PART.MAIN|lv.STATE.FOCUSED)
screen_pomodoro_btn_minus.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.FOCUSED)

# Create screen_pomodoro_img_countdown
screen_pomodoro_img_countdown = lv.img(screen_pomodoro)
screen_pomodoro_img_countdown.set_src("B:MicroPython/_AAA_daojishi_alpha_15x15.bin")
screen_pomodoro_img_countdown.add_flag(lv.obj.FLAG.CLICKABLE)
screen_pomodoro_img_countdown.set_pivot(50,50)
screen_pomodoro_img_countdown.set_angle(0)
screen_pomodoro_img_countdown.set_pos(252, 47)
screen_pomodoro_img_countdown.set_size(15, 15)
# Set style for screen_pomodoro_img_countdown, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_pomodoro_img_countdown.set_style_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_pomodoro_img_countdown.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_pomodoro_img_countdown.set_style_clip_corner(True, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_pomodoro_img_stopwatch
screen_pomodoro_img_stopwatch = lv.img(screen_pomodoro)
screen_pomodoro_img_stopwatch.set_src("B:MicroPython/_AAA_miaobiao_alpha_15x15.bin")
screen_pomodoro_img_stopwatch.add_flag(lv.obj.FLAG.CLICKABLE)
screen_pomodoro_img_stopwatch.set_pivot(50,50)
screen_pomodoro_img_stopwatch.set_angle(0)
screen_pomodoro_img_stopwatch.set_pos(252, 47)
screen_pomodoro_img_stopwatch.set_size(15, 15)
# Set style for screen_pomodoro_img_stopwatch, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_pomodoro_img_stopwatch.set_style_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_pomodoro_img_stopwatch.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_pomodoro_img_stopwatch.set_style_clip_corner(True, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_pomodoro_img_pause
screen_pomodoro_img_pause = lv.img(screen_pomodoro)
screen_pomodoro_img_pause.set_src("B:MicroPython/_AAA_stop_alpha_69x21.bin")
screen_pomodoro_img_pause.add_flag(lv.obj.FLAG.CLICKABLE)
screen_pomodoro_img_pause.set_pivot(50,50)
screen_pomodoro_img_pause.set_angle(0)
screen_pomodoro_img_pause.set_pos(95, 143)
screen_pomodoro_img_pause.set_size(69, 21)
# Set style for screen_pomodoro_img_pause, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_pomodoro_img_pause.set_style_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_pomodoro_img_pause.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_pomodoro_img_pause.set_style_clip_corner(True, lv.PART.MAIN|lv.STATE.DEFAULT)

screen_pomodoro.update_layout()
# Create screen_setting
screen_setting = lv.obj()
g_kb_screen_setting = lv.keyboard(screen_setting)
g_kb_screen_setting.add_event_cb(lambda e: ta_event_cb(e, g_kb_screen_setting), lv.EVENT.ALL, None)
g_kb_screen_setting.add_flag(lv.obj.FLAG.HIDDEN)
g_kb_screen_setting.set_style_text_font(test_font("Acme_Regular", 12), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_setting.set_size(320, 170)
screen_setting.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
# Set style for screen_setting, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_setting.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_setting.set_style_bg_img_src("B:MicroPython/_AAA_reppo_setting_bg_320x170.bin", lv.PART.MAIN|lv.STATE.DEFAULT)
screen_setting.set_style_bg_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_setting.set_style_bg_img_recolor(lv.color_hex(0x7987a6), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_setting.set_style_bg_img_recolor_opa(21, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_setting_btn_back
screen_setting_btn_back = lv.btn(screen_setting)
screen_setting_btn_back_label = lv.label(screen_setting_btn_back)
screen_setting_btn_back_label.set_text("")
screen_setting_btn_back_label.set_long_mode(lv.label.LONG.WRAP)
screen_setting_btn_back_label.set_width(lv.pct(100))
screen_setting_btn_back_label.align(lv.ALIGN.CENTER, 0, 0)
screen_setting_btn_back.set_style_pad_all(0, lv.STATE.DEFAULT)
screen_setting_btn_back.set_pos(7, 8)
screen_setting_btn_back.set_size(17, 17)
# Set style for screen_setting_btn_back, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_setting_btn_back.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_setting_btn_back.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_setting_btn_back.set_style_radius(5, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_setting_btn_back.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_setting_btn_back.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_setting_btn_back.set_style_text_font(test_font("SourceHanSansSC_Medium", 14), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_setting_btn_back.set_style_text_opa(192, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_setting_btn_back.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
# Set style for screen_setting_btn_back, Part: lv.PART.MAIN, State: lv.STATE.FOCUSED.
screen_setting_btn_back.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_setting_btn_back.set_style_border_width(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_setting_btn_back.set_style_radius(5, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_setting_btn_back.set_style_shadow_width(5, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_setting_btn_back.set_style_shadow_color(lv.color_hex(0x274a6e), lv.PART.MAIN|lv.STATE.FOCUSED)
screen_setting_btn_back.set_style_shadow_opa(255, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_setting_btn_back.set_style_shadow_spread(1, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_setting_btn_back.set_style_shadow_ofs_x(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_setting_btn_back.set_style_shadow_ofs_y(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_setting_btn_back.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.FOCUSED)
screen_setting_btn_back.set_style_text_font(test_font("SourceHanSansSC_Medium", 14), lv.PART.MAIN|lv.STATE.FOCUSED)
screen_setting_btn_back.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.FOCUSED)

# Create screen_setting_btn_general
screen_setting_btn_general = lv.btn(screen_setting)
screen_setting_btn_general_label = lv.label(screen_setting_btn_general)
screen_setting_btn_general_label.set_text("General")
screen_setting_btn_general_label.set_long_mode(lv.label.LONG.WRAP)
screen_setting_btn_general_label.set_width(lv.pct(100))
screen_setting_btn_general_label.align(lv.ALIGN.LEFT_MID, 0, 0)
screen_setting_btn_general.set_style_pad_all(0, lv.STATE.DEFAULT)
screen_setting_btn_general.set_pos(19, 135)
screen_setting_btn_general.set_size(95, 16)
# Set style for screen_setting_btn_general, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_setting_btn_general.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_setting_btn_general.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_setting_btn_general.set_style_radius(5, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_setting_btn_general.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_setting_btn_general.set_style_text_color(lv.color_hex(0x274a6e), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_setting_btn_general.set_style_text_font(test_font("SourceHanSansSC_Medium", 14), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_setting_btn_general.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_setting_btn_general.set_style_text_align(lv.TEXT_ALIGN.LEFT, lv.PART.MAIN|lv.STATE.DEFAULT)
# Set style for screen_setting_btn_general, Part: lv.PART.MAIN, State: lv.STATE.FOCUSED.
screen_setting_btn_general.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_setting_btn_general.set_style_bg_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.FOCUSED)
screen_setting_btn_general.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_setting_btn_general.set_style_border_width(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_setting_btn_general.set_style_radius(5, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_setting_btn_general.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_setting_btn_general.set_style_text_color(lv.color_hex(0x274a6e), lv.PART.MAIN|lv.STATE.FOCUSED)
screen_setting_btn_general.set_style_text_font(test_font("SourceHanSansSC_Medium", 14), lv.PART.MAIN|lv.STATE.FOCUSED)
screen_setting_btn_general.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.FOCUSED)

# Create screen_setting_btn_brightness
screen_setting_btn_brightness = lv.btn(screen_setting)
screen_setting_btn_brightness_label = lv.label(screen_setting_btn_brightness)
screen_setting_btn_brightness_label.set_text("Brightness")
screen_setting_btn_brightness_label.set_long_mode(lv.label.LONG.WRAP)
screen_setting_btn_brightness_label.set_width(lv.pct(100))
screen_setting_btn_brightness_label.align(lv.ALIGN.LEFT_MID, 0, 0)
screen_setting_btn_brightness.set_style_pad_all(0, lv.STATE.DEFAULT)
screen_setting_btn_brightness.set_pos(19, 110)
screen_setting_btn_brightness.set_size(95, 16)
# Set style for screen_setting_btn_brightness, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_setting_btn_brightness.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_setting_btn_brightness.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_setting_btn_brightness.set_style_radius(5, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_setting_btn_brightness.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_setting_btn_brightness.set_style_text_color(lv.color_hex(0x274a6e), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_setting_btn_brightness.set_style_text_font(test_font("SourceHanSansSC_Medium", 14), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_setting_btn_brightness.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_setting_btn_brightness.set_style_text_align(lv.TEXT_ALIGN.LEFT, lv.PART.MAIN|lv.STATE.DEFAULT)
# Set style for screen_setting_btn_brightness, Part: lv.PART.MAIN, State: lv.STATE.FOCUSED.
screen_setting_btn_brightness.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_setting_btn_brightness.set_style_bg_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.FOCUSED)
screen_setting_btn_brightness.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_setting_btn_brightness.set_style_border_width(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_setting_btn_brightness.set_style_radius(5, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_setting_btn_brightness.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_setting_btn_brightness.set_style_text_color(lv.color_hex(0x274a6e), lv.PART.MAIN|lv.STATE.FOCUSED)
screen_setting_btn_brightness.set_style_text_font(test_font("SourceHanSansSC_Medium", 14), lv.PART.MAIN|lv.STATE.FOCUSED)
screen_setting_btn_brightness.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.FOCUSED)

# Create screen_setting_btn_volume
screen_setting_btn_volume = lv.btn(screen_setting)
screen_setting_btn_volume_label = lv.label(screen_setting_btn_volume)
screen_setting_btn_volume_label.set_text("Volume")
screen_setting_btn_volume_label.set_long_mode(lv.label.LONG.WRAP)
screen_setting_btn_volume_label.set_width(lv.pct(100))
screen_setting_btn_volume_label.align(lv.ALIGN.LEFT_MID, 0, 0)
screen_setting_btn_volume.set_style_pad_all(0, lv.STATE.DEFAULT)
screen_setting_btn_volume.set_pos(19, 85)
screen_setting_btn_volume.set_size(95, 16)
# Set style for screen_setting_btn_volume, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_setting_btn_volume.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_setting_btn_volume.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_setting_btn_volume.set_style_radius(5, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_setting_btn_volume.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_setting_btn_volume.set_style_text_color(lv.color_hex(0x274a6e), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_setting_btn_volume.set_style_text_font(test_font("SourceHanSansSC_Medium", 14), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_setting_btn_volume.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_setting_btn_volume.set_style_text_align(lv.TEXT_ALIGN.LEFT, lv.PART.MAIN|lv.STATE.DEFAULT)
# Set style for screen_setting_btn_volume, Part: lv.PART.MAIN, State: lv.STATE.FOCUSED.
screen_setting_btn_volume.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_setting_btn_volume.set_style_bg_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.FOCUSED)
screen_setting_btn_volume.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_setting_btn_volume.set_style_border_width(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_setting_btn_volume.set_style_radius(5, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_setting_btn_volume.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_setting_btn_volume.set_style_text_color(lv.color_hex(0x274a6e), lv.PART.MAIN|lv.STATE.FOCUSED)
screen_setting_btn_volume.set_style_text_font(test_font("SourceHanSansSC_Medium", 14), lv.PART.MAIN|lv.STATE.FOCUSED)
screen_setting_btn_volume.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.FOCUSED)

# Create screen_setting_btn_ble
screen_setting_btn_ble = lv.btn(screen_setting)
screen_setting_btn_ble_label = lv.label(screen_setting_btn_ble)
screen_setting_btn_ble_label.set_text("BlueTooth")
screen_setting_btn_ble_label.set_long_mode(lv.label.LONG.WRAP)
screen_setting_btn_ble_label.set_width(lv.pct(100))
screen_setting_btn_ble_label.align(lv.ALIGN.LEFT_MID, 0, 0)
screen_setting_btn_ble.set_style_pad_all(0, lv.STATE.DEFAULT)
screen_setting_btn_ble.set_pos(19, 60)
screen_setting_btn_ble.set_size(95, 16)
# Set style for screen_setting_btn_ble, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_setting_btn_ble.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_setting_btn_ble.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_setting_btn_ble.set_style_radius(5, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_setting_btn_ble.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_setting_btn_ble.set_style_text_color(lv.color_hex(0x274a6e), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_setting_btn_ble.set_style_text_font(test_font("SourceHanSansSC_Medium", 14), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_setting_btn_ble.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_setting_btn_ble.set_style_text_align(lv.TEXT_ALIGN.LEFT, lv.PART.MAIN|lv.STATE.DEFAULT)
# Set style for screen_setting_btn_ble, Part: lv.PART.MAIN, State: lv.STATE.FOCUSED.
screen_setting_btn_ble.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_setting_btn_ble.set_style_bg_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.FOCUSED)
screen_setting_btn_ble.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_setting_btn_ble.set_style_border_width(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_setting_btn_ble.set_style_radius(5, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_setting_btn_ble.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_setting_btn_ble.set_style_text_color(lv.color_hex(0x274a6e), lv.PART.MAIN|lv.STATE.FOCUSED)
screen_setting_btn_ble.set_style_text_font(test_font("SourceHanSansSC_Medium", 14), lv.PART.MAIN|lv.STATE.FOCUSED)
screen_setting_btn_ble.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.FOCUSED)

# Create screen_setting_btn_wifi
screen_setting_btn_wifi = lv.btn(screen_setting)
screen_setting_btn_wifi_label = lv.label(screen_setting_btn_wifi)
screen_setting_btn_wifi_label.set_text("WIFI")
screen_setting_btn_wifi_label.set_long_mode(lv.label.LONG.WRAP)
screen_setting_btn_wifi_label.set_width(lv.pct(100))
screen_setting_btn_wifi_label.align(lv.ALIGN.LEFT_MID, 0, 0)
screen_setting_btn_wifi.set_style_pad_all(0, lv.STATE.DEFAULT)
screen_setting_btn_wifi.set_pos(19, 35)
screen_setting_btn_wifi.set_size(95, 16)
# Set style for screen_setting_btn_wifi, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_setting_btn_wifi.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_setting_btn_wifi.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_setting_btn_wifi.set_style_radius(5, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_setting_btn_wifi.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_setting_btn_wifi.set_style_text_color(lv.color_hex(0x274A6E), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_setting_btn_wifi.set_style_text_font(test_font("SourceHanSansSC_Medium", 14), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_setting_btn_wifi.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_setting_btn_wifi.set_style_text_align(lv.TEXT_ALIGN.LEFT, lv.PART.MAIN|lv.STATE.DEFAULT)
# Set style for screen_setting_btn_wifi, Part: lv.PART.MAIN, State: lv.STATE.FOCUSED.
screen_setting_btn_wifi.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_setting_btn_wifi.set_style_bg_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.FOCUSED)
screen_setting_btn_wifi.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_setting_btn_wifi.set_style_border_width(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_setting_btn_wifi.set_style_radius(5, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_setting_btn_wifi.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_setting_btn_wifi.set_style_text_color(lv.color_hex(0x274a6e), lv.PART.MAIN|lv.STATE.FOCUSED)
screen_setting_btn_wifi.set_style_text_font(test_font("SourceHanSansSC_Medium", 14), lv.PART.MAIN|lv.STATE.FOCUSED)
screen_setting_btn_wifi.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.FOCUSED)
# Set style for screen_setting_btn_wifi, Part: lv.PART.MAIN, State: lv.STATE.PRESSED.
screen_setting_btn_wifi.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.PRESSED)
screen_setting_btn_wifi.set_style_border_width(0, lv.PART.MAIN|lv.STATE.PRESSED)
screen_setting_btn_wifi.set_style_radius(5, lv.PART.MAIN|lv.STATE.PRESSED)
screen_setting_btn_wifi.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.PRESSED)
screen_setting_btn_wifi.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.PRESSED)
screen_setting_btn_wifi.set_style_text_font(test_font("montserratMedium", 16), lv.PART.MAIN|lv.STATE.PRESSED)
screen_setting_btn_wifi.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.PRESSED)

# Create screen_setting_img_volume_open
screen_setting_img_volume_open = lv.img(screen_setting)
screen_setting_img_volume_open.set_src("B:MicroPython/_AAA_reppo_volume_kai_icon_alpha_93x75.bin")
screen_setting_img_volume_open.add_flag(lv.obj.FLAG.CLICKABLE)
screen_setting_img_volume_open.set_pivot(50,50)
screen_setting_img_volume_open.set_angle(0)
screen_setting_img_volume_open.set_pos(173, 56)
screen_setting_img_volume_open.set_size(93, 75)
screen_setting_img_volume_open.add_flag(lv.obj.FLAG.HIDDEN)
# Set style for screen_setting_img_volume_open, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_setting_img_volume_open.set_style_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_setting_img_volume_open.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_setting_img_volume_open.set_style_clip_corner(True, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_setting_img_volume_close
screen_setting_img_volume_close = lv.img(screen_setting)
screen_setting_img_volume_close.set_src("B:MicroPython/_AAA_reppo_volume_guan_icon_alpha_93x75.bin")
screen_setting_img_volume_close.add_flag(lv.obj.FLAG.CLICKABLE)
screen_setting_img_volume_close.set_pivot(50,50)
screen_setting_img_volume_close.set_angle(0)
screen_setting_img_volume_close.set_pos(173, 56)
screen_setting_img_volume_close.set_size(93, 75)
screen_setting_img_volume_close.add_flag(lv.obj.FLAG.HIDDEN)
# Set style for screen_setting_img_volume_close, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_setting_img_volume_close.set_style_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_setting_img_volume_close.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_setting_img_volume_close.set_style_clip_corner(True, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_setting_img_wifi_open
screen_setting_img_wifi_open = lv.img(screen_setting)
screen_setting_img_wifi_open.set_src("B:MicroPython/_AAA_clock_wifi_icon_alpha_100x100.bin")
screen_setting_img_wifi_open.add_flag(lv.obj.FLAG.CLICKABLE)
screen_setting_img_wifi_open.set_pivot(50,50)
screen_setting_img_wifi_open.set_angle(0)
screen_setting_img_wifi_open.set_pos(173, 44)
screen_setting_img_wifi_open.set_size(100, 100)
screen_setting_img_wifi_open.add_flag(lv.obj.FLAG.HIDDEN)
# Set style for screen_setting_img_wifi_open, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_setting_img_wifi_open.set_style_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_setting_img_wifi_open.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_setting_img_wifi_open.set_style_clip_corner(True, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_setting_img_wifi_close
screen_setting_img_wifi_close = lv.img(screen_setting)
screen_setting_img_wifi_close.set_src("B:MicroPython/_AAA_clock_wifiduan_icon_alpha_100x100.bin")
screen_setting_img_wifi_close.add_flag(lv.obj.FLAG.CLICKABLE)
screen_setting_img_wifi_close.set_pivot(50,50)
screen_setting_img_wifi_close.set_angle(0)
screen_setting_img_wifi_close.set_pos(173, 41)
screen_setting_img_wifi_close.set_size(100, 100)
screen_setting_img_wifi_close.add_flag(lv.obj.FLAG.HIDDEN)
# Set style for screen_setting_img_wifi_close, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_setting_img_wifi_close.set_style_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_setting_img_wifi_close.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_setting_img_wifi_close.set_style_clip_corner(True, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_setting_img_ble
screen_setting_img_ble = lv.img(screen_setting)
screen_setting_img_ble.set_src("B:MicroPython/_AAA_reppo_bluetooth_icon_alpha_57x92.bin")
screen_setting_img_ble.add_flag(lv.obj.FLAG.CLICKABLE)
screen_setting_img_ble.set_pivot(50,50)
screen_setting_img_ble.set_angle(0)
screen_setting_img_ble.set_pos(191, 44)
screen_setting_img_ble.set_size(57, 92)
screen_setting_img_ble.add_flag(lv.obj.FLAG.HIDDEN)
# Set style for screen_setting_img_ble, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_setting_img_ble.set_style_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_setting_img_ble.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_setting_img_ble.set_style_clip_corner(True, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_setting_img_ble_dis
screen_setting_img_ble_dis = lv.img(screen_setting)
screen_setting_img_ble_dis.set_src("B:MicroPython/_AAA_reppo_ble_dis_alpha_100x100.bin")
screen_setting_img_ble_dis.add_flag(lv.obj.FLAG.CLICKABLE)
screen_setting_img_ble_dis.set_pivot(50,50)
screen_setting_img_ble_dis.set_angle(0)
screen_setting_img_ble_dis.set_pos(173, 41)
screen_setting_img_ble_dis.set_size(100, 100)
screen_setting_img_ble_dis.add_flag(lv.obj.FLAG.HIDDEN)
# Set style for screen_setting_img_ble_dis, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_setting_img_ble_dis.set_style_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_setting_img_ble_dis.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_setting_img_ble_dis.set_style_clip_corner(True, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_setting_img_brightness
screen_setting_img_brightness = lv.img(screen_setting)
screen_setting_img_brightness.set_src("B:MicroPython/_AAA_setting_brightness_alpha_105x105.bin")
screen_setting_img_brightness.add_flag(lv.obj.FLAG.CLICKABLE)
screen_setting_img_brightness.set_pivot(50,50)
screen_setting_img_brightness.set_angle(0)
screen_setting_img_brightness.set_pos(173, 41)
screen_setting_img_brightness.set_size(105, 105)
screen_setting_img_brightness.add_flag(lv.obj.FLAG.HIDDEN)
# Set style for screen_setting_img_brightness, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_setting_img_brightness.set_style_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_setting_img_brightness.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_setting_img_brightness.set_style_clip_corner(True, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_setting_label_about
screen_setting_label_about = lv.label(screen_setting)
screen_setting_label_about.set_text("About Reppo")
screen_setting_label_about.set_long_mode(lv.label.LONG.WRAP)
screen_setting_label_about.set_width(lv.pct(100))
screen_setting_label_about.set_pos(167, 35)
screen_setting_label_about.set_size(109, 18)
# Set style for screen_setting_label_about, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_setting_label_about.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_setting_label_about.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_setting_label_about.set_style_text_color(lv.color_hex(0x274a6e), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_setting_label_about.set_style_text_font(test_font("SourceHanSansSC_Medium", 14), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_setting_label_about.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_setting_label_about.set_style_text_letter_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_setting_label_about.set_style_text_line_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_setting_label_about.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_setting_label_about.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_setting_label_about.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_setting_label_about.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_setting_label_about.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_setting_label_about.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_setting_label_about.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_setting_label_txt1
screen_setting_label_txt1 = lv.label(screen_setting)
screen_setting_label_txt1.set_text("Device")
screen_setting_label_txt1.set_long_mode(lv.label.LONG.WRAP)
screen_setting_label_txt1.set_width(lv.pct(100))
screen_setting_label_txt1.set_pos(139, 60)
screen_setting_label_txt1.set_size(78, 14)
# Set style for screen_setting_label_txt1, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_setting_label_txt1.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_setting_label_txt1.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_setting_label_txt1.set_style_text_color(lv.color_hex(0x274a6e), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_setting_label_txt1.set_style_text_font(test_font("SourceHanSansSC_Medium", 12), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_setting_label_txt1.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_setting_label_txt1.set_style_text_letter_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_setting_label_txt1.set_style_text_line_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_setting_label_txt1.set_style_text_align(lv.TEXT_ALIGN.LEFT, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_setting_label_txt1.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_setting_label_txt1.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_setting_label_txt1.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_setting_label_txt1.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_setting_label_txt1.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_setting_label_txt1.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_setting_label_txt2
screen_setting_label_txt2 = lv.label(screen_setting)
screen_setting_label_txt2.set_text("Model")
screen_setting_label_txt2.set_long_mode(lv.label.LONG.WRAP)
screen_setting_label_txt2.set_width(lv.pct(100))
screen_setting_label_txt2.set_pos(139, 85)
screen_setting_label_txt2.set_size(72, 16)
# Set style for screen_setting_label_txt2, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_setting_label_txt2.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_setting_label_txt2.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_setting_label_txt2.set_style_text_color(lv.color_hex(0x274a6e), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_setting_label_txt2.set_style_text_font(test_font("SourceHanSansSC_Medium", 12), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_setting_label_txt2.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_setting_label_txt2.set_style_text_letter_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_setting_label_txt2.set_style_text_line_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_setting_label_txt2.set_style_text_align(lv.TEXT_ALIGN.LEFT, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_setting_label_txt2.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_setting_label_txt2.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_setting_label_txt2.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_setting_label_txt2.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_setting_label_txt2.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_setting_label_txt2.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_setting_label_txt3
screen_setting_label_txt3 = lv.label(screen_setting)
screen_setting_label_txt3.set_text("Reppo OS")
screen_setting_label_txt3.set_long_mode(lv.label.LONG.WRAP)
screen_setting_label_txt3.set_width(lv.pct(100))
screen_setting_label_txt3.set_pos(139, 110)
screen_setting_label_txt3.set_size(59, 15)
# Set style for screen_setting_label_txt3, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_setting_label_txt3.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_setting_label_txt3.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_setting_label_txt3.set_style_text_color(lv.color_hex(0x274a6e), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_setting_label_txt3.set_style_text_font(test_font("SourceHanSansSC_Medium", 12), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_setting_label_txt3.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_setting_label_txt3.set_style_text_letter_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_setting_label_txt3.set_style_text_line_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_setting_label_txt3.set_style_text_align(lv.TEXT_ALIGN.LEFT, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_setting_label_txt3.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_setting_label_txt3.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_setting_label_txt3.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_setting_label_txt3.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_setting_label_txt3.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_setting_label_txt3.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_setting_label_txt4
screen_setting_label_txt4 = lv.label(screen_setting)
screen_setting_label_txt4.set_text("Hardware")
screen_setting_label_txt4.set_long_mode(lv.label.LONG.WRAP)
screen_setting_label_txt4.set_width(lv.pct(100))
screen_setting_label_txt4.set_pos(139, 135)
screen_setting_label_txt4.set_size(67, 18)
# Set style for screen_setting_label_txt4, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_setting_label_txt4.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_setting_label_txt4.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_setting_label_txt4.set_style_text_color(lv.color_hex(0x274a6e), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_setting_label_txt4.set_style_text_font(test_font("SourceHanSansSC_Medium", 12), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_setting_label_txt4.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_setting_label_txt4.set_style_text_letter_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_setting_label_txt4.set_style_text_line_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_setting_label_txt4.set_style_text_align(lv.TEXT_ALIGN.LEFT, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_setting_label_txt4.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_setting_label_txt4.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_setting_label_txt4.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_setting_label_txt4.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_setting_label_txt4.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_setting_label_txt4.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_setting_label_info1
screen_setting_label_info1 = lv.label(screen_setting)
screen_setting_label_info1.set_text("Reppo")
screen_setting_label_info1.set_long_mode(lv.label.LONG.WRAP)
screen_setting_label_info1.set_width(lv.pct(100))
screen_setting_label_info1.set_pos(232, 60)
screen_setting_label_info1.set_size(70, 16)
# Set style for screen_setting_label_info1, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_setting_label_info1.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_setting_label_info1.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_setting_label_info1.set_style_text_color(lv.color_hex(0x274a6e), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_setting_label_info1.set_style_text_font(test_font("SourceHanSansSC_Medium", 12), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_setting_label_info1.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_setting_label_info1.set_style_text_letter_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_setting_label_info1.set_style_text_line_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_setting_label_info1.set_style_text_align(lv.TEXT_ALIGN.LEFT, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_setting_label_info1.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_setting_label_info1.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_setting_label_info1.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_setting_label_info1.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_setting_label_info1.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_setting_label_info1.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_setting_label_info2
screen_setting_label_info2 = lv.label(screen_setting)
screen_setting_label_info2.set_text("Reppo Zero")
screen_setting_label_info2.set_long_mode(lv.label.LONG.WRAP)
screen_setting_label_info2.set_width(lv.pct(100))
screen_setting_label_info2.set_pos(232, 85)
screen_setting_label_info2.set_size(70, 16)
# Set style for screen_setting_label_info2, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_setting_label_info2.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_setting_label_info2.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_setting_label_info2.set_style_text_color(lv.color_hex(0x274a6e), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_setting_label_info2.set_style_text_font(test_font("SourceHanSansSC_Medium", 12), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_setting_label_info2.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_setting_label_info2.set_style_text_letter_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_setting_label_info2.set_style_text_line_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_setting_label_info2.set_style_text_align(lv.TEXT_ALIGN.LEFT, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_setting_label_info2.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_setting_label_info2.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_setting_label_info2.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_setting_label_info2.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_setting_label_info2.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_setting_label_info2.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_setting_label_info3
screen_setting_label_info3 = lv.label(screen_setting)
screen_setting_label_info3.set_text("2.0")
screen_setting_label_info3.set_long_mode(lv.label.LONG.WRAP)
screen_setting_label_info3.set_width(lv.pct(100))
screen_setting_label_info3.set_pos(232, 110)
screen_setting_label_info3.set_size(70, 16)
# Set style for screen_setting_label_info3, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_setting_label_info3.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_setting_label_info3.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_setting_label_info3.set_style_text_color(lv.color_hex(0x274a6e), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_setting_label_info3.set_style_text_font(test_font("SourceHanSansSC_Medium", 12), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_setting_label_info3.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_setting_label_info3.set_style_text_letter_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_setting_label_info3.set_style_text_line_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_setting_label_info3.set_style_text_align(lv.TEXT_ALIGN.LEFT, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_setting_label_info3.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_setting_label_info3.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_setting_label_info3.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_setting_label_info3.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_setting_label_info3.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_setting_label_info3.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_setting_label_info4
screen_setting_label_info4 = lv.label(screen_setting)
screen_setting_label_info4.set_text("4.0")
screen_setting_label_info4.set_long_mode(lv.label.LONG.WRAP)
screen_setting_label_info4.set_width(lv.pct(100))
screen_setting_label_info4.set_pos(232, 135)
screen_setting_label_info4.set_size(70, 16)
# Set style for screen_setting_label_info4, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_setting_label_info4.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_setting_label_info4.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_setting_label_info4.set_style_text_color(lv.color_hex(0x274a6e), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_setting_label_info4.set_style_text_font(test_font("SourceHanSansSC_Medium", 12), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_setting_label_info4.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_setting_label_info4.set_style_text_letter_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_setting_label_info4.set_style_text_line_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_setting_label_info4.set_style_text_align(lv.TEXT_ALIGN.LEFT, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_setting_label_info4.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_setting_label_info4.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_setting_label_info4.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_setting_label_info4.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_setting_label_info4.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_setting_label_info4.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

screen_setting.update_layout()
# Create screen_wificfg
screen_wificfg = lv.obj()
g_kb_screen_wificfg = lv.keyboard(screen_wificfg)
g_kb_screen_wificfg.add_event_cb(lambda e: ta_event_cb(e, g_kb_screen_wificfg), lv.EVENT.ALL, None)
g_kb_screen_wificfg.add_flag(lv.obj.FLAG.HIDDEN)
g_kb_screen_wificfg.set_style_text_font(test_font("Acme_Regular", 12), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wificfg.set_size(320, 170)
screen_wificfg.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
# Set style for screen_wificfg, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_wificfg.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wificfg.set_style_bg_img_src("B:MicroPython/_AAA_wifi_lianjie_320x170.bin", lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wificfg.set_style_bg_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wificfg.set_style_bg_img_recolor_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_wificfg_img_network
screen_wificfg_img_network = lv.img(screen_wificfg)
screen_wificfg_img_network.set_src("B:MicroPython/_pomorect_alpha_160x125.bin")
screen_wificfg_img_network.add_flag(lv.obj.FLAG.CLICKABLE)
screen_wificfg_img_network.set_pivot(50,50)
screen_wificfg_img_network.set_angle(0)
screen_wificfg_img_network.set_pos(155, 33)
screen_wificfg_img_network.set_size(160, 125)
# Set style for screen_wificfg_img_network, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_wificfg_img_network.set_style_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wificfg_img_network.set_style_radius(15, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wificfg_img_network.set_style_clip_corner(True, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_wificfg_btn_back
screen_wificfg_btn_back = lv.btn(screen_wificfg)
screen_wificfg_btn_back_label = lv.label(screen_wificfg_btn_back)
screen_wificfg_btn_back_label.set_text("")
screen_wificfg_btn_back_label.set_long_mode(lv.label.LONG.WRAP)
screen_wificfg_btn_back_label.set_width(lv.pct(100))
screen_wificfg_btn_back_label.align(lv.ALIGN.CENTER, 0, 0)
screen_wificfg_btn_back.set_style_pad_all(0, lv.STATE.DEFAULT)
screen_wificfg_btn_back.set_pos(8, 9)
screen_wificfg_btn_back.set_size(15, 16)
# Set style for screen_wificfg_btn_back, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_wificfg_btn_back.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wificfg_btn_back.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wificfg_btn_back.set_style_radius(5, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wificfg_btn_back.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wificfg_btn_back.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wificfg_btn_back.set_style_text_font(test_font("SourceHanSansSC_Medium", 14), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wificfg_btn_back.set_style_text_opa(195, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wificfg_btn_back.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
# Set style for screen_wificfg_btn_back, Part: lv.PART.MAIN, State: lv.STATE.FOCUSED.
screen_wificfg_btn_back.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_wificfg_btn_back.set_style_border_width(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_wificfg_btn_back.set_style_radius(5, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_wificfg_btn_back.set_style_shadow_width(5, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_wificfg_btn_back.set_style_shadow_color(lv.color_hex(0x274a6e), lv.PART.MAIN|lv.STATE.FOCUSED)
screen_wificfg_btn_back.set_style_shadow_opa(255, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_wificfg_btn_back.set_style_shadow_spread(1, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_wificfg_btn_back.set_style_shadow_ofs_x(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_wificfg_btn_back.set_style_shadow_ofs_y(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_wificfg_btn_back.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.FOCUSED)
screen_wificfg_btn_back.set_style_text_font(test_font("SourceHanSansSC_Medium", 14), lv.PART.MAIN|lv.STATE.FOCUSED)
screen_wificfg_btn_back.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.FOCUSED)

# Create screen_wificfg_img_statusbg
screen_wificfg_img_statusbg = lv.img(screen_wificfg)
screen_wificfg_img_statusbg.set_src("B:MicroPython/_AAA_wifi_kuang_alpha_145x125.bin")
screen_wificfg_img_statusbg.add_flag(lv.obj.FLAG.CLICKABLE)
screen_wificfg_img_statusbg.set_pivot(50,50)
screen_wificfg_img_statusbg.set_angle(0)
screen_wificfg_img_statusbg.set_pos(11, 33)
screen_wificfg_img_statusbg.set_size(145, 125)
# Set style for screen_wificfg_img_statusbg, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_wificfg_img_statusbg.set_style_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wificfg_img_statusbg.set_style_radius(15, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wificfg_img_statusbg.set_style_clip_corner(True, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_wificfg_label_status
screen_wificfg_label_status = lv.label(screen_wificfg)
screen_wificfg_label_status.set_text("Connection Status")
screen_wificfg_label_status.set_long_mode(lv.label.LONG.SCROLL_CIRCULAR)
screen_wificfg_label_status.set_width(lv.pct(100))
screen_wificfg_label_status.set_pos(25, 40)
screen_wificfg_label_status.set_size(113, 14)
# Set style for screen_wificfg_label_status, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_wificfg_label_status.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wificfg_label_status.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wificfg_label_status.set_style_text_color(lv.color_hex(0x274a6e), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wificfg_label_status.set_style_text_font(test_font("SourceHanSansSC_Medium", 12), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wificfg_label_status.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wificfg_label_status.set_style_text_letter_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wificfg_label_status.set_style_text_line_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wificfg_label_status.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wificfg_label_status.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wificfg_label_status.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wificfg_label_status.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wificfg_label_status.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wificfg_label_status.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wificfg_label_status.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_wificfg_img_wifiicon
screen_wificfg_img_wifiicon = lv.img(screen_wificfg)
screen_wificfg_img_wifiicon.set_src("B:MicroPython/_AAA_wifi_icons_alpha_110x110.bin")
screen_wificfg_img_wifiicon.add_flag(lv.obj.FLAG.CLICKABLE)
screen_wificfg_img_wifiicon.set_pivot(50,50)
screen_wificfg_img_wifiicon.set_angle(0)
screen_wificfg_img_wifiicon.set_pos(28, 43)
screen_wificfg_img_wifiicon.set_size(110, 110)
# Set style for screen_wificfg_img_wifiicon, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_wificfg_img_wifiicon.set_style_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wificfg_img_wifiicon.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wificfg_img_wifiicon.set_style_clip_corner(True, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_wificfg_label_status2
screen_wificfg_label_status2 = lv.label(screen_wificfg)
screen_wificfg_label_status2.set_text("Disconnected")
screen_wificfg_label_status2.set_long_mode(lv.label.LONG.WRAP)
screen_wificfg_label_status2.set_width(lv.pct(100))
screen_wificfg_label_status2.set_pos(38, 139)
screen_wificfg_label_status2.set_size(91, 12)
# Set style for screen_wificfg_label_status2, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_wificfg_label_status2.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wificfg_label_status2.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wificfg_label_status2.set_style_text_color(lv.color_hex(0x274a6e), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wificfg_label_status2.set_style_text_font(test_font("Alatsi_Regular", 12), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wificfg_label_status2.set_style_text_opa(216, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wificfg_label_status2.set_style_text_letter_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wificfg_label_status2.set_style_text_line_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wificfg_label_status2.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wificfg_label_status2.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wificfg_label_status2.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wificfg_label_status2.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wificfg_label_status2.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wificfg_label_status2.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wificfg_label_status2.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_wificfg_img_wififalse
screen_wificfg_img_wififalse = lv.img(screen_wificfg)
screen_wificfg_img_wififalse.set_src("B:MicroPython/_wifi_false_alpha_20x20.bin")
screen_wificfg_img_wififalse.add_flag(lv.obj.FLAG.CLICKABLE)
screen_wificfg_img_wififalse.set_pivot(50,50)
screen_wificfg_img_wififalse.set_angle(0)
screen_wificfg_img_wififalse.set_pos(111, 60)
screen_wificfg_img_wififalse.set_size(20, 20)
# Set style for screen_wificfg_img_wififalse, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_wificfg_img_wififalse.set_style_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wificfg_img_wififalse.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wificfg_img_wififalse.set_style_clip_corner(True, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_wificfg_list_1
screen_wificfg_list_1 = lv.list(screen_wificfg)
screen_wificfg_list_1_item0 = screen_wificfg_list_1.add_btn(lv.SYMBOL.WIFI, "wifi1_dummy")
screen_wificfg_list_1_item1 = screen_wificfg_list_1.add_btn(lv.SYMBOL.WIFI, "wifi2_dummy")
screen_wificfg_list_1_item2 = screen_wificfg_list_1.add_btn(lv.SYMBOL.WIFI, "wifi3_dummy")
screen_wificfg_list_1_item3 = screen_wificfg_list_1.add_btn(lv.SYMBOL.WIFI, "wifi4_dummy")
screen_wificfg_list_1_item4 = screen_wificfg_list_1.add_btn(lv.SYMBOL.WIFI, "wifi5_dummy")
screen_wificfg_list_1.set_pos(172, 64)
screen_wificfg_list_1.set_size(131, 60)
screen_wificfg_list_1.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
# Set style for screen_wificfg_list_1, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_wificfg_list_1.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wificfg_list_1.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wificfg_list_1.set_style_pad_right(5, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wificfg_list_1.set_style_pad_bottom(5, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wificfg_list_1.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wificfg_list_1.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wificfg_list_1.set_style_radius(3, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wificfg_list_1.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
# Set style for screen_wificfg_list_1, Part: lv.PART.MAIN, State: lv.STATE.FOCUSED.
screen_wificfg_list_1.set_style_pad_top(5, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_wificfg_list_1.set_style_pad_left(5, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_wificfg_list_1.set_style_pad_right(5, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_wificfg_list_1.set_style_pad_bottom(5, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_wificfg_list_1.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_wificfg_list_1.set_style_border_width(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_wificfg_list_1.set_style_radius(3, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_wificfg_list_1.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.FOCUSED)
# Set style for screen_wificfg_list_1, Part: lv.PART.MAIN, State: lv.STATE.DISABLED.
screen_wificfg_list_1.set_style_pad_top(5, lv.PART.MAIN|lv.STATE.DISABLED)
screen_wificfg_list_1.set_style_pad_left(5, lv.PART.MAIN|lv.STATE.DISABLED)
screen_wificfg_list_1.set_style_pad_right(5, lv.PART.MAIN|lv.STATE.DISABLED)
screen_wificfg_list_1.set_style_pad_bottom(5, lv.PART.MAIN|lv.STATE.DISABLED)
screen_wificfg_list_1.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.DISABLED)
screen_wificfg_list_1.set_style_bg_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DISABLED)
screen_wificfg_list_1.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DISABLED)
screen_wificfg_list_1.set_style_border_width(1, lv.PART.MAIN|lv.STATE.DISABLED)
screen_wificfg_list_1.set_style_border_opa(255, lv.PART.MAIN|lv.STATE.DISABLED)
screen_wificfg_list_1.set_style_border_color(lv.color_hex(0xe1e6ee), lv.PART.MAIN|lv.STATE.DISABLED)
screen_wificfg_list_1.set_style_border_side(lv.BORDER_SIDE.FULL, lv.PART.MAIN|lv.STATE.DISABLED)
screen_wificfg_list_1.set_style_radius(3, lv.PART.MAIN|lv.STATE.DISABLED)
screen_wificfg_list_1.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DISABLED)
# Set style for screen_wificfg_list_1, Part: lv.PART.SCROLLBAR, State: lv.STATE.DEFAULT.
screen_wificfg_list_1.set_style_radius(3, lv.PART.SCROLLBAR|lv.STATE.DEFAULT)
screen_wificfg_list_1.set_style_bg_opa(255, lv.PART.SCROLLBAR|lv.STATE.DEFAULT)
screen_wificfg_list_1.set_style_bg_color(lv.color_hex(0xffffff), lv.PART.SCROLLBAR|lv.STATE.DEFAULT)
screen_wificfg_list_1.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.SCROLLBAR|lv.STATE.DEFAULT)
# Set style for screen_wificfg_list_1, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
style_screen_wificfg_list_1_extra_btns_main_default = lv.style_t()
style_screen_wificfg_list_1_extra_btns_main_default.init()
style_screen_wificfg_list_1_extra_btns_main_default.set_pad_top(0)
style_screen_wificfg_list_1_extra_btns_main_default.set_pad_left(0)
style_screen_wificfg_list_1_extra_btns_main_default.set_pad_right(5)
style_screen_wificfg_list_1_extra_btns_main_default.set_pad_bottom(4)
style_screen_wificfg_list_1_extra_btns_main_default.set_border_width(0)
style_screen_wificfg_list_1_extra_btns_main_default.set_text_color(lv.color_hex(0x274a6e))
style_screen_wificfg_list_1_extra_btns_main_default.set_text_font(test_font("SourceHanSansSC_Medium", 11))
style_screen_wificfg_list_1_extra_btns_main_default.set_text_opa(255)
style_screen_wificfg_list_1_extra_btns_main_default.set_radius(3)
style_screen_wificfg_list_1_extra_btns_main_default.set_bg_opa(17)
style_screen_wificfg_list_1_extra_btns_main_default.set_bg_color(lv.color_hex(0xffffff))
style_screen_wificfg_list_1_extra_btns_main_default.set_bg_grad_dir(lv.GRAD_DIR.NONE)
screen_wificfg_list_1_item4.add_style(style_screen_wificfg_list_1_extra_btns_main_default, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wificfg_list_1_item3.add_style(style_screen_wificfg_list_1_extra_btns_main_default, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wificfg_list_1_item2.add_style(style_screen_wificfg_list_1_extra_btns_main_default, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wificfg_list_1_item1.add_style(style_screen_wificfg_list_1_extra_btns_main_default, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wificfg_list_1_item0.add_style(style_screen_wificfg_list_1_extra_btns_main_default, lv.PART.MAIN|lv.STATE.DEFAULT)
# Set style for screen_wificfg_list_1, Part: lv.PART.MAIN, State: lv.STATE.PRESSED.
style_screen_wificfg_list_1_extra_btns_main_pressed = lv.style_t()
style_screen_wificfg_list_1_extra_btns_main_pressed.init()
style_screen_wificfg_list_1_extra_btns_main_pressed.set_pad_top(0)
style_screen_wificfg_list_1_extra_btns_main_pressed.set_pad_left(0)
style_screen_wificfg_list_1_extra_btns_main_pressed.set_pad_right(5)
style_screen_wificfg_list_1_extra_btns_main_pressed.set_pad_bottom(4)
style_screen_wificfg_list_1_extra_btns_main_pressed.set_border_width(0)
style_screen_wificfg_list_1_extra_btns_main_pressed.set_radius(3)
style_screen_wificfg_list_1_extra_btns_main_pressed.set_text_color(lv.color_hex(0x274a6e))
style_screen_wificfg_list_1_extra_btns_main_pressed.set_text_font(test_font("SourceHanSansSC_Medium", 13))
style_screen_wificfg_list_1_extra_btns_main_pressed.set_text_opa(255)
style_screen_wificfg_list_1_extra_btns_main_pressed.set_bg_opa(0)
screen_wificfg_list_1_item4.add_style(style_screen_wificfg_list_1_extra_btns_main_pressed, lv.PART.MAIN|lv.STATE.PRESSED)
screen_wificfg_list_1_item3.add_style(style_screen_wificfg_list_1_extra_btns_main_pressed, lv.PART.MAIN|lv.STATE.PRESSED)
screen_wificfg_list_1_item2.add_style(style_screen_wificfg_list_1_extra_btns_main_pressed, lv.PART.MAIN|lv.STATE.PRESSED)
screen_wificfg_list_1_item1.add_style(style_screen_wificfg_list_1_extra_btns_main_pressed, lv.PART.MAIN|lv.STATE.PRESSED)
screen_wificfg_list_1_item0.add_style(style_screen_wificfg_list_1_extra_btns_main_pressed, lv.PART.MAIN|lv.STATE.PRESSED)
# Set style for screen_wificfg_list_1, Part: lv.PART.MAIN, State: lv.STATE.FOCUSED.
style_screen_wificfg_list_1_extra_btns_main_focused = lv.style_t()
style_screen_wificfg_list_1_extra_btns_main_focused.init()
style_screen_wificfg_list_1_extra_btns_main_focused.set_pad_top(0)
style_screen_wificfg_list_1_extra_btns_main_focused.set_pad_left(1)
style_screen_wificfg_list_1_extra_btns_main_focused.set_pad_right(5)
style_screen_wificfg_list_1_extra_btns_main_focused.set_pad_bottom(4)
style_screen_wificfg_list_1_extra_btns_main_focused.set_border_width(0)
style_screen_wificfg_list_1_extra_btns_main_focused.set_radius(3)
style_screen_wificfg_list_1_extra_btns_main_focused.set_text_color(lv.color_hex(0x274a6e))
style_screen_wificfg_list_1_extra_btns_main_focused.set_text_font(test_font("SourceHanSansSC_Medium", 12))
style_screen_wificfg_list_1_extra_btns_main_focused.set_text_opa(255)
style_screen_wificfg_list_1_extra_btns_main_focused.set_bg_opa(19)
style_screen_wificfg_list_1_extra_btns_main_focused.set_bg_color(lv.color_hex(0xffffff))
style_screen_wificfg_list_1_extra_btns_main_focused.set_bg_grad_dir(lv.GRAD_DIR.NONE)
screen_wificfg_list_1_item4.add_style(style_screen_wificfg_list_1_extra_btns_main_focused, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_wificfg_list_1_item3.add_style(style_screen_wificfg_list_1_extra_btns_main_focused, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_wificfg_list_1_item2.add_style(style_screen_wificfg_list_1_extra_btns_main_focused, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_wificfg_list_1_item1.add_style(style_screen_wificfg_list_1_extra_btns_main_focused, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_wificfg_list_1_item0.add_style(style_screen_wificfg_list_1_extra_btns_main_focused, lv.PART.MAIN|lv.STATE.FOCUSED)
# Set style for screen_wificfg_list_1, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
style_screen_wificfg_list_1_extra_texts_main_default = lv.style_t()
style_screen_wificfg_list_1_extra_texts_main_default.init()
style_screen_wificfg_list_1_extra_texts_main_default.set_pad_top(5)
style_screen_wificfg_list_1_extra_texts_main_default.set_pad_left(5)
style_screen_wificfg_list_1_extra_texts_main_default.set_pad_right(5)
style_screen_wificfg_list_1_extra_texts_main_default.set_pad_bottom(5)
style_screen_wificfg_list_1_extra_texts_main_default.set_border_width(0)
style_screen_wificfg_list_1_extra_texts_main_default.set_text_color(lv.color_hex(0x274a6e))
style_screen_wificfg_list_1_extra_texts_main_default.set_text_font(test_font("SourceHanSansSC_Medium", 12))
style_screen_wificfg_list_1_extra_texts_main_default.set_text_opa(255)
style_screen_wificfg_list_1_extra_texts_main_default.set_radius(3)
style_screen_wificfg_list_1_extra_texts_main_default.set_transform_width(0)
style_screen_wificfg_list_1_extra_texts_main_default.set_bg_opa(255)
style_screen_wificfg_list_1_extra_texts_main_default.set_bg_color(lv.color_hex(0xffffff))
style_screen_wificfg_list_1_extra_texts_main_default.set_bg_grad_dir(lv.GRAD_DIR.NONE)

# Create screen_wificfg_btn_scan
screen_wificfg_btn_scan = lv.btn(screen_wificfg)
screen_wificfg_btn_scan_label = lv.label(screen_wificfg_btn_scan)
screen_wificfg_btn_scan_label.set_text("")
screen_wificfg_btn_scan_label.set_long_mode(lv.label.LONG.WRAP)
screen_wificfg_btn_scan_label.set_width(lv.pct(100))
screen_wificfg_btn_scan_label.align(lv.ALIGN.CENTER, 0, 0)
screen_wificfg_btn_scan.set_style_pad_all(0, lv.STATE.DEFAULT)
screen_wificfg_btn_scan.set_pos(177, 135)
screen_wificfg_btn_scan.set_size(41, 11)
# Set style for screen_wificfg_btn_scan, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_wificfg_btn_scan.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wificfg_btn_scan.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wificfg_btn_scan.set_style_radius(5, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wificfg_btn_scan.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wificfg_btn_scan.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wificfg_btn_scan.set_style_text_font(test_font("Alatsi_Regular", 12), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wificfg_btn_scan.set_style_text_opa(198, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wificfg_btn_scan.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
# Set style for screen_wificfg_btn_scan, Part: lv.PART.MAIN, State: lv.STATE.FOCUSED.
screen_wificfg_btn_scan.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_wificfg_btn_scan.set_style_border_width(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_wificfg_btn_scan.set_style_radius(5, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_wificfg_btn_scan.set_style_shadow_width(5, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_wificfg_btn_scan.set_style_shadow_color(lv.color_hex(0x274a6e), lv.PART.MAIN|lv.STATE.FOCUSED)
screen_wificfg_btn_scan.set_style_shadow_opa(255, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_wificfg_btn_scan.set_style_shadow_spread(1, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_wificfg_btn_scan.set_style_shadow_ofs_x(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_wificfg_btn_scan.set_style_shadow_ofs_y(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_wificfg_btn_scan.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.FOCUSED)
screen_wificfg_btn_scan.set_style_text_font(test_font("Alatsi_Regular", 12), lv.PART.MAIN|lv.STATE.FOCUSED)
screen_wificfg_btn_scan.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.FOCUSED)

# Create screen_wificfg_btn_disconnect
screen_wificfg_btn_disconnect = lv.btn(screen_wificfg)
screen_wificfg_btn_disconnect_label = lv.label(screen_wificfg_btn_disconnect)
screen_wificfg_btn_disconnect_label.set_text("")
screen_wificfg_btn_disconnect_label.set_long_mode(lv.label.LONG.WRAP)
screen_wificfg_btn_disconnect_label.set_width(lv.pct(100))
screen_wificfg_btn_disconnect_label.align(lv.ALIGN.CENTER, 0, 0)
screen_wificfg_btn_disconnect.set_style_pad_all(0, lv.STATE.DEFAULT)
screen_wificfg_btn_disconnect.set_pos(226, 135)
screen_wificfg_btn_disconnect.set_size(76, 11)
# Set style for screen_wificfg_btn_disconnect, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_wificfg_btn_disconnect.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wificfg_btn_disconnect.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wificfg_btn_disconnect.set_style_radius(5, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wificfg_btn_disconnect.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wificfg_btn_disconnect.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wificfg_btn_disconnect.set_style_text_font(test_font("Alatsi_Regular", 12), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wificfg_btn_disconnect.set_style_text_opa(193, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wificfg_btn_disconnect.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
# Set style for screen_wificfg_btn_disconnect, Part: lv.PART.MAIN, State: lv.STATE.FOCUSED.
screen_wificfg_btn_disconnect.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_wificfg_btn_disconnect.set_style_border_width(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_wificfg_btn_disconnect.set_style_radius(5, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_wificfg_btn_disconnect.set_style_shadow_width(5, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_wificfg_btn_disconnect.set_style_shadow_color(lv.color_hex(0x274a6e), lv.PART.MAIN|lv.STATE.FOCUSED)
screen_wificfg_btn_disconnect.set_style_shadow_opa(255, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_wificfg_btn_disconnect.set_style_shadow_spread(1, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_wificfg_btn_disconnect.set_style_shadow_ofs_x(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_wificfg_btn_disconnect.set_style_shadow_ofs_y(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_wificfg_btn_disconnect.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.FOCUSED)
screen_wificfg_btn_disconnect.set_style_text_font(test_font("Alatsi_Regular", 12), lv.PART.MAIN|lv.STATE.FOCUSED)
screen_wificfg_btn_disconnect.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.FOCUSED)

# Create screen_wificfg_btn_cancel
screen_wificfg_btn_cancel = lv.btn(screen_wificfg)
screen_wificfg_btn_cancel_label = lv.label(screen_wificfg_btn_cancel)
screen_wificfg_btn_cancel_label.set_text("Cancel")
screen_wificfg_btn_cancel_label.set_long_mode(lv.label.LONG.WRAP)
screen_wificfg_btn_cancel_label.set_width(lv.pct(100))
screen_wificfg_btn_cancel_label.align(lv.ALIGN.CENTER, 0, 0)
screen_wificfg_btn_cancel.set_style_pad_all(0, lv.STATE.DEFAULT)
screen_wificfg_btn_cancel.set_pos(23, 91)
screen_wificfg_btn_cancel.set_size(45, 14)
screen_wificfg_btn_cancel.add_flag(lv.obj.FLAG.HIDDEN)
# Set style for screen_wificfg_btn_cancel, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_wificfg_btn_cancel.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wificfg_btn_cancel.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wificfg_btn_cancel.set_style_radius(5, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wificfg_btn_cancel.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wificfg_btn_cancel.set_style_text_color(lv.color_hex(0x274a6e), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wificfg_btn_cancel.set_style_text_font(test_font("SourceHanSansSC_Medium", 12), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wificfg_btn_cancel.set_style_text_opa(198, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wificfg_btn_cancel.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
# Set style for screen_wificfg_btn_cancel, Part: lv.PART.MAIN, State: lv.STATE.FOCUSED.
screen_wificfg_btn_cancel.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_wificfg_btn_cancel.set_style_border_width(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_wificfg_btn_cancel.set_style_radius(5, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_wificfg_btn_cancel.set_style_shadow_width(5, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_wificfg_btn_cancel.set_style_shadow_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.FOCUSED)
screen_wificfg_btn_cancel.set_style_shadow_opa(255, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_wificfg_btn_cancel.set_style_shadow_spread(1, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_wificfg_btn_cancel.set_style_shadow_ofs_x(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_wificfg_btn_cancel.set_style_shadow_ofs_y(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_wificfg_btn_cancel.set_style_text_color(lv.color_hex(0x274a6e), lv.PART.MAIN|lv.STATE.FOCUSED)
screen_wificfg_btn_cancel.set_style_text_font(test_font("SourceHanSansSC_Medium", 12), lv.PART.MAIN|lv.STATE.FOCUSED)
screen_wificfg_btn_cancel.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.FOCUSED)

# Create screen_wificfg_btn_connect
screen_wificfg_btn_connect = lv.btn(screen_wificfg)
screen_wificfg_btn_connect_label = lv.label(screen_wificfg_btn_connect)
screen_wificfg_btn_connect_label.set_text("Connect")
screen_wificfg_btn_connect_label.set_long_mode(lv.label.LONG.WRAP)
screen_wificfg_btn_connect_label.set_width(lv.pct(100))
screen_wificfg_btn_connect_label.align(lv.ALIGN.CENTER, 0, 0)
screen_wificfg_btn_connect.set_style_pad_all(0, lv.STATE.DEFAULT)
screen_wificfg_btn_connect.set_pos(95, 91)
screen_wificfg_btn_connect.set_size(49, 14)
screen_wificfg_btn_connect.add_flag(lv.obj.FLAG.HIDDEN)
# Set style for screen_wificfg_btn_connect, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_wificfg_btn_connect.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wificfg_btn_connect.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wificfg_btn_connect.set_style_radius(5, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wificfg_btn_connect.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wificfg_btn_connect.set_style_text_color(lv.color_hex(0x274a6e), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wificfg_btn_connect.set_style_text_font(test_font("SourceHanSansSC_Medium", 12), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wificfg_btn_connect.set_style_text_opa(198, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wificfg_btn_connect.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
# Set style for screen_wificfg_btn_connect, Part: lv.PART.MAIN, State: lv.STATE.FOCUSED.
screen_wificfg_btn_connect.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_wificfg_btn_connect.set_style_border_width(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_wificfg_btn_connect.set_style_radius(5, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_wificfg_btn_connect.set_style_shadow_width(5, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_wificfg_btn_connect.set_style_shadow_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.FOCUSED)
screen_wificfg_btn_connect.set_style_shadow_opa(255, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_wificfg_btn_connect.set_style_shadow_spread(1, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_wificfg_btn_connect.set_style_shadow_ofs_x(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_wificfg_btn_connect.set_style_shadow_ofs_y(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_wificfg_btn_connect.set_style_text_color(lv.color_hex(0x274a6e), lv.PART.MAIN|lv.STATE.FOCUSED)
screen_wificfg_btn_connect.set_style_text_font(test_font("SourceHanSansSC_Medium", 12), lv.PART.MAIN|lv.STATE.FOCUSED)
screen_wificfg_btn_connect.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.FOCUSED)

# Create screen_wificfg_ta_password
screen_wificfg_ta_password = lv.textarea(screen_wificfg)
screen_wificfg_ta_password.set_text("")
screen_wificfg_ta_password.set_placeholder_text("")
screen_wificfg_ta_password.set_password_bullet("*")
screen_wificfg_ta_password.set_password_mode(False)
screen_wificfg_ta_password.set_one_line(True)
screen_wificfg_ta_password.set_accepted_chars("")
screen_wificfg_ta_password.set_max_length(32)
screen_wificfg_ta_password.add_event_cb(lambda e: ta_event_cb(e, g_kb_screen_wificfg), lv.EVENT.ALL, None)
screen_wificfg_ta_password.set_pos(15, 60)
screen_wificfg_ta_password.set_size(135, 26)
screen_wificfg_ta_password.add_flag(lv.obj.FLAG.HIDDEN)
screen_wificfg_ta_password.add_flag(lv.obj.FLAG.CLICKABLE)
screen_wificfg_ta_password.add_flag(lv.obj.FLAG.CLICK_FOCUSABLE)
# Set style for screen_wificfg_ta_password, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_wificfg_ta_password.set_style_text_color(lv.color_hex(0x274a6e), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wificfg_ta_password.set_style_text_font(test_font("SourceHanSansSC_Medium", 12), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wificfg_ta_password.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wificfg_ta_password.set_style_text_letter_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wificfg_ta_password.set_style_text_align(lv.TEXT_ALIGN.LEFT, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wificfg_ta_password.set_style_bg_opa(70, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wificfg_ta_password.set_style_bg_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wificfg_ta_password.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wificfg_ta_password.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wificfg_ta_password.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wificfg_ta_password.set_style_pad_top(7, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wificfg_ta_password.set_style_pad_right(4, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wificfg_ta_password.set_style_pad_left(26, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wificfg_ta_password.set_style_radius(7, lv.PART.MAIN|lv.STATE.DEFAULT)
# Set style for screen_wificfg_ta_password, Part: lv.PART.MAIN, State: lv.STATE.FOCUSED.
screen_wificfg_ta_password.set_style_text_color(lv.color_hex(0x274a6e), lv.PART.MAIN|lv.STATE.FOCUSED)
screen_wificfg_ta_password.set_style_text_font(test_font("SourceHanSansSC_Medium", 12), lv.PART.MAIN|lv.STATE.FOCUSED)
screen_wificfg_ta_password.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_wificfg_ta_password.set_style_bg_opa(120, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_wificfg_ta_password.set_style_bg_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.FOCUSED)
screen_wificfg_ta_password.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_wificfg_ta_password.set_style_border_width(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_wificfg_ta_password.set_style_shadow_width(5, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_wificfg_ta_password.set_style_shadow_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.FOCUSED)
screen_wificfg_ta_password.set_style_shadow_opa(255, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_wificfg_ta_password.set_style_shadow_spread(1, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_wificfg_ta_password.set_style_shadow_ofs_x(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_wificfg_ta_password.set_style_shadow_ofs_y(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_wificfg_ta_password.set_style_pad_top(7, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_wificfg_ta_password.set_style_pad_right(4, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_wificfg_ta_password.set_style_pad_left(22, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_wificfg_ta_password.set_style_radius(4, lv.PART.MAIN|lv.STATE.FOCUSED)
# Set style for screen_wificfg_ta_password, Part: lv.PART.SCROLLBAR, State: lv.STATE.DEFAULT.
screen_wificfg_ta_password.set_style_bg_opa(0, lv.PART.SCROLLBAR|lv.STATE.DEFAULT)
screen_wificfg_ta_password.set_style_radius(0, lv.PART.SCROLLBAR|lv.STATE.DEFAULT)

# Create screen_wificfg_img_passicon
screen_wificfg_img_passicon = lv.img(screen_wificfg)
screen_wificfg_img_passicon.set_src("B:MicroPython/_AAA_wifi_suo_icon_alpha_13x15.bin")
screen_wificfg_img_passicon.add_flag(lv.obj.FLAG.CLICKABLE)
screen_wificfg_img_passicon.set_pivot(50,50)
screen_wificfg_img_passicon.set_angle(0)
screen_wificfg_img_passicon.set_pos(18, 65)
screen_wificfg_img_passicon.set_size(13, 15)
screen_wificfg_img_passicon.add_flag(lv.obj.FLAG.HIDDEN)
# Set style for screen_wificfg_img_passicon, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_wificfg_img_passicon.set_style_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wificfg_img_passicon.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wificfg_img_passicon.set_style_clip_corner(True, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_wificfg_spinner_connect
screen_wificfg_spinner_connect = lv.spinner(screen_wificfg, 1000, 60)
screen_wificfg_spinner_connect.set_pos(63, 114)
screen_wificfg_spinner_connect.set_size(40, 40)
screen_wificfg_spinner_connect.add_flag(lv.obj.FLAG.HIDDEN)
# Set style for screen_wificfg_spinner_connect, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_wificfg_spinner_connect.set_style_pad_top(5, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wificfg_spinner_connect.set_style_pad_bottom(5, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wificfg_spinner_connect.set_style_pad_left(5, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wificfg_spinner_connect.set_style_pad_right(5, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wificfg_spinner_connect.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wificfg_spinner_connect.set_style_arc_width(4, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wificfg_spinner_connect.set_style_arc_opa(208, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wificfg_spinner_connect.set_style_arc_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_wificfg_spinner_connect.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
# Set style for screen_wificfg_spinner_connect, Part: lv.PART.INDICATOR, State: lv.STATE.DEFAULT.
screen_wificfg_spinner_connect.set_style_arc_width(4, lv.PART.INDICATOR|lv.STATE.DEFAULT)
screen_wificfg_spinner_connect.set_style_arc_opa(255, lv.PART.INDICATOR|lv.STATE.DEFAULT)
screen_wificfg_spinner_connect.set_style_arc_color(lv.color_hex(0x274a6e), lv.PART.INDICATOR|lv.STATE.DEFAULT)

screen_wificfg.update_layout()
# Create screen_donelist
screen_donelist = lv.obj()
g_kb_screen_donelist = lv.keyboard(screen_donelist)
g_kb_screen_donelist.add_event_cb(lambda e: ta_event_cb(e, g_kb_screen_donelist), lv.EVENT.ALL, None)
g_kb_screen_donelist.add_flag(lv.obj.FLAG.HIDDEN)
g_kb_screen_donelist.set_style_text_font(test_font("Acme_Regular", 12), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist.set_size(320, 170)
screen_donelist.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
# Set style for screen_donelist, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_donelist.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist.set_style_bg_img_src("B:MicroPython/_AAA_done_bg_320x170.bin", lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist.set_style_bg_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist.set_style_bg_img_recolor_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_donelist_line_4
screen_donelist_line_4 = lv.line(screen_donelist)
screen_donelist_line_4_line_points = [{"x":0, "y":0},{"x":320, "y":0},]
screen_donelist_line_4.set_points(screen_donelist_line_4_line_points, 2)
screen_donelist_line_4.set_pos(25, 125)
screen_donelist_line_4.set_size(279, 1)
# Set style for screen_donelist_line_4, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_donelist_line_4.set_style_line_width(1, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_line_4.set_style_line_color(lv.color_hex(0x7698bc), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_line_4.set_style_line_opa(130, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_line_4.set_style_line_rounded(True, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_donelist_line_3
screen_donelist_line_3 = lv.line(screen_donelist)
screen_donelist_line_3_line_points = [{"x":0, "y":0},{"x":320, "y":0},]
screen_donelist_line_3.set_points(screen_donelist_line_3_line_points, 2)
screen_donelist_line_3.set_pos(25, 99)
screen_donelist_line_3.set_size(279, 1)
# Set style for screen_donelist_line_3, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_donelist_line_3.set_style_line_width(1, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_line_3.set_style_line_color(lv.color_hex(0x7698bc), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_line_3.set_style_line_opa(136, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_line_3.set_style_line_rounded(True, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_donelist_line_2
screen_donelist_line_2 = lv.line(screen_donelist)
screen_donelist_line_2_line_points = [{"x":0, "y":0},{"x":320, "y":0},]
screen_donelist_line_2.set_points(screen_donelist_line_2_line_points, 2)
screen_donelist_line_2.set_pos(25, 72)
screen_donelist_line_2.set_size(279, 1)
# Set style for screen_donelist_line_2, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_donelist_line_2.set_style_line_width(1, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_line_2.set_style_line_color(lv.color_hex(0x7698bc), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_line_2.set_style_line_opa(130, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_line_2.set_style_line_rounded(True, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_donelist_line_1
screen_donelist_line_1 = lv.line(screen_donelist)
screen_donelist_line_1_line_points = [{"x":0, "y":0},{"x":320, "y":0},]
screen_donelist_line_1.set_points(screen_donelist_line_1_line_points, 2)
screen_donelist_line_1.set_pos(25, 45)
screen_donelist_line_1.set_size(279, 1)
# Set style for screen_donelist_line_1, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_donelist_line_1.set_style_line_width(1, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_line_1.set_style_line_color(lv.color_hex(0x7698bc), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_line_1.set_style_line_opa(134, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_line_1.set_style_line_rounded(True, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_donelist_label_donenum
screen_donelist_label_donenum = lv.label(screen_donelist)
screen_donelist_label_donenum.set_text("100")
screen_donelist_label_donenum.set_long_mode(lv.label.LONG.WRAP)
screen_donelist_label_donenum.set_width(lv.pct(100))
screen_donelist_label_donenum.set_pos(152, 9)
screen_donelist_label_donenum.set_size(33, 13)
# Set style for screen_donelist_label_donenum, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_donelist_label_donenum.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_label_donenum.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_label_donenum.set_style_text_color(lv.color_hex(0x0689a1), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_label_donenum.set_style_text_font(test_font("SourceHanSansSC_Medium", 14), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_label_donenum.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_label_donenum.set_style_text_letter_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_label_donenum.set_style_text_line_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_label_donenum.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_label_donenum.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_label_donenum.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_label_donenum.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_label_donenum.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_label_donenum.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_label_donenum.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_donelist_label_nosavenum
screen_donelist_label_nosavenum = lv.label(screen_donelist)
screen_donelist_label_nosavenum.set_text("100")
screen_donelist_label_nosavenum.set_long_mode(lv.label.LONG.WRAP)
screen_donelist_label_nosavenum.set_width(lv.pct(100))
screen_donelist_label_nosavenum.set_pos(252, 9)
screen_donelist_label_nosavenum.set_size(33, 13)
# Set style for screen_donelist_label_nosavenum, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_donelist_label_nosavenum.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_label_nosavenum.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_label_nosavenum.set_style_text_color(lv.color_hex(0x8b287d), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_label_nosavenum.set_style_text_font(test_font("SourceHanSansSC_Medium", 14), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_label_nosavenum.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_label_nosavenum.set_style_text_letter_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_label_nosavenum.set_style_text_line_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_label_nosavenum.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_label_nosavenum.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_label_nosavenum.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_label_nosavenum.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_label_nosavenum.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_label_nosavenum.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_label_nosavenum.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_donelist_label_date1
screen_donelist_label_date1 = lv.label(screen_donelist)
screen_donelist_label_date1.set_text("2026.04.13")
screen_donelist_label_date1.set_long_mode(lv.label.LONG.WRAP)
screen_donelist_label_date1.set_width(lv.pct(100))
screen_donelist_label_date1.set_pos(205, 39)
screen_donelist_label_date1.set_size(60, 13)
# Set style for screen_donelist_label_date1, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_donelist_label_date1.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_label_date1.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_label_date1.set_style_text_color(lv.color_hex(0x2f67a4), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_label_date1.set_style_text_font(test_font("SourceHanSansSC_Medium", 11), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_label_date1.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_label_date1.set_style_text_letter_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_label_date1.set_style_text_line_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_label_date1.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_label_date1.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_label_date1.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_label_date1.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_label_date1.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_label_date1.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_label_date1.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_donelist_label_done1
screen_donelist_label_done1 = lv.label(screen_donelist)
screen_donelist_label_done1.set_text("跑步3公里")
screen_donelist_label_done1.set_long_mode(lv.label.LONG.DOT)
screen_donelist_label_done1.set_width(lv.pct(100))
screen_donelist_label_done1.set_pos(28, 38)
screen_donelist_label_done1.set_size(165, 18)
# Set style for screen_donelist_label_done1, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_donelist_label_done1.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_label_done1.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_label_done1.set_style_text_color(lv.color_hex(0x2f76a4), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_label_done1.set_style_text_font(test_font("SourceHanSansSC_Medium", 14), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_label_done1.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_label_done1.set_style_text_letter_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_label_done1.set_style_text_line_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_label_done1.set_style_text_align(lv.TEXT_ALIGN.LEFT, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_label_done1.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_label_done1.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_label_done1.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_label_done1.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_label_done1.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_label_done1.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_donelist_label_done2
screen_donelist_label_done2 = lv.label(screen_donelist)
screen_donelist_label_done2.set_text("考研英语一第二次模拟考试")
screen_donelist_label_done2.set_long_mode(lv.label.LONG.DOT)
screen_donelist_label_done2.set_width(lv.pct(100))
screen_donelist_label_done2.set_pos(28, 65)
screen_donelist_label_done2.set_size(165, 19)
# Set style for screen_donelist_label_done2, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_donelist_label_done2.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_label_done2.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_label_done2.set_style_text_color(lv.color_hex(0x2f67a4), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_label_done2.set_style_text_font(test_font("SourceHanSansSC_Medium", 14), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_label_done2.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_label_done2.set_style_text_letter_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_label_done2.set_style_text_line_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_label_done2.set_style_text_align(lv.TEXT_ALIGN.LEFT, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_label_done2.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_label_done2.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_label_done2.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_label_done2.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_label_done2.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_label_done2.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_donelist_label_done3
screen_donelist_label_done3 = lv.label(screen_donelist)
screen_donelist_label_done3.set_text("阅读专业书籍")
screen_donelist_label_done3.set_long_mode(lv.label.LONG.DOT)
screen_donelist_label_done3.set_width(lv.pct(100))
screen_donelist_label_done3.set_pos(28, 91)
screen_donelist_label_done3.set_size(165, 18)
# Set style for screen_donelist_label_done3, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_donelist_label_done3.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_label_done3.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_label_done3.set_style_text_color(lv.color_hex(0x2f67a4), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_label_done3.set_style_text_font(test_font("SourceHanSansSC_Medium", 14), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_label_done3.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_label_done3.set_style_text_letter_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_label_done3.set_style_text_line_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_label_done3.set_style_text_align(lv.TEXT_ALIGN.LEFT, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_label_done3.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_label_done3.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_label_done3.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_label_done3.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_label_done3.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_label_done3.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_donelist_label_done4
screen_donelist_label_done4 = lv.label(screen_donelist)
screen_donelist_label_done4.set_text("马克思主义论文")
screen_donelist_label_done4.set_long_mode(lv.label.LONG.DOT)
screen_donelist_label_done4.set_width(lv.pct(100))
screen_donelist_label_done4.set_pos(28, 118)
screen_donelist_label_done4.set_size(165, 18)
# Set style for screen_donelist_label_done4, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_donelist_label_done4.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_label_done4.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_label_done4.set_style_text_color(lv.color_hex(0x2f67a4), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_label_done4.set_style_text_font(test_font("SourceHanSansSC_Medium", 14), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_label_done4.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_label_done4.set_style_text_letter_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_label_done4.set_style_text_line_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_label_done4.set_style_text_align(lv.TEXT_ALIGN.LEFT, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_label_done4.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_label_done4.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_label_done4.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_label_done4.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_label_done4.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_label_done4.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_donelist_label_date2
screen_donelist_label_date2 = lv.label(screen_donelist)
screen_donelist_label_date2.set_text("2026.04.13")
screen_donelist_label_date2.set_long_mode(lv.label.LONG.WRAP)
screen_donelist_label_date2.set_width(lv.pct(100))
screen_donelist_label_date2.set_pos(205, 66)
screen_donelist_label_date2.set_size(60, 13)
# Set style for screen_donelist_label_date2, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_donelist_label_date2.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_label_date2.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_label_date2.set_style_text_color(lv.color_hex(0x2f67a4), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_label_date2.set_style_text_font(test_font("SourceHanSansSC_Medium", 11), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_label_date2.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_label_date2.set_style_text_letter_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_label_date2.set_style_text_line_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_label_date2.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_label_date2.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_label_date2.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_label_date2.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_label_date2.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_label_date2.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_label_date2.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_donelist_label_date3
screen_donelist_label_date3 = lv.label(screen_donelist)
screen_donelist_label_date3.set_text("2026.04.13")
screen_donelist_label_date3.set_long_mode(lv.label.LONG.WRAP)
screen_donelist_label_date3.set_width(lv.pct(100))
screen_donelist_label_date3.set_pos(205, 93)
screen_donelist_label_date3.set_size(60, 13)
# Set style for screen_donelist_label_date3, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_donelist_label_date3.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_label_date3.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_label_date3.set_style_text_color(lv.color_hex(0x2f67a4), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_label_date3.set_style_text_font(test_font("SourceHanSansSC_Medium", 11), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_label_date3.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_label_date3.set_style_text_letter_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_label_date3.set_style_text_line_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_label_date3.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_label_date3.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_label_date3.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_label_date3.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_label_date3.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_label_date3.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_label_date3.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_donelist_label_date4
screen_donelist_label_date4 = lv.label(screen_donelist)
screen_donelist_label_date4.set_text("2026.04.13")
screen_donelist_label_date4.set_long_mode(lv.label.LONG.WRAP)
screen_donelist_label_date4.set_width(lv.pct(100))
screen_donelist_label_date4.set_pos(205, 119)
screen_donelist_label_date4.set_size(60, 13)
# Set style for screen_donelist_label_date4, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_donelist_label_date4.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_label_date4.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_label_date4.set_style_text_color(lv.color_hex(0x2f67a4), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_label_date4.set_style_text_font(test_font("SourceHanSansSC_Medium", 11), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_label_date4.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_label_date4.set_style_text_letter_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_label_date4.set_style_text_line_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_label_date4.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_label_date4.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_label_date4.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_label_date4.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_label_date4.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_label_date4.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_label_date4.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_donelist_label_page
screen_donelist_label_page = lv.label(screen_donelist)
screen_donelist_label_page.set_text("1/1")
screen_donelist_label_page.set_long_mode(lv.label.LONG.WRAP)
screen_donelist_label_page.set_width(lv.pct(100))
screen_donelist_label_page.set_pos(117, 147)
screen_donelist_label_page.set_size(90, 15)
# Set style for screen_donelist_label_page, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_donelist_label_page.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_label_page.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_label_page.set_style_text_color(lv.color_hex(0x2f67a4), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_label_page.set_style_text_font(test_font("SourceHanSansSC_Medium", 14), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_label_page.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_label_page.set_style_text_letter_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_label_page.set_style_text_line_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_label_page.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_label_page.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_label_page.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_label_page.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_label_page.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_label_page.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_label_page.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_donelist_btn_save
screen_donelist_btn_save = lv.btn(screen_donelist)
screen_donelist_btn_save_label = lv.label(screen_donelist_btn_save)
screen_donelist_btn_save_label.set_text("")
screen_donelist_btn_save_label.set_long_mode(lv.label.LONG.WRAP)
screen_donelist_btn_save_label.set_width(lv.pct(100))
screen_donelist_btn_save_label.align(lv.ALIGN.CENTER, 0, 0)
screen_donelist_btn_save.set_style_pad_all(0, lv.STATE.DEFAULT)
screen_donelist_btn_save.set_pos(294, 7)
screen_donelist_btn_save.set_size(18, 19)
# Set style for screen_donelist_btn_save, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_donelist_btn_save.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_btn_save.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_btn_save.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_btn_save.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_btn_save.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_btn_save.set_style_text_font(test_font("montserratMedium", 16), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_btn_save.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_btn_save.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
# Set style for screen_donelist_btn_save, Part: lv.PART.MAIN, State: lv.STATE.FOCUSED.
screen_donelist_btn_save.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_donelist_btn_save.set_style_border_width(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_donelist_btn_save.set_style_radius(5, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_donelist_btn_save.set_style_shadow_width(5, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_donelist_btn_save.set_style_shadow_color(lv.color_hex(0x2f67a4), lv.PART.MAIN|lv.STATE.FOCUSED)
screen_donelist_btn_save.set_style_shadow_opa(255, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_donelist_btn_save.set_style_shadow_spread(1, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_donelist_btn_save.set_style_shadow_ofs_x(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_donelist_btn_save.set_style_shadow_ofs_y(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_donelist_btn_save.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.FOCUSED)
screen_donelist_btn_save.set_style_text_font(test_font("montserratMedium", 16), lv.PART.MAIN|lv.STATE.FOCUSED)
screen_donelist_btn_save.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.FOCUSED)

# Create screen_donelist_btn_nxtpage
screen_donelist_btn_nxtpage = lv.btn(screen_donelist)
screen_donelist_btn_nxtpage_label = lv.label(screen_donelist_btn_nxtpage)
screen_donelist_btn_nxtpage_label.set_text("")
screen_donelist_btn_nxtpage_label.set_long_mode(lv.label.LONG.WRAP)
screen_donelist_btn_nxtpage_label.set_width(lv.pct(100))
screen_donelist_btn_nxtpage_label.align(lv.ALIGN.CENTER, 0, 0)
screen_donelist_btn_nxtpage.set_style_pad_all(0, lv.STATE.DEFAULT)
screen_donelist_btn_nxtpage.set_pos(217, 145)
screen_donelist_btn_nxtpage.set_size(97, 19)
# Set style for screen_donelist_btn_nxtpage, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_donelist_btn_nxtpage.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_btn_nxtpage.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_btn_nxtpage.set_style_radius(5, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_btn_nxtpage.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_btn_nxtpage.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_btn_nxtpage.set_style_text_font(test_font("montserratMedium", 16), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_btn_nxtpage.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_btn_nxtpage.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
# Set style for screen_donelist_btn_nxtpage, Part: lv.PART.MAIN, State: lv.STATE.FOCUSED.
screen_donelist_btn_nxtpage.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_donelist_btn_nxtpage.set_style_border_width(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_donelist_btn_nxtpage.set_style_radius(5, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_donelist_btn_nxtpage.set_style_shadow_width(5, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_donelist_btn_nxtpage.set_style_shadow_color(lv.color_hex(0x2f67a4), lv.PART.MAIN|lv.STATE.FOCUSED)
screen_donelist_btn_nxtpage.set_style_shadow_opa(255, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_donelist_btn_nxtpage.set_style_shadow_spread(1, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_donelist_btn_nxtpage.set_style_shadow_ofs_x(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_donelist_btn_nxtpage.set_style_shadow_ofs_y(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_donelist_btn_nxtpage.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.FOCUSED)
screen_donelist_btn_nxtpage.set_style_text_font(test_font("montserratMedium", 16), lv.PART.MAIN|lv.STATE.FOCUSED)
screen_donelist_btn_nxtpage.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.FOCUSED)

# Create screen_donelist_btn_lastpage
screen_donelist_btn_lastpage = lv.btn(screen_donelist)
screen_donelist_btn_lastpage_label = lv.label(screen_donelist_btn_lastpage)
screen_donelist_btn_lastpage_label.set_text("")
screen_donelist_btn_lastpage_label.set_long_mode(lv.label.LONG.WRAP)
screen_donelist_btn_lastpage_label.set_width(lv.pct(100))
screen_donelist_btn_lastpage_label.align(lv.ALIGN.CENTER, 0, 0)
screen_donelist_btn_lastpage.set_style_pad_all(0, lv.STATE.DEFAULT)
screen_donelist_btn_lastpage.set_pos(5, 146)
screen_donelist_btn_lastpage.set_size(94, 18)
# Set style for screen_donelist_btn_lastpage, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_donelist_btn_lastpage.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_btn_lastpage.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_btn_lastpage.set_style_radius(5, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_btn_lastpage.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_btn_lastpage.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_btn_lastpage.set_style_text_font(test_font("montserratMedium", 16), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_btn_lastpage.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_btn_lastpage.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
# Set style for screen_donelist_btn_lastpage, Part: lv.PART.MAIN, State: lv.STATE.FOCUSED.
screen_donelist_btn_lastpage.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_donelist_btn_lastpage.set_style_border_width(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_donelist_btn_lastpage.set_style_radius(5, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_donelist_btn_lastpage.set_style_shadow_width(5, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_donelist_btn_lastpage.set_style_shadow_color(lv.color_hex(0x2f67a4), lv.PART.MAIN|lv.STATE.FOCUSED)
screen_donelist_btn_lastpage.set_style_shadow_opa(255, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_donelist_btn_lastpage.set_style_shadow_spread(1, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_donelist_btn_lastpage.set_style_shadow_ofs_x(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_donelist_btn_lastpage.set_style_shadow_ofs_y(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_donelist_btn_lastpage.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.FOCUSED)
screen_donelist_btn_lastpage.set_style_text_font(test_font("montserratMedium", 16), lv.PART.MAIN|lv.STATE.FOCUSED)
screen_donelist_btn_lastpage.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.FOCUSED)

# Create screen_donelist_label_focus1
screen_donelist_label_focus1 = lv.label(screen_donelist)
screen_donelist_label_focus1.set_text("20min")
screen_donelist_label_focus1.set_long_mode(lv.label.LONG.WRAP)
screen_donelist_label_focus1.set_width(lv.pct(100))
screen_donelist_label_focus1.set_pos(268, 39)
screen_donelist_label_focus1.set_size(40, 12)
# Set style for screen_donelist_label_focus1, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_donelist_label_focus1.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_label_focus1.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_label_focus1.set_style_text_color(lv.color_hex(0x2f67a4), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_label_focus1.set_style_text_font(test_font("SourceHanSansSC_Medium", 11), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_label_focus1.set_style_text_opa(240, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_label_focus1.set_style_text_letter_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_label_focus1.set_style_text_line_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_label_focus1.set_style_text_align(lv.TEXT_ALIGN.RIGHT, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_label_focus1.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_label_focus1.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_label_focus1.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_label_focus1.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_label_focus1.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_label_focus1.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_donelist_label_focus2
screen_donelist_label_focus2 = lv.label(screen_donelist)
screen_donelist_label_focus2.set_text("200min")
screen_donelist_label_focus2.set_long_mode(lv.label.LONG.WRAP)
screen_donelist_label_focus2.set_width(lv.pct(100))
screen_donelist_label_focus2.set_pos(268, 66)
screen_donelist_label_focus2.set_size(40, 12)
# Set style for screen_donelist_label_focus2, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_donelist_label_focus2.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_label_focus2.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_label_focus2.set_style_text_color(lv.color_hex(0x2f67a4), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_label_focus2.set_style_text_font(test_font("SourceHanSansSC_Medium", 11), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_label_focus2.set_style_text_opa(244, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_label_focus2.set_style_text_letter_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_label_focus2.set_style_text_line_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_label_focus2.set_style_text_align(lv.TEXT_ALIGN.RIGHT, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_label_focus2.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_label_focus2.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_label_focus2.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_label_focus2.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_label_focus2.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_label_focus2.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_donelist_label_focus3
screen_donelist_label_focus3 = lv.label(screen_donelist)
screen_donelist_label_focus3.set_text("20min")
screen_donelist_label_focus3.set_long_mode(lv.label.LONG.WRAP)
screen_donelist_label_focus3.set_width(lv.pct(100))
screen_donelist_label_focus3.set_pos(268, 93)
screen_donelist_label_focus3.set_size(40, 12)
# Set style for screen_donelist_label_focus3, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_donelist_label_focus3.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_label_focus3.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_label_focus3.set_style_text_color(lv.color_hex(0x2f67a4), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_label_focus3.set_style_text_font(test_font("SourceHanSansSC_Medium", 11), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_label_focus3.set_style_text_opa(245, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_label_focus3.set_style_text_letter_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_label_focus3.set_style_text_line_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_label_focus3.set_style_text_align(lv.TEXT_ALIGN.RIGHT, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_label_focus3.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_label_focus3.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_label_focus3.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_label_focus3.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_label_focus3.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_label_focus3.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_donelist_label_focus4
screen_donelist_label_focus4 = lv.label(screen_donelist)
screen_donelist_label_focus4.set_text("80min")
screen_donelist_label_focus4.set_long_mode(lv.label.LONG.WRAP)
screen_donelist_label_focus4.set_width(lv.pct(100))
screen_donelist_label_focus4.set_pos(268, 119)
screen_donelist_label_focus4.set_size(40, 12)
# Set style for screen_donelist_label_focus4, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_donelist_label_focus4.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_label_focus4.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_label_focus4.set_style_text_color(lv.color_hex(0x2f67a4), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_label_focus4.set_style_text_font(test_font("SourceHanSansSC_Medium", 11), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_label_focus4.set_style_text_opa(244, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_label_focus4.set_style_text_letter_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_label_focus4.set_style_text_line_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_label_focus4.set_style_text_align(lv.TEXT_ALIGN.RIGHT, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_label_focus4.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_label_focus4.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_label_focus4.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_label_focus4.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_label_focus4.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_label_focus4.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_donelist_btn_back
screen_donelist_btn_back = lv.btn(screen_donelist)
screen_donelist_btn_back_label = lv.label(screen_donelist_btn_back)
screen_donelist_btn_back_label.set_text("<")
screen_donelist_btn_back_label.set_long_mode(lv.label.LONG.WRAP)
screen_donelist_btn_back_label.set_width(lv.pct(100))
screen_donelist_btn_back_label.align(lv.ALIGN.CENTER, 0, 0)
screen_donelist_btn_back.set_style_pad_all(0, lv.STATE.DEFAULT)
screen_donelist_btn_back.set_pos(7, 8)
screen_donelist_btn_back.set_size(20, 17)
# Set style for screen_donelist_btn_back, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_donelist_btn_back.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_btn_back.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_btn_back.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_btn_back.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_btn_back.set_style_text_color(lv.color_hex(0x2f67a4), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_btn_back.set_style_text_font(test_font("SourceHanSansSC_Medium", 14), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_btn_back.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_donelist_btn_back.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
# Set style for screen_donelist_btn_back, Part: lv.PART.MAIN, State: lv.STATE.FOCUSED.
screen_donelist_btn_back.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_donelist_btn_back.set_style_border_width(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_donelist_btn_back.set_style_radius(5, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_donelist_btn_back.set_style_shadow_width(5, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_donelist_btn_back.set_style_shadow_color(lv.color_hex(0x2f67a4), lv.PART.MAIN|lv.STATE.FOCUSED)
screen_donelist_btn_back.set_style_shadow_opa(255, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_donelist_btn_back.set_style_shadow_spread(1, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_donelist_btn_back.set_style_shadow_ofs_x(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_donelist_btn_back.set_style_shadow_ofs_y(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_donelist_btn_back.set_style_text_color(lv.color_hex(0x2f67a4), lv.PART.MAIN|lv.STATE.FOCUSED)
screen_donelist_btn_back.set_style_text_font(test_font("SourceHanSansSC_Medium", 14), lv.PART.MAIN|lv.STATE.FOCUSED)
screen_donelist_btn_back.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.FOCUSED)

screen_donelist.update_layout()
# Create screen_answer
screen_answer = lv.obj()
g_kb_screen_answer = lv.keyboard(screen_answer)
g_kb_screen_answer.add_event_cb(lambda e: ta_event_cb(e, g_kb_screen_answer), lv.EVENT.ALL, None)
g_kb_screen_answer.add_flag(lv.obj.FLAG.HIDDEN)
g_kb_screen_answer.set_style_text_font(test_font("Acme_Regular", 12), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_answer.set_size(320, 170)
screen_answer.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
# Set style for screen_answer, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_answer.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_answer.set_style_bg_img_src("B:MicroPython/_AAA_answer_1_0905_320x170.bin", lv.PART.MAIN|lv.STATE.DEFAULT)
screen_answer.set_style_bg_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_answer.set_style_bg_img_recolor_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_answer_btn_start
screen_answer_btn_start = lv.btn(screen_answer)
screen_answer_btn_start_label = lv.label(screen_answer_btn_start)
screen_answer_btn_start_label.set_text("")
screen_answer_btn_start_label.set_long_mode(lv.label.LONG.WRAP)
screen_answer_btn_start_label.set_width(lv.pct(100))
screen_answer_btn_start_label.align(lv.ALIGN.CENTER, 0, 0)
screen_answer_btn_start.set_style_pad_all(0, lv.STATE.DEFAULT)
screen_answer_btn_start.set_pos(8, 155)
screen_answer_btn_start.set_size(1, 9)
# Set style for screen_answer_btn_start, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_answer_btn_start.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_answer_btn_start.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_answer_btn_start.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_answer_btn_start.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_answer_btn_start.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_answer_btn_start.set_style_text_font(test_font("SourceHanSansSC_Medium", 14), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_answer_btn_start.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_answer_btn_start.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
# Set style for screen_answer_btn_start, Part: lv.PART.MAIN, State: lv.STATE.FOCUSED.
screen_answer_btn_start.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_answer_btn_start.set_style_border_width(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_answer_btn_start.set_style_radius(10, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_answer_btn_start.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_answer_btn_start.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.FOCUSED)
screen_answer_btn_start.set_style_text_font(test_font("SourceHanSansSC_Medium", 14), lv.PART.MAIN|lv.STATE.FOCUSED)
screen_answer_btn_start.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.FOCUSED)
# Set style for screen_answer_btn_start, Part: lv.PART.MAIN, State: lv.STATE.PRESSED.
screen_answer_btn_start.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.PRESSED)
screen_answer_btn_start.set_style_border_width(0, lv.PART.MAIN|lv.STATE.PRESSED)
screen_answer_btn_start.set_style_radius(0, lv.PART.MAIN|lv.STATE.PRESSED)
screen_answer_btn_start.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.PRESSED)
screen_answer_btn_start.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.PRESSED)
screen_answer_btn_start.set_style_text_font(test_font("SourceHanSansSC_Medium", 14), lv.PART.MAIN|lv.STATE.PRESSED)
screen_answer_btn_start.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.PRESSED)

screen_answer.update_layout()
# Create screen_loading
screen_loading = lv.obj()
g_kb_screen_loading = lv.keyboard(screen_loading)
g_kb_screen_loading.add_event_cb(lambda e: ta_event_cb(e, g_kb_screen_loading), lv.EVENT.ALL, None)
g_kb_screen_loading.add_flag(lv.obj.FLAG.HIDDEN)
g_kb_screen_loading.set_style_text_font(test_font("Acme_Regular", 12), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_loading.set_size(320, 170)
screen_loading.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
# Set style for screen_loading, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_loading.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_loading.set_style_bg_img_src("B:MicroPython/_AAA_answer_2_0905_320x170.bin", lv.PART.MAIN|lv.STATE.DEFAULT)
screen_loading.set_style_bg_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_loading.set_style_bg_img_recolor_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_loading_img_icon
screen_loading_img_icon = lv.img(screen_loading)
screen_loading_img_icon.set_src("B:MicroPython/_AAA_answer_icon_bg_alpha_58x58.bin")
screen_loading_img_icon.add_flag(lv.obj.FLAG.CLICKABLE)
screen_loading_img_icon.set_pivot(50,50)
screen_loading_img_icon.set_angle(0)
screen_loading_img_icon.set_pos(128, 48)
screen_loading_img_icon.set_size(58, 58)
# Set style for screen_loading_img_icon, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_loading_img_icon.set_style_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_loading_img_icon.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_loading_img_icon.set_style_clip_corner(True, lv.PART.MAIN|lv.STATE.DEFAULT)

screen_loading.update_layout()
# Create screen_getanswer
screen_getanswer = lv.obj()
g_kb_screen_getanswer = lv.keyboard(screen_getanswer)
g_kb_screen_getanswer.add_event_cb(lambda e: ta_event_cb(e, g_kb_screen_getanswer), lv.EVENT.ALL, None)
g_kb_screen_getanswer.add_flag(lv.obj.FLAG.HIDDEN)
g_kb_screen_getanswer.set_style_text_font(test_font("Acme_Regular", 12), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_getanswer.set_size(320, 170)
screen_getanswer.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
# Set style for screen_getanswer, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_getanswer.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_getanswer.set_style_bg_img_src("B:MicroPython/_AAA__answer_end_bg_320x170.bin", lv.PART.MAIN|lv.STATE.DEFAULT)
screen_getanswer.set_style_bg_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_getanswer.set_style_bg_img_recolor_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_getanswer_btn_answer
screen_getanswer_btn_answer = lv.btn(screen_getanswer)
screen_getanswer_btn_answer_label = lv.label(screen_getanswer_btn_answer)
screen_getanswer_btn_answer_label.set_text("\"最好的事情正要发生\"")
screen_getanswer_btn_answer_label.set_long_mode(lv.label.LONG.WRAP)
screen_getanswer_btn_answer_label.set_width(lv.pct(100))
screen_getanswer_btn_answer_label.align(lv.ALIGN.CENTER, 0, 0)
screen_getanswer_btn_answer.set_style_pad_all(0, lv.STATE.DEFAULT)
screen_getanswer_btn_answer.set_pos(26, 69)
screen_getanswer_btn_answer.set_size(271, 32)
# Set style for screen_getanswer_btn_answer, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_getanswer_btn_answer.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_getanswer_btn_answer.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_getanswer_btn_answer.set_style_radius(5, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_getanswer_btn_answer.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_getanswer_btn_answer.set_style_text_color(lv.color_hex(0x274a6e), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_getanswer_btn_answer.set_style_text_font(test_font("SourceHanSansSC_Medium", 14), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_getanswer_btn_answer.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_getanswer_btn_answer.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
# Set style for screen_getanswer_btn_answer, Part: lv.PART.MAIN, State: lv.STATE.FOCUSED.
screen_getanswer_btn_answer.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_getanswer_btn_answer.set_style_border_width(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_getanswer_btn_answer.set_style_radius(5, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_getanswer_btn_answer.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_getanswer_btn_answer.set_style_text_color(lv.color_hex(0x274a6e), lv.PART.MAIN|lv.STATE.FOCUSED)
screen_getanswer_btn_answer.set_style_text_font(test_font("SourceHanSansSC_Medium", 14), lv.PART.MAIN|lv.STATE.FOCUSED)
screen_getanswer_btn_answer.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.FOCUSED)
# Set style for screen_getanswer_btn_answer, Part: lv.PART.MAIN, State: lv.STATE.PRESSED.
screen_getanswer_btn_answer.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.PRESSED)
screen_getanswer_btn_answer.set_style_border_width(0, lv.PART.MAIN|lv.STATE.PRESSED)
screen_getanswer_btn_answer.set_style_radius(5, lv.PART.MAIN|lv.STATE.PRESSED)
screen_getanswer_btn_answer.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.PRESSED)
screen_getanswer_btn_answer.set_style_text_color(lv.color_hex(0x274a6e), lv.PART.MAIN|lv.STATE.PRESSED)
screen_getanswer_btn_answer.set_style_text_font(test_font("SourceHanSansSC_Medium", 14), lv.PART.MAIN|lv.STATE.PRESSED)
screen_getanswer_btn_answer.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.PRESSED)

screen_getanswer.update_layout()
# Create screen_picture
screen_picture = lv.obj()
g_kb_screen_picture = lv.keyboard(screen_picture)
g_kb_screen_picture.add_event_cb(lambda e: ta_event_cb(e, g_kb_screen_picture), lv.EVENT.ALL, None)
g_kb_screen_picture.add_flag(lv.obj.FLAG.HIDDEN)
g_kb_screen_picture.set_style_text_font(test_font("Acme_Regular", 12), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_picture.set_size(320, 170)
screen_picture.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
# Set style for screen_picture, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_picture.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_picture_img_picture
screen_picture_img_picture = lv.img(screen_picture)
screen_picture_img_picture.set_src("B:MicroPython/_word_BG1_alpha_320x170.bin")
screen_picture_img_picture.add_flag(lv.obj.FLAG.CLICKABLE)
screen_picture_img_picture.set_pivot(50,50)
screen_picture_img_picture.set_angle(0)
screen_picture_img_picture.set_pos(0, 0)
screen_picture_img_picture.set_size(320, 170)
# Set style for screen_picture_img_picture, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_picture_img_picture.set_style_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_picture_img_picture.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_picture_img_picture.set_style_clip_corner(True, lv.PART.MAIN|lv.STATE.DEFAULT)

screen_picture.update_layout()
# Create screen_gash
screen_gash = lv.obj()
g_kb_screen_gash = lv.keyboard(screen_gash)
g_kb_screen_gash.add_event_cb(lambda e: ta_event_cb(e, g_kb_screen_gash), lv.EVENT.ALL, None)
g_kb_screen_gash.add_flag(lv.obj.FLAG.HIDDEN)
g_kb_screen_gash.set_style_text_font(test_font("Acme_Regular", 12), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_gash.set_size(320, 170)
screen_gash.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
# Set style for screen_gash, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_gash.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_gash.set_style_bg_img_src("B:MicroPython/_AAA_clawmachine_bg_0905_320x170.bin", lv.PART.MAIN|lv.STATE.DEFAULT)
screen_gash.set_style_bg_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_gash.set_style_bg_img_recolor_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_gash_img_stick
screen_gash_img_stick = lv.img(screen_gash)
screen_gash_img_stick.set_src("B:MicroPython/_AAA_Handle_0905_alpha_10x16.bin")
screen_gash_img_stick.add_flag(lv.obj.FLAG.CLICKABLE)
screen_gash_img_stick.set_pivot(50,50)
screen_gash_img_stick.set_angle(0)
screen_gash_img_stick.set_pos(258, 134)
screen_gash_img_stick.set_size(10, 16)
# Set style for screen_gash_img_stick, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_gash_img_stick.set_style_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_gash_img_stick.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_gash_img_stick.set_style_clip_corner(True, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_gash_img_claw1
screen_gash_img_claw1 = lv.img(screen_gash)
screen_gash_img_claw1.set_src("B:MicroPython/_AAA_claw1_0905_alpha_15x42.bin")
screen_gash_img_claw1.add_flag(lv.obj.FLAG.CLICKABLE)
screen_gash_img_claw1.set_pivot(50,50)
screen_gash_img_claw1.set_angle(0)
screen_gash_img_claw1.set_pos(41, 8)
screen_gash_img_claw1.set_size(15, 42)
# Set style for screen_gash_img_claw1, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_gash_img_claw1.set_style_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_gash_img_claw1.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_gash_img_claw1.set_style_clip_corner(True, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_gash_img_claw2
screen_gash_img_claw2 = lv.img(screen_gash)
screen_gash_img_claw2.set_src("B:MicroPython/_AAA_claw2_0905_alpha_15x58.bin")
screen_gash_img_claw2.add_flag(lv.obj.FLAG.CLICKABLE)
screen_gash_img_claw2.set_pivot(50,50)
screen_gash_img_claw2.set_angle(0)
screen_gash_img_claw2.set_pos(41, 8)
screen_gash_img_claw2.set_size(15, 58)
# Set style for screen_gash_img_claw2, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_gash_img_claw2.set_style_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_gash_img_claw2.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_gash_img_claw2.set_style_clip_corner(True, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_gash_img_claw3
screen_gash_img_claw3 = lv.img(screen_gash)
screen_gash_img_claw3.set_src("B:MicroPython/_AAA_claw3_0905_alpha_18x58.bin")
screen_gash_img_claw3.add_flag(lv.obj.FLAG.CLICKABLE)
screen_gash_img_claw3.set_pivot(50,50)
screen_gash_img_claw3.set_angle(0)
screen_gash_img_claw3.set_pos(40, 8)
screen_gash_img_claw3.set_size(18, 58)
# Set style for screen_gash_img_claw3, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_gash_img_claw3.set_style_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_gash_img_claw3.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_gash_img_claw3.set_style_clip_corner(True, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_gash_img_ball1
screen_gash_img_ball1 = lv.img(screen_gash)
screen_gash_img_ball1.set_src("B:MicroPython/_AAA_ball1_0905_alpha_26x26.bin")
screen_gash_img_ball1.add_flag(lv.obj.FLAG.CLICKABLE)
screen_gash_img_ball1.set_pivot(50,50)
screen_gash_img_ball1.set_angle(0)
screen_gash_img_ball1.set_pos(109, 17)
screen_gash_img_ball1.set_size(26, 26)
# Set style for screen_gash_img_ball1, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_gash_img_ball1.set_style_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_gash_img_ball1.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_gash_img_ball1.set_style_clip_corner(True, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_gash_img_ball2
screen_gash_img_ball2 = lv.img(screen_gash)
screen_gash_img_ball2.set_src("B:MicroPython/_AAA_ball2_0905_alpha_26x26.bin")
screen_gash_img_ball2.add_flag(lv.obj.FLAG.CLICKABLE)
screen_gash_img_ball2.set_pivot(50,50)
screen_gash_img_ball2.set_angle(0)
screen_gash_img_ball2.set_pos(149, 17)
screen_gash_img_ball2.set_size(26, 26)
# Set style for screen_gash_img_ball2, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_gash_img_ball2.set_style_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_gash_img_ball2.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_gash_img_ball2.set_style_clip_corner(True, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_gash_img_ball3
screen_gash_img_ball3 = lv.img(screen_gash)
screen_gash_img_ball3.set_src("B:MicroPython/_AAA_ball3_0905_alpha_26x26.bin")
screen_gash_img_ball3.add_flag(lv.obj.FLAG.CLICKABLE)
screen_gash_img_ball3.set_pivot(50,50)
screen_gash_img_ball3.set_angle(0)
screen_gash_img_ball3.set_pos(185, 17)
screen_gash_img_ball3.set_size(26, 26)
# Set style for screen_gash_img_ball3, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_gash_img_ball3.set_style_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_gash_img_ball3.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_gash_img_ball3.set_style_clip_corner(True, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_gash_img_ball4
screen_gash_img_ball4 = lv.img(screen_gash)
screen_gash_img_ball4.set_src("B:MicroPython/_AAA_ball4_0905_alpha_26x26.bin")
screen_gash_img_ball4.add_flag(lv.obj.FLAG.CLICKABLE)
screen_gash_img_ball4.set_pivot(50,50)
screen_gash_img_ball4.set_angle(0)
screen_gash_img_ball4.set_pos(227, 17)
screen_gash_img_ball4.set_size(26, 26)
# Set style for screen_gash_img_ball4, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_gash_img_ball4.set_style_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_gash_img_ball4.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_gash_img_ball4.set_style_clip_corner(True, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_gash_img_randontask
screen_gash_img_randontask = lv.img(screen_gash)
screen_gash_img_randontask.set_src("B:MicroPython/_AAA_randomtask_0905_alpha_199x140.bin")
screen_gash_img_randontask.add_flag(lv.obj.FLAG.CLICKABLE)
screen_gash_img_randontask.set_pivot(50,50)
screen_gash_img_randontask.set_angle(0)
screen_gash_img_randontask.set_pos(66, 17)
screen_gash_img_randontask.set_size(199, 140)
# Set style for screen_gash_img_randontask, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_gash_img_randontask.set_style_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_gash_img_randontask.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_gash_img_randontask.set_style_clip_corner(True, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_gash_btn_addtodo
screen_gash_btn_addtodo = lv.btn(screen_gash)
screen_gash_btn_addtodo_label = lv.label(screen_gash_btn_addtodo)
screen_gash_btn_addtodo_label.set_text("Add todo")
screen_gash_btn_addtodo_label.set_long_mode(lv.label.LONG.WRAP)
screen_gash_btn_addtodo_label.set_width(lv.pct(100))
screen_gash_btn_addtodo_label.align(lv.ALIGN.CENTER, 0, 0)
screen_gash_btn_addtodo.set_style_pad_all(0, lv.STATE.DEFAULT)
screen_gash_btn_addtodo.set_pos(84, 122)
screen_gash_btn_addtodo.set_size(77, 20)
# Set style for screen_gash_btn_addtodo, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_gash_btn_addtodo.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_gash_btn_addtodo.set_style_bg_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_gash_btn_addtodo.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_gash_btn_addtodo.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_gash_btn_addtodo.set_style_radius(5, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_gash_btn_addtodo.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_gash_btn_addtodo.set_style_text_color(lv.color_hex(0x13407a), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_gash_btn_addtodo.set_style_text_font(test_font("montserratMedium", 12), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_gash_btn_addtodo.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_gash_btn_addtodo.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
# Set style for screen_gash_btn_addtodo, Part: lv.PART.MAIN, State: lv.STATE.FOCUSED.
screen_gash_btn_addtodo.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_gash_btn_addtodo.set_style_bg_color(lv.color_hex(0x13407a), lv.PART.MAIN|lv.STATE.FOCUSED)
screen_gash_btn_addtodo.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_gash_btn_addtodo.set_style_border_width(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_gash_btn_addtodo.set_style_radius(5, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_gash_btn_addtodo.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_gash_btn_addtodo.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.FOCUSED)
screen_gash_btn_addtodo.set_style_text_font(test_font("montserratMedium", 12), lv.PART.MAIN|lv.STATE.FOCUSED)
screen_gash_btn_addtodo.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.FOCUSED)
# Set style for screen_gash_btn_addtodo, Part: lv.PART.MAIN, State: lv.STATE.PRESSED.
screen_gash_btn_addtodo.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.PRESSED)
screen_gash_btn_addtodo.set_style_bg_color(lv.color_hex(0x13407a), lv.PART.MAIN|lv.STATE.PRESSED)
screen_gash_btn_addtodo.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.PRESSED)
screen_gash_btn_addtodo.set_style_border_width(0, lv.PART.MAIN|lv.STATE.PRESSED)
screen_gash_btn_addtodo.set_style_radius(5, lv.PART.MAIN|lv.STATE.PRESSED)
screen_gash_btn_addtodo.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.PRESSED)
screen_gash_btn_addtodo.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.PRESSED)
screen_gash_btn_addtodo.set_style_text_font(test_font("montserratMedium", 12), lv.PART.MAIN|lv.STATE.PRESSED)
screen_gash_btn_addtodo.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.PRESSED)

# Create screen_gash_btn_again
screen_gash_btn_again = lv.btn(screen_gash)
screen_gash_btn_again_label = lv.label(screen_gash_btn_again)
screen_gash_btn_again_label.set_text("Once again")
screen_gash_btn_again_label.set_long_mode(lv.label.LONG.WRAP)
screen_gash_btn_again_label.set_width(lv.pct(100))
screen_gash_btn_again_label.align(lv.ALIGN.CENTER, 0, 0)
screen_gash_btn_again.set_style_pad_all(0, lv.STATE.DEFAULT)
screen_gash_btn_again.set_pos(170, 122)
screen_gash_btn_again.set_size(77, 20)
# Set style for screen_gash_btn_again, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_gash_btn_again.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_gash_btn_again.set_style_bg_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_gash_btn_again.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_gash_btn_again.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_gash_btn_again.set_style_radius(5, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_gash_btn_again.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_gash_btn_again.set_style_text_color(lv.color_hex(0x13407a), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_gash_btn_again.set_style_text_font(test_font("montserratMedium", 12), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_gash_btn_again.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_gash_btn_again.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
# Set style for screen_gash_btn_again, Part: lv.PART.MAIN, State: lv.STATE.FOCUSED.
screen_gash_btn_again.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_gash_btn_again.set_style_bg_color(lv.color_hex(0x13407a), lv.PART.MAIN|lv.STATE.FOCUSED)
screen_gash_btn_again.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_gash_btn_again.set_style_border_width(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_gash_btn_again.set_style_radius(5, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_gash_btn_again.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.FOCUSED)
screen_gash_btn_again.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.FOCUSED)
screen_gash_btn_again.set_style_text_font(test_font("montserratMedium", 12), lv.PART.MAIN|lv.STATE.FOCUSED)
screen_gash_btn_again.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.FOCUSED)
# Set style for screen_gash_btn_again, Part: lv.PART.MAIN, State: lv.STATE.PRESSED.
screen_gash_btn_again.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.PRESSED)
screen_gash_btn_again.set_style_bg_color(lv.color_hex(0x13407a), lv.PART.MAIN|lv.STATE.PRESSED)
screen_gash_btn_again.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.PRESSED)
screen_gash_btn_again.set_style_border_width(0, lv.PART.MAIN|lv.STATE.PRESSED)
screen_gash_btn_again.set_style_radius(5, lv.PART.MAIN|lv.STATE.PRESSED)
screen_gash_btn_again.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.PRESSED)
screen_gash_btn_again.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.PRESSED)
screen_gash_btn_again.set_style_text_font(test_font("montserratMedium", 12), lv.PART.MAIN|lv.STATE.PRESSED)
screen_gash_btn_again.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.PRESSED)

screen_gash.update_layout()
# Create screen_game
screen_game = lv.obj()
g_kb_screen_game = lv.keyboard(screen_game)
g_kb_screen_game.add_event_cb(lambda e: ta_event_cb(e, g_kb_screen_game), lv.EVENT.ALL, None)
g_kb_screen_game.add_flag(lv.obj.FLAG.HIDDEN)
g_kb_screen_game.set_style_text_font(test_font("Acme_Regular", 12), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_game.set_size(320, 170)
screen_game.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
# Set style for screen_game, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_game.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)

screen_game.update_layout()
# Create screen_keep
screen_keep = lv.obj()
g_kb_screen_keep = lv.keyboard(screen_keep)
g_kb_screen_keep.add_event_cb(lambda e: ta_event_cb(e, g_kb_screen_keep), lv.EVENT.ALL, None)
g_kb_screen_keep.add_flag(lv.obj.FLAG.HIDDEN)
g_kb_screen_keep.set_style_text_font(test_font("Acme_Regular", 12), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_keep.set_size(320, 170)
screen_keep.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
# Set style for screen_keep, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_keep.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_keep.set_style_bg_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_keep.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_keep_img_bg
screen_keep_img_bg = lv.img(screen_keep)
screen_keep_img_bg.set_src("B:MicroPython/_AAA_keep_bg_alpha_312x137.bin")
screen_keep_img_bg.add_flag(lv.obj.FLAG.CLICKABLE)
screen_keep_img_bg.set_pivot(50,50)
screen_keep_img_bg.set_angle(0)
screen_keep_img_bg.set_pos(4, 22)
screen_keep_img_bg.set_size(312, 137)
# Set style for screen_keep_img_bg, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_keep_img_bg.set_style_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_keep_img_bg.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_keep_img_bg.set_style_clip_corner(True, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_keep_img_lv10_wither
screen_keep_img_lv10_wither = lv.img(screen_keep)
screen_keep_img_lv10_wither.set_src("B:MicroPython/_AAA_keep_LV10_wither_alpha_200x150.bin")
screen_keep_img_lv10_wither.add_flag(lv.obj.FLAG.CLICKABLE)
screen_keep_img_lv10_wither.set_pivot(50,50)
screen_keep_img_lv10_wither.set_angle(0)
screen_keep_img_lv10_wither.set_pos(0, 0)
screen_keep_img_lv10_wither.set_size(200, 150)
screen_keep_img_lv10_wither.add_flag(lv.obj.FLAG.HIDDEN)
# Set style for screen_keep_img_lv10_wither, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_keep_img_lv10_wither.set_style_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_keep_img_lv10_wither.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_keep_img_lv10_wither.set_style_clip_corner(True, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_keep_img_lv10
screen_keep_img_lv10 = lv.img(screen_keep)
screen_keep_img_lv10.set_src("B:MicroPython/_AAA_keep_LV10_alpha_184x160.bin")
screen_keep_img_lv10.add_flag(lv.obj.FLAG.CLICKABLE)
screen_keep_img_lv10.set_pivot(50,50)
screen_keep_img_lv10.set_angle(0)
screen_keep_img_lv10.set_pos(0, 0)
screen_keep_img_lv10.set_size(184, 160)
screen_keep_img_lv10.add_flag(lv.obj.FLAG.HIDDEN)
# Set style for screen_keep_img_lv10, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_keep_img_lv10.set_style_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_keep_img_lv10.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_keep_img_lv10.set_style_clip_corner(True, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_keep_img_lv9_wither
screen_keep_img_lv9_wither = lv.img(screen_keep)
screen_keep_img_lv9_wither.set_src("B:MicroPython/_AAA_keep_LV9_wither_alpha_170x155.bin")
screen_keep_img_lv9_wither.add_flag(lv.obj.FLAG.CLICKABLE)
screen_keep_img_lv9_wither.set_pivot(50,50)
screen_keep_img_lv9_wither.set_angle(0)
screen_keep_img_lv9_wither.set_pos(0, 1)
screen_keep_img_lv9_wither.set_size(170, 155)
screen_keep_img_lv9_wither.add_flag(lv.obj.FLAG.HIDDEN)
# Set style for screen_keep_img_lv9_wither, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_keep_img_lv9_wither.set_style_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_keep_img_lv9_wither.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_keep_img_lv9_wither.set_style_clip_corner(True, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_keep_img_lv9
screen_keep_img_lv9 = lv.img(screen_keep)
screen_keep_img_lv9.set_src("B:MicroPython/_AAA_keep_LV9_alpha_165x161.bin")
screen_keep_img_lv9.add_flag(lv.obj.FLAG.CLICKABLE)
screen_keep_img_lv9.set_pivot(50,50)
screen_keep_img_lv9.set_angle(0)
screen_keep_img_lv9.set_pos(0, 0)
screen_keep_img_lv9.set_size(165, 161)
screen_keep_img_lv9.add_flag(lv.obj.FLAG.HIDDEN)
# Set style for screen_keep_img_lv9, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_keep_img_lv9.set_style_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_keep_img_lv9.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_keep_img_lv9.set_style_clip_corner(True, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_keep_img_lv8_wither
screen_keep_img_lv8_wither = lv.img(screen_keep)
screen_keep_img_lv8_wither.set_src("B:MicroPython/_AAA_keep_LV8_wither_alpha_154x170.bin")
screen_keep_img_lv8_wither.add_flag(lv.obj.FLAG.CLICKABLE)
screen_keep_img_lv8_wither.set_pivot(50,50)
screen_keep_img_lv8_wither.set_angle(0)
screen_keep_img_lv8_wither.set_pos(0, 0)
screen_keep_img_lv8_wither.set_size(154, 170)
screen_keep_img_lv8_wither.add_flag(lv.obj.FLAG.HIDDEN)
# Set style for screen_keep_img_lv8_wither, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_keep_img_lv8_wither.set_style_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_keep_img_lv8_wither.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_keep_img_lv8_wither.set_style_clip_corner(True, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_keep_img_lv8
screen_keep_img_lv8 = lv.img(screen_keep)
screen_keep_img_lv8.set_src("B:MicroPython/_AAA_keep_LV8_alpha_154x170.bin")
screen_keep_img_lv8.add_flag(lv.obj.FLAG.CLICKABLE)
screen_keep_img_lv8.set_pivot(50,50)
screen_keep_img_lv8.set_angle(0)
screen_keep_img_lv8.set_pos(0, 0)
screen_keep_img_lv8.set_size(154, 170)
screen_keep_img_lv8.add_flag(lv.obj.FLAG.HIDDEN)
# Set style for screen_keep_img_lv8, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_keep_img_lv8.set_style_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_keep_img_lv8.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_keep_img_lv8.set_style_clip_corner(True, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_keep_img_lv7_wither
screen_keep_img_lv7_wither = lv.img(screen_keep)
screen_keep_img_lv7_wither.set_src("B:MicroPython/_AAA_keep_LV7_wither_alpha_186x170.bin")
screen_keep_img_lv7_wither.add_flag(lv.obj.FLAG.CLICKABLE)
screen_keep_img_lv7_wither.set_pivot(50,50)
screen_keep_img_lv7_wither.set_angle(0)
screen_keep_img_lv7_wither.set_pos(0, 0)
screen_keep_img_lv7_wither.set_size(186, 170)
screen_keep_img_lv7_wither.add_flag(lv.obj.FLAG.HIDDEN)
# Set style for screen_keep_img_lv7_wither, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_keep_img_lv7_wither.set_style_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_keep_img_lv7_wither.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_keep_img_lv7_wither.set_style_clip_corner(True, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_keep_img_lv7
screen_keep_img_lv7 = lv.img(screen_keep)
screen_keep_img_lv7.set_src("B:MicroPython/_AAA_keep_LV7_alpha_186x170.bin")
screen_keep_img_lv7.add_flag(lv.obj.FLAG.CLICKABLE)
screen_keep_img_lv7.set_pivot(50,50)
screen_keep_img_lv7.set_angle(0)
screen_keep_img_lv7.set_pos(0, 0)
screen_keep_img_lv7.set_size(186, 170)
screen_keep_img_lv7.add_flag(lv.obj.FLAG.HIDDEN)
# Set style for screen_keep_img_lv7, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_keep_img_lv7.set_style_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_keep_img_lv7.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_keep_img_lv7.set_style_clip_corner(True, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_keep_img_lv6_wither
screen_keep_img_lv6_wither = lv.img(screen_keep)
screen_keep_img_lv6_wither.set_src("B:MicroPython/_AAA_keep_LV6_wither_alpha_180x160.bin")
screen_keep_img_lv6_wither.add_flag(lv.obj.FLAG.CLICKABLE)
screen_keep_img_lv6_wither.set_pivot(50,50)
screen_keep_img_lv6_wither.set_angle(0)
screen_keep_img_lv6_wither.set_pos(0, 5)
screen_keep_img_lv6_wither.set_size(180, 160)
screen_keep_img_lv6_wither.add_flag(lv.obj.FLAG.HIDDEN)
# Set style for screen_keep_img_lv6_wither, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_keep_img_lv6_wither.set_style_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_keep_img_lv6_wither.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_keep_img_lv6_wither.set_style_clip_corner(True, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_keep_img_lv6
screen_keep_img_lv6 = lv.img(screen_keep)
screen_keep_img_lv6.set_src("B:MicroPython/_AAA_keep_LV6_alpha_189x160.bin")
screen_keep_img_lv6.add_flag(lv.obj.FLAG.CLICKABLE)
screen_keep_img_lv6.set_pivot(50,50)
screen_keep_img_lv6.set_angle(0)
screen_keep_img_lv6.set_pos(0, 5)
screen_keep_img_lv6.set_size(189, 160)
screen_keep_img_lv6.add_flag(lv.obj.FLAG.HIDDEN)
# Set style for screen_keep_img_lv6, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_keep_img_lv6.set_style_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_keep_img_lv6.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_keep_img_lv6.set_style_clip_corner(True, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_keep_img_lv5_wither
screen_keep_img_lv5_wither = lv.img(screen_keep)
screen_keep_img_lv5_wither.set_src("B:MicroPython/_AAA_keep_LV5_wither_alpha_170x158.bin")
screen_keep_img_lv5_wither.add_flag(lv.obj.FLAG.CLICKABLE)
screen_keep_img_lv5_wither.set_pivot(50,50)
screen_keep_img_lv5_wither.set_angle(0)
screen_keep_img_lv5_wither.set_pos(0, 5)
screen_keep_img_lv5_wither.set_size(170, 158)
screen_keep_img_lv5_wither.add_flag(lv.obj.FLAG.HIDDEN)
# Set style for screen_keep_img_lv5_wither, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_keep_img_lv5_wither.set_style_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_keep_img_lv5_wither.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_keep_img_lv5_wither.set_style_clip_corner(True, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_keep_img_lv5
screen_keep_img_lv5 = lv.img(screen_keep)
screen_keep_img_lv5.set_src("B:MicroPython/_AAA_keep_LV5_alpha_170x158.bin")
screen_keep_img_lv5.add_flag(lv.obj.FLAG.CLICKABLE)
screen_keep_img_lv5.set_pivot(50,50)
screen_keep_img_lv5.set_angle(0)
screen_keep_img_lv5.set_pos(0, 5)
screen_keep_img_lv5.set_size(170, 158)
screen_keep_img_lv5.add_flag(lv.obj.FLAG.HIDDEN)
# Set style for screen_keep_img_lv5, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_keep_img_lv5.set_style_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_keep_img_lv5.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_keep_img_lv5.set_style_clip_corner(True, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_keep_img_lv4_wither
screen_keep_img_lv4_wither = lv.img(screen_keep)
screen_keep_img_lv4_wither.set_src("B:MicroPython/_AAA_keep_LV4_wither_alpha_164x152.bin")
screen_keep_img_lv4_wither.add_flag(lv.obj.FLAG.CLICKABLE)
screen_keep_img_lv4_wither.set_pivot(50,50)
screen_keep_img_lv4_wither.set_angle(0)
screen_keep_img_lv4_wither.set_pos(0, 0)
screen_keep_img_lv4_wither.set_size(164, 152)
screen_keep_img_lv4_wither.add_flag(lv.obj.FLAG.HIDDEN)
# Set style for screen_keep_img_lv4_wither, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_keep_img_lv4_wither.set_style_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_keep_img_lv4_wither.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_keep_img_lv4_wither.set_style_clip_corner(True, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_keep_img_lv4
screen_keep_img_lv4 = lv.img(screen_keep)
screen_keep_img_lv4.set_src("B:MicroPython/_AAA_keep_LV4_alpha_164x149.bin")
screen_keep_img_lv4.add_flag(lv.obj.FLAG.CLICKABLE)
screen_keep_img_lv4.set_pivot(50,50)
screen_keep_img_lv4.set_angle(0)
screen_keep_img_lv4.set_pos(0, 1)
screen_keep_img_lv4.set_size(164, 149)
screen_keep_img_lv4.add_flag(lv.obj.FLAG.HIDDEN)
# Set style for screen_keep_img_lv4, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_keep_img_lv4.set_style_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_keep_img_lv4.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_keep_img_lv4.set_style_clip_corner(True, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_keep_img_lv3_wither
screen_keep_img_lv3_wither = lv.img(screen_keep)
screen_keep_img_lv3_wither.set_src("B:MicroPython/_AAA_keep_LV3_wither_alpha_114x147.bin")
screen_keep_img_lv3_wither.add_flag(lv.obj.FLAG.CLICKABLE)
screen_keep_img_lv3_wither.set_pivot(50,50)
screen_keep_img_lv3_wither.set_angle(0)
screen_keep_img_lv3_wither.set_pos(9, 3)
screen_keep_img_lv3_wither.set_size(114, 147)
screen_keep_img_lv3_wither.add_flag(lv.obj.FLAG.HIDDEN)
# Set style for screen_keep_img_lv3_wither, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_keep_img_lv3_wither.set_style_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_keep_img_lv3_wither.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_keep_img_lv3_wither.set_style_clip_corner(True, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_keep_img_lv3
screen_keep_img_lv3 = lv.img(screen_keep)
screen_keep_img_lv3.set_src("B:MicroPython/_AAA_keep_LV3_alpha_114x147.bin")
screen_keep_img_lv3.add_flag(lv.obj.FLAG.CLICKABLE)
screen_keep_img_lv3.set_pivot(50,50)
screen_keep_img_lv3.set_angle(0)
screen_keep_img_lv3.set_pos(10, 5)
screen_keep_img_lv3.set_size(114, 147)
screen_keep_img_lv3.add_flag(lv.obj.FLAG.HIDDEN)
# Set style for screen_keep_img_lv3, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_keep_img_lv3.set_style_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_keep_img_lv3.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_keep_img_lv3.set_style_clip_corner(True, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_keep_img_lv2_wither
screen_keep_img_lv2_wither = lv.img(screen_keep)
screen_keep_img_lv2_wither.set_src("B:MicroPython/_AAA_keep_LV2_wither_alpha_88x141.bin")
screen_keep_img_lv2_wither.add_flag(lv.obj.FLAG.CLICKABLE)
screen_keep_img_lv2_wither.set_pivot(50,50)
screen_keep_img_lv2_wither.set_angle(0)
screen_keep_img_lv2_wither.set_pos(16, 11)
screen_keep_img_lv2_wither.set_size(88, 141)
screen_keep_img_lv2_wither.add_flag(lv.obj.FLAG.HIDDEN)
# Set style for screen_keep_img_lv2_wither, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_keep_img_lv2_wither.set_style_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_keep_img_lv2_wither.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_keep_img_lv2_wither.set_style_clip_corner(True, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_keep_img_lv2
screen_keep_img_lv2 = lv.img(screen_keep)
screen_keep_img_lv2.set_src("B:MicroPython/_AAA_keep_LV2_alpha_88x141.bin")
screen_keep_img_lv2.add_flag(lv.obj.FLAG.CLICKABLE)
screen_keep_img_lv2.set_pivot(50,50)
screen_keep_img_lv2.set_angle(0)
screen_keep_img_lv2.set_pos(16, 11)
screen_keep_img_lv2.set_size(88, 141)
screen_keep_img_lv2.add_flag(lv.obj.FLAG.HIDDEN)
# Set style for screen_keep_img_lv2, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_keep_img_lv2.set_style_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_keep_img_lv2.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_keep_img_lv2.set_style_clip_corner(True, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_keep_img_lv1
screen_keep_img_lv1 = lv.img(screen_keep)
screen_keep_img_lv1.set_src("B:MicroPython/_AAA_keep_LV1_alpha_96x107.bin")
screen_keep_img_lv1.add_flag(lv.obj.FLAG.CLICKABLE)
screen_keep_img_lv1.set_pivot(50,50)
screen_keep_img_lv1.set_angle(0)
screen_keep_img_lv1.set_pos(12, 44)
screen_keep_img_lv1.set_size(96, 107)
# Set style for screen_keep_img_lv1, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_keep_img_lv1.set_style_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_keep_img_lv1.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_keep_img_lv1.set_style_clip_corner(True, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_keep_img_lv1_wither
screen_keep_img_lv1_wither = lv.img(screen_keep)
screen_keep_img_lv1_wither.set_src("B:MicroPython/_AAA_keep_LV1_wither_alpha_94x106.bin")
screen_keep_img_lv1_wither.add_flag(lv.obj.FLAG.CLICKABLE)
screen_keep_img_lv1_wither.set_pivot(50,50)
screen_keep_img_lv1_wither.set_angle(0)
screen_keep_img_lv1_wither.set_pos(12, 45)
screen_keep_img_lv1_wither.set_size(94, 106)
screen_keep_img_lv1_wither.add_flag(lv.obj.FLAG.HIDDEN)
# Set style for screen_keep_img_lv1_wither, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_keep_img_lv1_wither.set_style_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_keep_img_lv1_wither.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_keep_img_lv1_wither.set_style_clip_corner(True, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_keep_bar_focustime
screen_keep_bar_focustime = lv.bar(screen_keep)
screen_keep_bar_focustime.set_style_anim_time(1000, 0)
screen_keep_bar_focustime.set_mode(lv.bar.MODE.NORMAL)
screen_keep_bar_focustime.set_range(0, 100)
screen_keep_bar_focustime.set_value(50, lv.ANIM.OFF)
screen_keep_bar_focustime.set_pos(9, 150)
screen_keep_bar_focustime.set_size(297, 4)
# Set style for screen_keep_bar_focustime, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_keep_bar_focustime.set_style_bg_opa(60, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_keep_bar_focustime.set_style_bg_color(lv.color_hex(0x4d4e4e), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_keep_bar_focustime.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_keep_bar_focustime.set_style_radius(10, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_keep_bar_focustime.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
# Set style for screen_keep_bar_focustime, Part: lv.PART.INDICATOR, State: lv.STATE.DEFAULT.
screen_keep_bar_focustime.set_style_bg_opa(255, lv.PART.INDICATOR|lv.STATE.DEFAULT)
screen_keep_bar_focustime.set_style_bg_color(lv.color_hex(0x555d62), lv.PART.INDICATOR|lv.STATE.DEFAULT)
screen_keep_bar_focustime.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.INDICATOR|lv.STATE.DEFAULT)
screen_keep_bar_focustime.set_style_radius(10, lv.PART.INDICATOR|lv.STATE.DEFAULT)

# Create screen_keep_img_process_icon
screen_keep_img_process_icon = lv.img(screen_keep)
screen_keep_img_process_icon.set_src("B:MicroPython/_AAA_keep_icon_alpha_14x14.bin")
screen_keep_img_process_icon.add_flag(lv.obj.FLAG.CLICKABLE)
screen_keep_img_process_icon.set_pivot(50,50)
screen_keep_img_process_icon.set_angle(0)
screen_keep_img_process_icon.set_pos(297, 134)
screen_keep_img_process_icon.set_size(14, 14)
# Set style for screen_keep_img_process_icon, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_keep_img_process_icon.set_style_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_keep_img_process_icon.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_keep_img_process_icon.set_style_clip_corner(True, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_keep_img_dialog
screen_keep_img_dialog = lv.img(screen_keep)
screen_keep_img_dialog.set_src("B:MicroPython/_AAA_keep_dialogbox_alpha_204x78.bin")
screen_keep_img_dialog.add_flag(lv.obj.FLAG.CLICKABLE)
screen_keep_img_dialog.set_pivot(50,50)
screen_keep_img_dialog.set_angle(0)
screen_keep_img_dialog.set_pos(102, 35)
screen_keep_img_dialog.set_size(204, 78)
# Set style for screen_keep_img_dialog, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_keep_img_dialog.set_style_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_keep_img_dialog.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_keep_img_dialog.set_style_clip_corner(True, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_keep_label_state
screen_keep_label_state = lv.label(screen_keep)
screen_keep_label_state.set_text("First Sprout")
screen_keep_label_state.set_long_mode(lv.label.LONG.WRAP)
screen_keep_label_state.set_width(lv.pct(100))
screen_keep_label_state.set_pos(200, 2)
screen_keep_label_state.set_size(114, 18)
# Set style for screen_keep_label_state, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_keep_label_state.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_keep_label_state.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_keep_label_state.set_style_text_color(lv.color_hex(0x000000), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_keep_label_state.set_style_text_font(test_font("SourceHanSansSC_Medium", 16), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_keep_label_state.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_keep_label_state.set_style_text_letter_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_keep_label_state.set_style_text_line_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_keep_label_state.set_style_text_align(lv.TEXT_ALIGN.RIGHT, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_keep_label_state.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_keep_label_state.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_keep_label_state.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_keep_label_state.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_keep_label_state.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_keep_label_state.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_keep_label_conversion
screen_keep_label_conversion = lv.label(screen_keep)
screen_keep_label_conversion.set_text("You're doing a great job in taking care of me !")
screen_keep_label_conversion.set_long_mode(lv.label.LONG.WRAP)
screen_keep_label_conversion.set_width(lv.pct(100))
screen_keep_label_conversion.set_pos(116, 48)
screen_keep_label_conversion.set_size(187, 44)
# Set style for screen_keep_label_conversion, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_keep_label_conversion.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_keep_label_conversion.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_keep_label_conversion.set_style_text_color(lv.color_hex(0x000000), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_keep_label_conversion.set_style_text_font(test_font("arial", 14), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_keep_label_conversion.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_keep_label_conversion.set_style_text_letter_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_keep_label_conversion.set_style_text_line_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_keep_label_conversion.set_style_text_align(lv.TEXT_ALIGN.LEFT, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_keep_label_conversion.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_keep_label_conversion.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_keep_label_conversion.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_keep_label_conversion.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_keep_label_conversion.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_keep_label_conversion.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_keep_label_focustime
screen_keep_label_focustime = lv.label(screen_keep)
screen_keep_label_focustime.set_text("0/180 MIN")
screen_keep_label_focustime.set_long_mode(lv.label.LONG.WRAP)
screen_keep_label_focustime.set_width(lv.pct(100))
screen_keep_label_focustime.set_pos(225, 156)
screen_keep_label_focustime.set_size(80, 11)
# Set style for screen_keep_label_focustime, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_keep_label_focustime.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_keep_label_focustime.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_keep_label_focustime.set_style_text_color(lv.color_hex(0x000000), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_keep_label_focustime.set_style_text_font(test_font("SourceHanSansSC_Normal", 10), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_keep_label_focustime.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_keep_label_focustime.set_style_text_letter_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_keep_label_focustime.set_style_text_line_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_keep_label_focustime.set_style_text_align(lv.TEXT_ALIGN.RIGHT, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_keep_label_focustime.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_keep_label_focustime.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_keep_label_focustime.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_keep_label_focustime.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_keep_label_focustime.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_keep_label_focustime.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

screen_keep.update_layout()

def screen_reppo_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.SCREEN_LOAD_START):
        pass
        

screen_reppo.add_event_cb(lambda e: screen_reppo_event_handler(e), lv.EVENT.ALL, None)

def screen_main_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.SCREEN_LOAD_START):
        pass
        

screen_main.add_event_cb(lambda e: screen_main_event_handler(e), lv.EVENT.ALL, None)

def screen_main_btn_todolist_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        lv.scr_load_anim(screen_todolist, lv.SCR_LOAD_ANIM.FADE_ON, 100, 100, False)
    if (code == lv.EVENT.FOCUSED):
        pass
        

    if (code == lv.EVENT.DEFOCUSED):
        pass
        

screen_main_btn_todolist.add_event_cb(lambda e: screen_main_btn_todolist_event_handler(e), lv.EVENT.ALL, None)

def screen_main_btn_done_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        lv.scr_load_anim(screen_donelist, lv.SCR_LOAD_ANIM.FADE_ON, 100, 100, False)
    if (code == lv.EVENT.FOCUSED):
        pass
        

    if (code == lv.EVENT.DEFOCUSED):
        pass
        

screen_main_btn_done.add_event_cb(lambda e: screen_main_btn_done_event_handler(e), lv.EVENT.ALL, None)

def screen_main_btn_timer_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        lv.scr_load_anim(screen_pomodoro, lv.SCR_LOAD_ANIM.FADE_ON, 100, 100, False)
    if (code == lv.EVENT.FOCUSED):
        pass
        

    if (code == lv.EVENT.DEFOCUSED):
        pass
        

screen_main_btn_timer.add_event_cb(lambda e: screen_main_btn_timer_event_handler(e), lv.EVENT.ALL, None)

def screen_main_btn_ai_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        lv.scr_load_anim(screen_keep, lv.SCR_LOAD_ANIM.FADE_ON, 100, 100, False)
    if (code == lv.EVENT.FOCUSED):
        pass
        

    if (code == lv.EVENT.DEFOCUSED):
        pass
        

screen_main_btn_ai.add_event_cb(lambda e: screen_main_btn_ai_event_handler(e), lv.EVENT.ALL, None)

def screen_main_btn_gashapon_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.FOCUSED):
        pass
        

    if (code == lv.EVENT.DEFOCUSED):
        pass
        

    if (code == lv.EVENT.CLICKED):
        pass
        lv.scr_load_anim(screen_gash, lv.SCR_LOAD_ANIM.FADE_ON, 100, 100, False)
screen_main_btn_gashapon.add_event_cb(lambda e: screen_main_btn_gashapon_event_handler(e), lv.EVENT.ALL, None)

def screen_main_btn_word_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        lv.scr_load_anim(screen_wordmeun, lv.SCR_LOAD_ANIM.FADE_ON, 100, 100, False)
    if (code == lv.EVENT.FOCUSED):
        pass
        

    if (code == lv.EVENT.DEFOCUSED):
        pass
        

screen_main_btn_word.add_event_cb(lambda e: screen_main_btn_word_event_handler(e), lv.EVENT.ALL, None)

def screen_main_btn_fun_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        lv.scr_load_anim(screen_answer, lv.SCR_LOAD_ANIM.FADE_ON, 100, 100, False)
    if (code == lv.EVENT.FOCUSED):
        pass
        

    if (code == lv.EVENT.DEFOCUSED):
        pass
        

screen_main_btn_fun.add_event_cb(lambda e: screen_main_btn_fun_event_handler(e), lv.EVENT.ALL, None)

def screen_main_btn_photo_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        lv.scr_load_anim(screen_picture, lv.SCR_LOAD_ANIM.FADE_ON, 100, 100, False)
    if (code == lv.EVENT.FOCUSED):
        pass
        

    if (code == lv.EVENT.DEFOCUSED):
        pass
        

screen_main_btn_photo.add_event_cb(lambda e: screen_main_btn_photo_event_handler(e), lv.EVENT.ALL, None)

def screen_main_btn_game_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.FOCUSED):
        pass
        

    if (code == lv.EVENT.DEFOCUSED):
        pass
        

    if (code == lv.EVENT.CLICKED):
        pass
        lv.scr_load_anim(screen_game, lv.SCR_LOAD_ANIM.FADE_ON, 100, 100, False)
screen_main_btn_game.add_event_cb(lambda e: screen_main_btn_game_event_handler(e), lv.EVENT.ALL, None)

def screen_main_btn_setting_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        lv.scr_load_anim(screen_setting, lv.SCR_LOAD_ANIM.FADE_ON, 100, 100, False)
    if (code == lv.EVENT.FOCUSED):
        pass
        

    if (code == lv.EVENT.DEFOCUSED):
        pass
        

screen_main_btn_setting.add_event_cb(lambda e: screen_main_btn_setting_event_handler(e), lv.EVENT.ALL, None)

def screen_main_btn_realtime_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        lv.scr_load_anim(screen_timing, lv.SCR_LOAD_ANIM.FADE_ON, 100, 100, False)
    if (code == lv.EVENT.FOCUSED):
        pass
        

    if (code == lv.EVENT.DEFOCUSED):
        pass
        

screen_main_btn_realtime.add_event_cb(lambda e: screen_main_btn_realtime_event_handler(e), lv.EVENT.ALL, None)

def screen_todolist_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.SCREEN_LOAD_START):
        pass
        

screen_todolist.add_event_cb(lambda e: screen_todolist_event_handler(e), lv.EVENT.ALL, None)

def screen_todolist_btn_back_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        lv.scr_load_anim(screen_main, lv.SCR_LOAD_ANIM.FADE_ON, 100, 100, False)
        

screen_todolist_btn_back.add_event_cb(lambda e: screen_todolist_btn_back_event_handler(e), lv.EVENT.ALL, None)

def screen_todolist_btn_todo1_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        

    if (code == lv.EVENT.FOCUSED):
        pass
        #Write animation: screen_todolist_btn_todo1 y
        screen_todolist_btn_todo1_anim_y = lv.anim_t()
        screen_todolist_btn_todo1_anim_y.init()
        screen_todolist_btn_todo1_anim_y.set_var(screen_todolist_btn_todo1)
        screen_todolist_btn_todo1_anim_y.set_time(0)
        screen_todolist_btn_todo1_anim_y.set_delay(0)
        screen_todolist_btn_todo1_anim_y.set_custom_exec_cb(lambda e,val: anim_y_cb(screen_todolist_btn_todo1,val))
        screen_todolist_btn_todo1_anim_y.set_values(screen_todolist_btn_todo1.get_y(), 40)
        screen_todolist_btn_todo1_anim_y.set_path_cb(lv.anim_t.path_linear)
        screen_todolist_btn_todo1_anim_y.set_repeat_count(1)
        screen_todolist_btn_todo1_anim_y.set_repeat_delay(0)
        screen_todolist_btn_todo1_anim_y.set_playback_time(200)
        screen_todolist_btn_todo1_anim_y.set_playback_delay(0)
        screen_todolist_btn_todo1_anim_y.start()
    if (code == lv.EVENT.DEFOCUSED):
        pass
        screen_todolist_btn_todo1.set_y(51)
screen_todolist_btn_todo1.add_event_cb(lambda e: screen_todolist_btn_todo1_event_handler(e), lv.EVENT.ALL, None)

def screen_todolist_btn_todo2_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        

    if (code == lv.EVENT.FOCUSED):
        pass
        #Write animation: screen_todolist_btn_todo2 y
        screen_todolist_btn_todo2_anim_y = lv.anim_t()
        screen_todolist_btn_todo2_anim_y.init()
        screen_todolist_btn_todo2_anim_y.set_var(screen_todolist_btn_todo2)
        screen_todolist_btn_todo2_anim_y.set_time(0)
        screen_todolist_btn_todo2_anim_y.set_delay(0)
        screen_todolist_btn_todo2_anim_y.set_custom_exec_cb(lambda e,val: anim_y_cb(screen_todolist_btn_todo2,val))
        screen_todolist_btn_todo2_anim_y.set_values(screen_todolist_btn_todo2.get_y(), 40)
        screen_todolist_btn_todo2_anim_y.set_path_cb(lv.anim_t.path_linear)
        screen_todolist_btn_todo2_anim_y.set_repeat_count(1)
        screen_todolist_btn_todo2_anim_y.set_repeat_delay(0)
        screen_todolist_btn_todo2_anim_y.set_playback_time(200)
        screen_todolist_btn_todo2_anim_y.set_playback_delay(0)
        screen_todolist_btn_todo2_anim_y.start()
    if (code == lv.EVENT.DEFOCUSED):
        pass
        screen_todolist_btn_todo2.set_y(51)
screen_todolist_btn_todo2.add_event_cb(lambda e: screen_todolist_btn_todo2_event_handler(e), lv.EVENT.ALL, None)

def screen_todolist_btn_todo3_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        

    if (code == lv.EVENT.FOCUSED):
        pass
        #Write animation: screen_todolist_btn_todo3 y
        screen_todolist_btn_todo3_anim_y = lv.anim_t()
        screen_todolist_btn_todo3_anim_y.init()
        screen_todolist_btn_todo3_anim_y.set_var(screen_todolist_btn_todo3)
        screen_todolist_btn_todo3_anim_y.set_time(0)
        screen_todolist_btn_todo3_anim_y.set_delay(0)
        screen_todolist_btn_todo3_anim_y.set_custom_exec_cb(lambda e,val: anim_y_cb(screen_todolist_btn_todo3,val))
        screen_todolist_btn_todo3_anim_y.set_values(screen_todolist_btn_todo3.get_y(), 93)
        screen_todolist_btn_todo3_anim_y.set_path_cb(lv.anim_t.path_linear)
        screen_todolist_btn_todo3_anim_y.set_repeat_count(1)
        screen_todolist_btn_todo3_anim_y.set_repeat_delay(0)
        screen_todolist_btn_todo3_anim_y.set_playback_time(200)
        screen_todolist_btn_todo3_anim_y.set_playback_delay(0)
        screen_todolist_btn_todo3_anim_y.start()
    if (code == lv.EVENT.DEFOCUSED):
        pass
        screen_todolist_btn_todo3.set_y(104)
screen_todolist_btn_todo3.add_event_cb(lambda e: screen_todolist_btn_todo3_event_handler(e), lv.EVENT.ALL, None)

def screen_todolist_btn_todo4_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        

    if (code == lv.EVENT.FOCUSED):
        pass
        #Write animation: screen_todolist_btn_todo4 y
        screen_todolist_btn_todo4_anim_y = lv.anim_t()
        screen_todolist_btn_todo4_anim_y.init()
        screen_todolist_btn_todo4_anim_y.set_var(screen_todolist_btn_todo4)
        screen_todolist_btn_todo4_anim_y.set_time(0)
        screen_todolist_btn_todo4_anim_y.set_delay(0)
        screen_todolist_btn_todo4_anim_y.set_custom_exec_cb(lambda e,val: anim_y_cb(screen_todolist_btn_todo4,val))
        screen_todolist_btn_todo4_anim_y.set_values(screen_todolist_btn_todo4.get_y(), 93)
        screen_todolist_btn_todo4_anim_y.set_path_cb(lv.anim_t.path_linear)
        screen_todolist_btn_todo4_anim_y.set_repeat_count(1)
        screen_todolist_btn_todo4_anim_y.set_repeat_delay(0)
        screen_todolist_btn_todo4_anim_y.set_playback_time(200)
        screen_todolist_btn_todo4_anim_y.set_playback_delay(0)
        screen_todolist_btn_todo4_anim_y.start()
    if (code == lv.EVENT.DEFOCUSED):
        pass
        screen_todolist_btn_todo4.set_y(104)
screen_todolist_btn_todo4.add_event_cb(lambda e: screen_todolist_btn_todo4_event_handler(e), lv.EVENT.ALL, None)

def screen_todolist_btn_lastpage_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        

screen_todolist_btn_lastpage.add_event_cb(lambda e: screen_todolist_btn_lastpage_event_handler(e), lv.EVENT.ALL, None)

def screen_todolist_btn_nxtpage_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        

screen_todolist_btn_nxtpage.add_event_cb(lambda e: screen_todolist_btn_nxtpage_event_handler(e), lv.EVENT.ALL, None)

def screen_todosel_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.SCREEN_LOAD_START):
        pass
        

screen_todosel.add_event_cb(lambda e: screen_todosel_event_handler(e), lv.EVENT.ALL, None)

def screen_todosel_btn_complete_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        lv.scr_load_anim(screen_todolist, lv.SCR_LOAD_ANIM.FADE_ON, 100, 100, False)
        

screen_todosel_btn_complete.add_event_cb(lambda e: screen_todosel_btn_complete_event_handler(e), lv.EVENT.ALL, None)

def screen_todosel_btn_focus_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        lv.scr_load_anim(screen_pomodoro, lv.SCR_LOAD_ANIM.FADE_ON, 100, 100, False)
        

screen_todosel_btn_focus.add_event_cb(lambda e: screen_todosel_btn_focus_event_handler(e), lv.EVENT.ALL, None)

def screen_todosel_btn_cancel_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        lv.scr_load_anim(screen_todolist, lv.SCR_LOAD_ANIM.FADE_ON, 100, 100, False)
        

screen_todosel_btn_cancel.add_event_cb(lambda e: screen_todosel_btn_cancel_event_handler(e), lv.EVENT.ALL, None)

def screen_todoback_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.SCREEN_LOAD_START):
        pass
        

screen_todoback.add_event_cb(lambda e: screen_todoback_event_handler(e), lv.EVENT.ALL, None)

def screen_todoback_btn_cancel_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        lv.scr_load_anim(screen_todolist, lv.SCR_LOAD_ANIM.FADE_ON, 100, 100, False)
        

screen_todoback_btn_cancel.add_event_cb(lambda e: screen_todoback_btn_cancel_event_handler(e), lv.EVENT.ALL, None)

def screen_todoback_btn_recover_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        lv.scr_load_anim(screen_todolist, lv.SCR_LOAD_ANIM.FADE_ON, 100, 100, False)
        

screen_todoback_btn_recover.add_event_cb(lambda e: screen_todoback_btn_recover_event_handler(e), lv.EVENT.ALL, None)

def screen_wordcard_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.SCREEN_LOAD_START):
        pass
        

screen_wordcard.add_event_cb(lambda e: screen_wordcard_event_handler(e), lv.EVENT.ALL, None)

def screen_wordcard_btn_dui_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        

    if (code == lv.EVENT.FOCUSED):
        pass
        

screen_wordcard_btn_dui.add_event_cb(lambda e: screen_wordcard_btn_dui_event_handler(e), lv.EVENT.ALL, None)

def screen_wordcard_btn_cuo_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        

    if (code == lv.EVENT.FOCUSED):
        pass
        

screen_wordcard_btn_cuo.add_event_cb(lambda e: screen_wordcard_btn_cuo_event_handler(e), lv.EVENT.ALL, None)

def screen_wordcard_btn_back_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        lv.scr_load_anim(screen_wordmeun, lv.SCR_LOAD_ANIM.FADE_ON, 100, 100, False)
    if (code == lv.EVENT.FOCUSED):
        pass
        

screen_wordcard_btn_back.add_event_cb(lambda e: screen_wordcard_btn_back_event_handler(e), lv.EVENT.ALL, None)

def screen_timing_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.SCREEN_LOAD_START):
        pass
        

    if (code == lv.EVENT.SCREEN_LOADED):
        pass
        

    if (code == lv.EVENT.SCREEN_UNLOADED):
        pass
        

screen_timing.add_event_cb(lambda e: screen_timing_event_handler(e), lv.EVENT.ALL, None)

def screen_timing_btn_main_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        lv.scr_load_anim(screen_main, lv.SCR_LOAD_ANIM.FADE_ON, 100, 100, False)
        

screen_timing_btn_main.add_event_cb(lambda e: screen_timing_btn_main_event_handler(e), lv.EVENT.ALL, None)

def screen_wordmeun_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.SCREEN_LOAD_START):
        pass
        

screen_wordmeun.add_event_cb(lambda e: screen_wordmeun_event_handler(e), lv.EVENT.ALL, None)

def screen_wordmeun_btn_learn_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        lv.scr_load_anim(screen_wordcard, lv.SCR_LOAD_ANIM.FADE_ON, 100, 100, False)
screen_wordmeun_btn_learn.add_event_cb(lambda e: screen_wordmeun_btn_learn_event_handler(e), lv.EVENT.ALL, None)

def screen_wordmeun_btn_review_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        lv.scr_load_anim(screen_reviewcard, lv.SCR_LOAD_ANIM.FADE_ON, 100, 100, False)
screen_wordmeun_btn_review.add_event_cb(lambda e: screen_wordmeun_btn_review_event_handler(e), lv.EVENT.ALL, None)

def screen_wordmeun_btn_back_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        lv.scr_load_anim(screen_main, lv.SCR_LOAD_ANIM.FADE_ON, 100, 100, False)
screen_wordmeun_btn_back.add_event_cb(lambda e: screen_wordmeun_btn_back_event_handler(e), lv.EVENT.ALL, None)

def screen_reviewcard_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.SCREEN_LOAD_START):
        pass
        

screen_reviewcard.add_event_cb(lambda e: screen_reviewcard_event_handler(e), lv.EVENT.ALL, None)

def screen_reviewcard_btn_dui_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        

    if (code == lv.EVENT.FOCUSED):
        pass
        

    if (code == lv.EVENT.DEFOCUSED):
        pass
        

screen_reviewcard_btn_dui.add_event_cb(lambda e: screen_reviewcard_btn_dui_event_handler(e), lv.EVENT.ALL, None)

def screen_reviewcard_btn_back_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        lv.scr_load_anim(screen_wordmeun, lv.SCR_LOAD_ANIM.FADE_ON, 100, 100, False)
screen_reviewcard_btn_back.add_event_cb(lambda e: screen_reviewcard_btn_back_event_handler(e), lv.EVENT.ALL, None)

def screen_pomodoro_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.SCREEN_LOAD_START):
        pass
        

screen_pomodoro.add_event_cb(lambda e: screen_pomodoro_event_handler(e), lv.EVENT.ALL, None)

def screen_pomodoro_btn_stopwatch_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        

screen_pomodoro_btn_stopwatch.add_event_cb(lambda e: screen_pomodoro_btn_stopwatch_event_handler(e), lv.EVENT.ALL, None)

def screen_pomodoro_btn_countdown_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        

screen_pomodoro_btn_countdown.add_event_cb(lambda e: screen_pomodoro_btn_countdown_event_handler(e), lv.EVENT.ALL, None)

def screen_pomodoro_btn_back_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        lv.scr_load_anim(screen_main, lv.SCR_LOAD_ANIM.FADE_ON, 100, 100, False)
        

screen_pomodoro_btn_back.add_event_cb(lambda e: screen_pomodoro_btn_back_event_handler(e), lv.EVENT.ALL, None)

def screen_pomodoro_btn_start_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        

screen_pomodoro_btn_start.add_event_cb(lambda e: screen_pomodoro_btn_start_event_handler(e), lv.EVENT.ALL, None)

def screen_pomodoro_btn_reset_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        

screen_pomodoro_btn_reset.add_event_cb(lambda e: screen_pomodoro_btn_reset_event_handler(e), lv.EVENT.ALL, None)

def screen_pomodoro_btn_addnum_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        

screen_pomodoro_btn_addnum.add_event_cb(lambda e: screen_pomodoro_btn_addnum_event_handler(e), lv.EVENT.ALL, None)

def screen_pomodoro_btn_complete_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        

screen_pomodoro_btn_complete.add_event_cb(lambda e: screen_pomodoro_btn_complete_event_handler(e), lv.EVENT.ALL, None)

def screen_pomodoro_btn_continue_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        

screen_pomodoro_btn_continue.add_event_cb(lambda e: screen_pomodoro_btn_continue_event_handler(e), lv.EVENT.ALL, None)

def screen_pomodoro_btn_plus_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        

screen_pomodoro_btn_plus.add_event_cb(lambda e: screen_pomodoro_btn_plus_event_handler(e), lv.EVENT.ALL, None)

def screen_pomodoro_btn_minus_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        

screen_pomodoro_btn_minus.add_event_cb(lambda e: screen_pomodoro_btn_minus_event_handler(e), lv.EVENT.ALL, None)

def screen_setting_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.SCREEN_LOAD_START):
        pass
        

screen_setting.add_event_cb(lambda e: screen_setting_event_handler(e), lv.EVENT.ALL, None)

def screen_setting_btn_back_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        lv.scr_load_anim(screen_main, lv.SCR_LOAD_ANIM.FADE_ON, 200, 200, False)
        

screen_setting_btn_back.add_event_cb(lambda e: screen_setting_btn_back_event_handler(e), lv.EVENT.ALL, None)

def screen_setting_btn_general_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.FOCUSED):
        pass
        #Write animation: screen_setting_btn_general y
        screen_setting_btn_general_anim_y = lv.anim_t()
        screen_setting_btn_general_anim_y.init()
        screen_setting_btn_general_anim_y.set_var(screen_setting_btn_general)
        screen_setting_btn_general_anim_y.set_time(100)
        screen_setting_btn_general_anim_y.set_delay(0)
        screen_setting_btn_general_anim_y.set_custom_exec_cb(lambda e,val: anim_y_cb(screen_setting_btn_general,val))
        screen_setting_btn_general_anim_y.set_values(screen_setting_btn_general.get_y(), 130)
        screen_setting_btn_general_anim_y.set_path_cb(lv.anim_t.path_linear)
        screen_setting_btn_general_anim_y.set_repeat_count(0)
        screen_setting_btn_general_anim_y.set_repeat_delay(0)
        screen_setting_btn_general_anim_y.set_playback_time(100)
        screen_setting_btn_general_anim_y.set_playback_delay(0)
        screen_setting_btn_general_anim_y.start()
        

    if (code == lv.EVENT.DEFOCUSED):
        pass
        screen_setting_btn_general.set_y(135)
        

screen_setting_btn_general.add_event_cb(lambda e: screen_setting_btn_general_event_handler(e), lv.EVENT.ALL, None)

def screen_setting_btn_brightness_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.FOCUSED):
        pass
        

        #Write animation: screen_setting_btn_brightness y
        screen_setting_btn_brightness_anim_y = lv.anim_t()
        screen_setting_btn_brightness_anim_y.init()
        screen_setting_btn_brightness_anim_y.set_var(screen_setting_btn_brightness)
        screen_setting_btn_brightness_anim_y.set_time(100)
        screen_setting_btn_brightness_anim_y.set_delay(0)
        screen_setting_btn_brightness_anim_y.set_custom_exec_cb(lambda e,val: anim_y_cb(screen_setting_btn_brightness,val))
        screen_setting_btn_brightness_anim_y.set_values(screen_setting_btn_brightness.get_y(), 105)
        screen_setting_btn_brightness_anim_y.set_path_cb(lv.anim_t.path_linear)
        screen_setting_btn_brightness_anim_y.set_repeat_count(0)
        screen_setting_btn_brightness_anim_y.set_repeat_delay(0)
        screen_setting_btn_brightness_anim_y.set_playback_time(100)
        screen_setting_btn_brightness_anim_y.set_playback_delay(0)
        screen_setting_btn_brightness_anim_y.start()
    if (code == lv.EVENT.DEFOCUSED):
        pass
        screen_setting_btn_brightness.set_y(110)
    if (code == lv.EVENT.CLICKED):
        pass
        

screen_setting_btn_brightness.add_event_cb(lambda e: screen_setting_btn_brightness_event_handler(e), lv.EVENT.ALL, None)

def screen_setting_btn_volume_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.FOCUSED):
        pass
        

        #Write animation: screen_setting_btn_volume y
        screen_setting_btn_volume_anim_y = lv.anim_t()
        screen_setting_btn_volume_anim_y.init()
        screen_setting_btn_volume_anim_y.set_var(screen_setting_btn_volume)
        screen_setting_btn_volume_anim_y.set_time(100)
        screen_setting_btn_volume_anim_y.set_delay(0)
        screen_setting_btn_volume_anim_y.set_custom_exec_cb(lambda e,val: anim_y_cb(screen_setting_btn_volume,val))
        screen_setting_btn_volume_anim_y.set_values(screen_setting_btn_volume.get_y(), 80)
        screen_setting_btn_volume_anim_y.set_path_cb(lv.anim_t.path_linear)
        screen_setting_btn_volume_anim_y.set_repeat_count(0)
        screen_setting_btn_volume_anim_y.set_repeat_delay(0)
        screen_setting_btn_volume_anim_y.set_playback_time(100)
        screen_setting_btn_volume_anim_y.set_playback_delay(0)
        screen_setting_btn_volume_anim_y.start()
    if (code == lv.EVENT.DEFOCUSED):
        pass
        screen_setting_btn_volume.set_y(85)
    if (code == lv.EVENT.CLICKED):
        pass
        

screen_setting_btn_volume.add_event_cb(lambda e: screen_setting_btn_volume_event_handler(e), lv.EVENT.ALL, None)

def screen_setting_btn_ble_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.FOCUSED):
        pass
        

        #Write animation: screen_setting_btn_ble y
        screen_setting_btn_ble_anim_y = lv.anim_t()
        screen_setting_btn_ble_anim_y.init()
        screen_setting_btn_ble_anim_y.set_var(screen_setting_btn_ble)
        screen_setting_btn_ble_anim_y.set_time(100)
        screen_setting_btn_ble_anim_y.set_delay(0)
        screen_setting_btn_ble_anim_y.set_custom_exec_cb(lambda e,val: anim_y_cb(screen_setting_btn_ble,val))
        screen_setting_btn_ble_anim_y.set_values(screen_setting_btn_ble.get_y(), 55)
        screen_setting_btn_ble_anim_y.set_path_cb(lv.anim_t.path_linear)
        screen_setting_btn_ble_anim_y.set_repeat_count(0)
        screen_setting_btn_ble_anim_y.set_repeat_delay(0)
        screen_setting_btn_ble_anim_y.set_playback_time(100)
        screen_setting_btn_ble_anim_y.set_playback_delay(0)
        screen_setting_btn_ble_anim_y.start()
    if (code == lv.EVENT.DEFOCUSED):
        pass
        screen_setting_btn_ble.set_y(60)
    if (code == lv.EVENT.CLICKED):
        pass
        

screen_setting_btn_ble.add_event_cb(lambda e: screen_setting_btn_ble_event_handler(e), lv.EVENT.ALL, None)

def screen_setting_btn_wifi_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        lv.scr_load_anim(screen_wificfg, lv.SCR_LOAD_ANIM.FADE_ON, 100, 100, False)
    if (code == lv.EVENT.FOCUSED):
        pass
        

        #Write animation: screen_setting_btn_wifi y
        screen_setting_btn_wifi_anim_y = lv.anim_t()
        screen_setting_btn_wifi_anim_y.init()
        screen_setting_btn_wifi_anim_y.set_var(screen_setting_btn_wifi)
        screen_setting_btn_wifi_anim_y.set_time(100)
        screen_setting_btn_wifi_anim_y.set_delay(0)
        screen_setting_btn_wifi_anim_y.set_custom_exec_cb(lambda e,val: anim_y_cb(screen_setting_btn_wifi,val))
        screen_setting_btn_wifi_anim_y.set_values(screen_setting_btn_wifi.get_y(), 30)
        screen_setting_btn_wifi_anim_y.set_path_cb(lv.anim_t.path_linear)
        screen_setting_btn_wifi_anim_y.set_repeat_count(0)
        screen_setting_btn_wifi_anim_y.set_repeat_delay(0)
        screen_setting_btn_wifi_anim_y.set_playback_time(99)
        screen_setting_btn_wifi_anim_y.set_playback_delay(0)
        screen_setting_btn_wifi_anim_y.start()
    if (code == lv.EVENT.DEFOCUSED):
        pass
        screen_setting_btn_wifi.set_y(35)
screen_setting_btn_wifi.add_event_cb(lambda e: screen_setting_btn_wifi_event_handler(e), lv.EVENT.ALL, None)

def screen_wificfg_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.SCREEN_LOAD_START):
        pass
        

screen_wificfg.add_event_cb(lambda e: screen_wificfg_event_handler(e), lv.EVENT.ALL, None)

def screen_wificfg_btn_back_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        lv.scr_load_anim(screen_setting, lv.SCR_LOAD_ANIM.FADE_ON, 100, 100, False)
screen_wificfg_btn_back.add_event_cb(lambda e: screen_wificfg_btn_back_event_handler(e), lv.EVENT.ALL, None)

def screen_wificfg_list_1_item1_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        #Write animation: screen_wificfg_img_statusbg height
        screen_wificfg_img_statusbg_anim_height = lv.anim_t()
        screen_wificfg_img_statusbg_anim_height.init()
        screen_wificfg_img_statusbg_anim_height.set_var(screen_wificfg_img_statusbg)
        screen_wificfg_img_statusbg_anim_height.set_time(200)
        screen_wificfg_img_statusbg_anim_height.set_delay(0)
        screen_wificfg_img_statusbg_anim_height.set_custom_exec_cb(lambda e,val: anim_height_cb(screen_wificfg_img_statusbg,val))
        screen_wificfg_img_statusbg_anim_height.set_values(screen_wificfg_img_statusbg.get_height(), 80)
        screen_wificfg_img_statusbg_anim_height.set_path_cb(lv.anim_t.path_linear)
        screen_wificfg_img_statusbg_anim_height.set_repeat_count(1)
        screen_wificfg_img_statusbg_anim_height.set_repeat_delay(0)
        screen_wificfg_img_statusbg_anim_height.set_playback_time(0)
        screen_wificfg_img_statusbg_anim_height.set_playback_delay(0)
        screen_wificfg_img_statusbg_anim_height.start()
        

screen_wificfg_list_1_item1.add_event_cb(lambda e: screen_wificfg_list_1_item1_event_handler(e), lv.EVENT.ALL, None)

def screen_wificfg_list_1_item2_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        

        #Write animation: screen_wificfg_img_statusbg height
        screen_wificfg_img_statusbg_anim_height = lv.anim_t()
        screen_wificfg_img_statusbg_anim_height.init()
        screen_wificfg_img_statusbg_anim_height.set_var(screen_wificfg_img_statusbg)
        screen_wificfg_img_statusbg_anim_height.set_time(200)
        screen_wificfg_img_statusbg_anim_height.set_delay(0)
        screen_wificfg_img_statusbg_anim_height.set_custom_exec_cb(lambda e,val: anim_height_cb(screen_wificfg_img_statusbg,val))
        screen_wificfg_img_statusbg_anim_height.set_values(screen_wificfg_img_statusbg.get_height(), 80)
        screen_wificfg_img_statusbg_anim_height.set_path_cb(lv.anim_t.path_linear)
        screen_wificfg_img_statusbg_anim_height.set_repeat_count(1)
        screen_wificfg_img_statusbg_anim_height.set_repeat_delay(0)
        screen_wificfg_img_statusbg_anim_height.set_playback_time(0)
        screen_wificfg_img_statusbg_anim_height.set_playback_delay(0)
        screen_wificfg_img_statusbg_anim_height.start()
screen_wificfg_list_1_item2.add_event_cb(lambda e: screen_wificfg_list_1_item2_event_handler(e), lv.EVENT.ALL, None)

def screen_wificfg_list_1_item3_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        

        #Write animation: screen_wificfg_img_statusbg height
        screen_wificfg_img_statusbg_anim_height = lv.anim_t()
        screen_wificfg_img_statusbg_anim_height.init()
        screen_wificfg_img_statusbg_anim_height.set_var(screen_wificfg_img_statusbg)
        screen_wificfg_img_statusbg_anim_height.set_time(200)
        screen_wificfg_img_statusbg_anim_height.set_delay(0)
        screen_wificfg_img_statusbg_anim_height.set_custom_exec_cb(lambda e,val: anim_height_cb(screen_wificfg_img_statusbg,val))
        screen_wificfg_img_statusbg_anim_height.set_values(screen_wificfg_img_statusbg.get_height(), 80)
        screen_wificfg_img_statusbg_anim_height.set_path_cb(lv.anim_t.path_linear)
        screen_wificfg_img_statusbg_anim_height.set_repeat_count(1)
        screen_wificfg_img_statusbg_anim_height.set_repeat_delay(0)
        screen_wificfg_img_statusbg_anim_height.set_playback_time(0)
        screen_wificfg_img_statusbg_anim_height.set_playback_delay(-1)
        screen_wificfg_img_statusbg_anim_height.start()
screen_wificfg_list_1_item3.add_event_cb(lambda e: screen_wificfg_list_1_item3_event_handler(e), lv.EVENT.ALL, None)

def screen_wificfg_list_1_item4_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        

        #Write animation: screen_wificfg_img_statusbg height
        screen_wificfg_img_statusbg_anim_height = lv.anim_t()
        screen_wificfg_img_statusbg_anim_height.init()
        screen_wificfg_img_statusbg_anim_height.set_var(screen_wificfg_img_statusbg)
        screen_wificfg_img_statusbg_anim_height.set_time(200)
        screen_wificfg_img_statusbg_anim_height.set_delay(0)
        screen_wificfg_img_statusbg_anim_height.set_custom_exec_cb(lambda e,val: anim_height_cb(screen_wificfg_img_statusbg,val))
        screen_wificfg_img_statusbg_anim_height.set_values(screen_wificfg_img_statusbg.get_height(), 80)
        screen_wificfg_img_statusbg_anim_height.set_path_cb(lv.anim_t.path_linear)
        screen_wificfg_img_statusbg_anim_height.set_repeat_count(1)
        screen_wificfg_img_statusbg_anim_height.set_repeat_delay(0)
        screen_wificfg_img_statusbg_anim_height.set_playback_time(0)
        screen_wificfg_img_statusbg_anim_height.set_playback_delay(0)
        screen_wificfg_img_statusbg_anim_height.start()
screen_wificfg_list_1_item4.add_event_cb(lambda e: screen_wificfg_list_1_item4_event_handler(e), lv.EVENT.ALL, None)

def screen_wificfg_list_1_item5_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        

        #Write animation: screen_wificfg_img_statusbg height
        screen_wificfg_img_statusbg_anim_height = lv.anim_t()
        screen_wificfg_img_statusbg_anim_height.init()
        screen_wificfg_img_statusbg_anim_height.set_var(screen_wificfg_img_statusbg)
        screen_wificfg_img_statusbg_anim_height.set_time(200)
        screen_wificfg_img_statusbg_anim_height.set_delay(0)
        screen_wificfg_img_statusbg_anim_height.set_custom_exec_cb(lambda e,val: anim_height_cb(screen_wificfg_img_statusbg,val))
        screen_wificfg_img_statusbg_anim_height.set_values(screen_wificfg_img_statusbg.get_height(), 80)
        screen_wificfg_img_statusbg_anim_height.set_path_cb(lv.anim_t.path_linear)
        screen_wificfg_img_statusbg_anim_height.set_repeat_count(1)
        screen_wificfg_img_statusbg_anim_height.set_repeat_delay(0)
        screen_wificfg_img_statusbg_anim_height.set_playback_time(0)
        screen_wificfg_img_statusbg_anim_height.set_playback_delay(0)
        screen_wificfg_img_statusbg_anim_height.start()
screen_wificfg_list_1_item5.add_event_cb(lambda e: screen_wificfg_list_1_item5_event_handler(e), lv.EVENT.ALL, None)

def screen_wificfg_btn_scan_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        

screen_wificfg_btn_scan.add_event_cb(lambda e: screen_wificfg_btn_scan_event_handler(e), lv.EVENT.ALL, None)

def screen_wificfg_btn_disconnect_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        screen_wificfg_img_wififalse.clear_flag(lv.obj.FLAG.HIDDEN)
        
        screen_wificfg_label_status2.set_text("Disconnected")
        screen_wificfg_label_status2.set_style_text_color(lv.color_hex(0x274a6e), 0)
        

screen_wificfg_btn_disconnect.add_event_cb(lambda e: screen_wificfg_btn_disconnect_event_handler(e), lv.EVENT.ALL, None)

def screen_wificfg_btn_cancel_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        

screen_wificfg_btn_cancel.add_event_cb(lambda e: screen_wificfg_btn_cancel_event_handler(e), lv.EVENT.ALL, None)

def screen_wificfg_btn_connect_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        

screen_wificfg_btn_connect.add_event_cb(lambda e: screen_wificfg_btn_connect_event_handler(e), lv.EVENT.ALL, None)

def screen_wificfg_ta_password_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        

    if (code == lv.EVENT.FOCUSED):
        pass
        

screen_wificfg_ta_password.add_event_cb(lambda e: screen_wificfg_ta_password_event_handler(e), lv.EVENT.ALL, None)

def screen_donelist_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.SCREEN_LOAD_START):
        pass
        

screen_donelist.add_event_cb(lambda e: screen_donelist_event_handler(e), lv.EVENT.ALL, None)

def screen_donelist_btn_save_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        

screen_donelist_btn_save.add_event_cb(lambda e: screen_donelist_btn_save_event_handler(e), lv.EVENT.ALL, None)

def screen_donelist_btn_nxtpage_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        

screen_donelist_btn_nxtpage.add_event_cb(lambda e: screen_donelist_btn_nxtpage_event_handler(e), lv.EVENT.ALL, None)

def screen_donelist_btn_lastpage_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        

screen_donelist_btn_lastpage.add_event_cb(lambda e: screen_donelist_btn_lastpage_event_handler(e), lv.EVENT.ALL, None)

def screen_donelist_btn_back_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        lv.scr_load_anim(screen_main, lv.SCR_LOAD_ANIM.FADE_ON, 100, 100, False)
screen_donelist_btn_back.add_event_cb(lambda e: screen_donelist_btn_back_event_handler(e), lv.EVENT.ALL, None)

def screen_answer_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.SCREEN_LOAD_START):
        pass
        

screen_answer.add_event_cb(lambda e: screen_answer_event_handler(e), lv.EVENT.ALL, None)

def screen_answer_btn_start_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        lv.scr_load_anim(screen_loading, lv.SCR_LOAD_ANIM.FADE_ON, 100, 100, False)
        

screen_answer_btn_start.add_event_cb(lambda e: screen_answer_btn_start_event_handler(e), lv.EVENT.ALL, None)

def screen_loading_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.SCREEN_LOAD_START):
        pass
        screen_loading_img_icon.set_pivot(29, 29)
        #Write animation: screen_loading_img_icon rotate
        screen_loading_img_icon_anim_rotate = lv.anim_t()
        screen_loading_img_icon_anim_rotate.init()
        screen_loading_img_icon_anim_rotate.set_var(screen_loading_img_icon)
        screen_loading_img_icon_anim_rotate.set_time(3000)
        screen_loading_img_icon_anim_rotate.set_delay(0)
        screen_loading_img_icon_anim_rotate.set_custom_exec_cb(lambda e,val: anim_img_rotate_cb(screen_loading_img_icon,val))
        screen_loading_img_icon_anim_rotate.set_values(0, 360*10)
        screen_loading_img_icon_anim_rotate.set_path_cb(lv.anim_t.path_linear)
        screen_loading_img_icon_anim_rotate.set_repeat_count(1)
        screen_loading_img_icon_anim_rotate.set_repeat_delay(0)
        screen_loading_img_icon_anim_rotate.set_playback_time(0)
        screen_loading_img_icon_anim_rotate.set_playback_delay(0)
        screen_loading_img_icon_anim_rotate.set_ready_cb(loading_anim_done_cb)
        screen_loading_img_icon_anim_rotate.start()
        

screen_loading.add_event_cb(lambda e: screen_loading_event_handler(e), lv.EVENT.ALL, None)

def screen_getanswer_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.SCREEN_LOAD_START):
        pass
        

screen_getanswer.add_event_cb(lambda e: screen_getanswer_event_handler(e), lv.EVENT.ALL, None)

def screen_getanswer_btn_answer_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        lv.scr_load_anim(screen_main, lv.SCR_LOAD_ANIM.FADE_ON, 100, 100, False)
screen_getanswer_btn_answer.add_event_cb(lambda e: screen_getanswer_btn_answer_event_handler(e), lv.EVENT.ALL, None)

def screen_picture_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.SCREEN_LOAD_START):
        pass
        

screen_picture.add_event_cb(lambda e: screen_picture_event_handler(e), lv.EVENT.ALL, None)

def screen_picture_img_picture_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        lv.scr_load_anim(screen_main, lv.SCR_LOAD_ANIM.FADE_ON, 100, 100, False)
screen_picture_img_picture.add_event_cb(lambda e: screen_picture_img_picture_event_handler(e), lv.EVENT.ALL, None)

# content from custom.py

# Load the default screen
lv.scr_load(screen_reppo)

while SDL.check():
    time.sleep_ms(5)

