#include <pebble.h>
#pragma once

typedef enum {
  KEY_SECONDS = 0,
  KEY_BLUETOOTH,
  KEY_COLOR_FACE,
  KEY_COLOR_NUM,
  KEY_COLOR_DAY,
  KEY_COLOR_MON,
  KEY_COLOR_TIC,
  KEY_COLOR_SEC,
  KEY_COLOR_MIN,
  KEY_COLOR_HR,
  KEY_LAST
} MessageKey;

static const struct GPathInfo TIC_POINTS = {
  4, (GPoint []){
    {-3, -100},
    {3, -100},
    {3, -80},
    {-3, -80}
  }
};

static const GPathInfo SECOND_HAND_POINTS = {
  4,
  (GPoint []) {
    {0, -60},
    {4, -65},
    {0, -70},
    {-4, -65}
  }
};

static const GPathInfo MINUTE_HAND_POINTS = {
  5,
  (GPoint []) {
    { 4, -15 },
    { 6, -55 },
    { 0, -70 },
    { -6, -55 },
    { -4, -15 }
  }
};

static const GPathInfo HOUR_HAND_POINTS = {
  5,
  (GPoint []) {
    { 4, -15 },
    { 6, -40 },
    { 0, -50 },
    { -6, -40 },
    { -4, -15 }
  }
};
