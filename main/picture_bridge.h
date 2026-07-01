#ifndef PICTURE_BRIDGE_H
#define PICTURE_BRIDGE_H

/** 链入 picture_bridge 强符号（与 wificfg_bridge_init 同理） */
void picture_bridge_init(void);

/** 进入 screen_picture 时加载蓝牙下发的 JPEG 或占位图（仅 ESP-IDF 固件） */
void picture_bridge_screen_on_load(void);

#endif /* PICTURE_BRIDGE_H */
