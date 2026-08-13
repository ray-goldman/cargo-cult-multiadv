#pragma once

constexpr int OLED_SDA = 5;
constexpr int OLED_SCL = 6;
constexpr int LED_GREEN = 3;
constexpr int LED_RED = 4;
constexpr unsigned long SERIAL_BAUD = 115200;
#ifdef ROLE_INTERVAL_OVERRIDE_MS
constexpr unsigned long ROLE_INTERVAL_MS = ROLE_INTERVAL_OVERRIDE_MS;
#else
constexpr unsigned long ROLE_INTERVAL_MS = 60000;
#endif
