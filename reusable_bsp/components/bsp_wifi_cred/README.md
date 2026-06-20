# bsp_wifi_cred

Reusable NVS-backed Wi-Fi credential cache.

This module only stores SSID/password pairs.  It does not scan, connect, or own
the ESP-IDF Wi-Fi driver.

## Basic Usage

```c
nvs_flash_init();
bsp_wifi_cred_init(NULL, NULL);

bsp_wifi_cred_save("MyAP", "password123");

char pw[65];
if (bsp_wifi_cred_get_password("MyAP", pw, sizeof(pw))) {
    // use pw with esp_wifi_set_config()
}
```

## API

- `bsp_wifi_cred_init(namespace, key)`: load store from NVS.  Pass `NULL` for
  defaults.
- `bsp_wifi_cred_save(ssid, password)`: add or update a credential.  If full,
  the oldest entry is dropped.
- `bsp_wifi_cred_get_password(...)`: lookup by SSID.
- `bsp_wifi_cred_erase(ssid)`: remove one saved AP.
- `bsp_wifi_cred_clear()`: erase all saved APs.
- `bsp_wifi_cred_get_entry(index, entry)`: enumerate saved credentials.

## Porting Notes

Call `nvs_flash_init()` before this module.  Override
`BSP_WIFI_CRED_MAX_ENTRIES`, `BSP_WIFI_CRED_SSID_LEN`, or
`BSP_WIFI_CRED_PASS_LEN` from compile definitions if your product needs
different limits.

