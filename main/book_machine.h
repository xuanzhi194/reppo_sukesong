#pragma once

/**
 * Ebook lives in /spiffs/book.txt (see data/book.txt for the format).
 * App sends the same UTF-8 file over BLE 0x50/0x51/0x52; device writes
 * SPIFFS then reloads. Limits: 1 MB file, 64 parts, 1024 pages, ~400 chars/page.
 */
void book_machine_init(void);

/** Re-read /spiffs/book.txt after an App upload. */
void book_machine_reload(void);

/** After setup_scr_screen_book, bind widgets if Guider cleaned the main page first. */
void book_machine_poll(void);
