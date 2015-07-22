#include <pebble.h>
#pragma once

#define NUM_CLOCK_TICKS 8

static const struct GPathInfo ANALOG_BG_POINTS[] = {
  { 4, (GPoint []){
      {112, 10},
      {114, 12},
      {108, 23},
      {106, 21}
    }
  },
  { 4, (GPoint []){
      {132, 47},
      {144, 40},
      {144, 44},
      {135, 49}
    }
  },
  { 4, (GPoint []){
      {135, 118},
      {144, 123},
      {144, 126},
      {132, 120}
    }
  },
  { 4, (GPoint []){
      {108, 144},
      {114, 154},
      {112, 157},
      {106, 147}
    }
  },
  { 4, (GPoint []){
      {32, 10},
      {30, 12},
      {36, 23},
      {38, 21}
    }
  },
  { 4, (GPoint []){
      {12, 47},
      {-1, 40},
      {-1, 44},
      {9, 49}
    }
  },
  { 4, (GPoint []){
      {9, 118},
      {-1, 123},
      {-1, 126},
      {12, 120}
    }
  },
  { 4, (GPoint []){
      {36, 144},
      {30, 154},
      {32, 157},
      {38, 147}
    }
  },

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
