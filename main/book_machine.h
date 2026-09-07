#pragma once

/**
 * Ebook lives in /spiffs/book.txt (see data/book.txt for the format).
 * App import can overwrite that file, then call book_machine_reload().
 */
void book_machine_init(void);

/** Re-read /spiffs/book.txt after an App upload. */
void book_machine_reload(void);

/** After setup_scr_screen_book, bind widgets if Guider cleaned the main page first. */
void book_machine_poll(void);
