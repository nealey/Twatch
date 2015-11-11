#include <pebble.h>
#include "twatch.h"

static Window *window;
static Layer *s_simple_bg_layer, *s_date_layer, *s_hands_layer;
static TextLayer *s_bt_label, *s_day_label, *s_mon_label;



static GPath *s_tic_path;
static GPath *s_second_arrow, *s_minute_arrow, *s_hour_arrow;
static char s_mon_buffer[4], s_day_buffer[6];
static TextLayer *s_hour_label[4];
static char s_hour[4][4];

GColor fg;
GColor bg;
bool seconds = true;

bool bt_connected = true;

static void bg_update_proc(Layer *layer, GContext *ctx) {
  // Fill background
  graphics_context_set_fill_color(ctx, bg);
  graphics_fill_rect(ctx, layer_get_bounds(layer), 0, GCornerNone);
  
  // Draw tics
  graphics_context_set_fill_color(ctx, fg);
  for (int i = 0; i < 12; i += 1) {
    switch (i) {
      case 0:
      case 3:
      case 6:
      case 9:
        break;
      default:
        gpath_rotate_to(s_tic_path, TRIG_MAX_ANGLE * i / 12);
        gpath_draw_filled(ctx, s_tic_path);
        break;
    }
  }

  // Draw numbers
  for (int i = 0; i < 4; i += 1) {
    int hour = 3 * (i+1);

    if (1 && clock_is_24h_style()) {
      time_t epoch;
      struct tm *now;

      time(&epoch);
      now = localtime(&epoch);

      if ((now->tm_hour > 12) || (now->tm_hour == 0)) {
	      hour += 12;
      }
    }

    snprintf(s_hour[i], 3, "%d", hour);
    text_layer_set_text(s_hour_label[i], s_hour[i]);
  }
}

static void hands_update_proc(Layer *layer, GContext *ctx) {
  GRect bounds = layer_get_bounds(layer);

  time_t now = time(NULL);
  struct tm *t = localtime(&now);
  
  graphics_context_set_fill_color(ctx, fg);
  graphics_context_set_stroke_color(ctx, bg);

  // minute/hour hand
  gpath_rotate_to(s_minute_arrow, TRIG_MAX_ANGLE * t->tm_min / 60);
  gpath_draw_filled(ctx, s_minute_arrow);
  gpath_draw_outline(ctx, s_minute_arrow);

  gpath_rotate_to(s_hour_arrow, (TRIG_MAX_ANGLE * (((t->tm_hour % 12) * 6) + (t->tm_min / 10))) / (12 * 6));
  gpath_draw_filled(ctx, s_hour_arrow);
  gpath_draw_outline(ctx, s_hour_arrow);

  if (seconds) {
    // second hand
#ifdef PBL_COLOR
    graphics_context_set_fill_color(ctx, GColorJaegerGreen);
#endif
    gpath_rotate_to(s_second_arrow, TRIG_MAX_ANGLE * t->tm_sec / 60);
    gpath_draw_filled(ctx, s_second_arrow);
    //gpath_draw_outline(ctx, s_second_arrow);
  }

  // dot in the middle
  graphics_context_set_fill_color(ctx, fg);
  graphics_fill_circle(ctx, GPoint(bounds.size.w / 2, bounds.size.h / 2), 2);
}

static void date_update_proc(Layer *layer, GContext *ctx) {
  time_t now = time(NULL);
  struct tm *t = localtime(&now);
  char *b = s_day_buffer;

  if (bt_connected) {
    text_layer_set_text(s_bt_label, "");
  } else {
    text_layer_set_text(s_bt_label, "");
  }
  
  strftime(s_mon_buffer, sizeof(s_mon_buffer), "%b", t);
  text_layer_set_text(s_mon_label, s_mon_buffer);

  strftime(s_day_buffer, sizeof(s_day_buffer), "%d", t);
  if (b[0] == '0') {
    b += 1;
  }
  text_layer_set_text(s_day_label, b);
}

static void handle_second_tick(struct tm *tick_time, TimeUnits units_changed) {
  layer_mark_dirty(window_get_root_layer(window));
}

#define NUM_HEIGHT 50
#define NUM_WIDTH 54
#define BASELINE_OFFSET -7
#define NUM_FONT RESOURCE_ID_FONT_48

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);
  
  int xmax = bounds.size.w;
  int ymax = bounds.size.h;

  s_simple_bg_layer = layer_create(bounds);
  layer_set_update_proc(s_simple_bg_layer, bg_update_proc);
  layer_add_child(window_layer, s_simple_bg_layer);

  for (int i = 0; i < 4; i += 1) {
    int x, y;
    GTextAlignment align;

    switch (i) {
    case 0:
      x = (xmax - NUM_WIDTH) - 2; y = (ymax - NUM_HEIGHT) / 2 + BASELINE_OFFSET;
      align = GTextAlignmentRight;
      break;
    case 1:
      x = (xmax - NUM_WIDTH) / 2; y = ymax + BASELINE_OFFSET - NUM_HEIGHT;
      align = GTextAlignmentCenter;
      break;
    case 2:
      x = 2; y = (ymax - NUM_HEIGHT) / 2 + BASELINE_OFFSET;
      align = GTextAlignmentLeft;
      break;
    case 3:
      x = (xmax - NUM_WIDTH) / 2; y = BASELINE_OFFSET;
      align = GTextAlignmentCenter;
      break;
    }

    s_hour_label[i] = text_layer_create(GRect(x, y, NUM_WIDTH, NUM_HEIGHT));
    text_layer_set_text_alignment(s_hour_label[i], align);
    text_layer_set_background_color(s_hour_label[i], bg);
    text_layer_set_text_color(s_hour_label[i], fg);
    text_layer_set_font(s_hour_label[i], fonts_load_custom_font(resource_get_handle(NUM_FONT)));

    layer_add_child(s_simple_bg_layer, text_layer_get_layer(s_hour_label[i]));
  }

  s_date_layer = layer_create(bounds);
  layer_set_update_proc(s_date_layer, date_update_proc);
  layer_add_child(window_layer, s_date_layer);

  // Missing phone
#ifdef PBL_RECT
  s_bt_label = text_layer_create(GRect(20, 95, 52, 52));
#else
  s_bt_label = text_layer_create(GRect(xmax/2 - 44, ymax/2 + 5, 52, 52));
#endif
  text_layer_set_text_alignment(s_bt_label, GTextAlignmentLeft);
  text_layer_set_text(s_bt_label, "");
  text_layer_set_background_color(s_bt_label, GColorClear);
  text_layer_set_text_color(s_bt_label, fg);
  text_layer_set_font(s_bt_label, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_SYMBOLS_52)));
  layer_add_child(s_date_layer, text_layer_get_layer(s_bt_label));

  // Month
#ifdef PBL_RECT
  s_mon_label = text_layer_create(GRect(116, 145, 27, 18));
  text_layer_set_font(s_mon_label, fonts_get_system_font(FONT_KEY_GOTHIC_18));
#else
  s_mon_label = text_layer_create(GRect(124, 140, 27, 14));
  text_layer_set_font(s_mon_label, fonts_get_system_font(FONT_KEY_GOTHIC_14));
#endif
  text_layer_set_text_alignment(s_mon_label, GTextAlignmentRight);
  text_layer_set_text(s_mon_label, s_mon_buffer);
  text_layer_set_background_color(s_mon_label, GColorClear);
  text_layer_set_text_color(s_mon_label, fg);
  layer_add_child(s_date_layer, text_layer_get_layer(s_mon_label));

  // Day
#ifdef PBL_RECT
  s_day_label = text_layer_create(GRect(121, 122, 20, 24));
  text_layer_set_font(s_day_label, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
#else
  s_day_label = text_layer_create(GRect(135, 124, 20, 18));
  text_layer_set_font(s_day_label, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
#endif
  text_layer_set_text_alignment(s_day_label, GTextAlignmentRight);
  text_layer_set_text(s_day_label, s_day_buffer);
  text_layer_set_background_color(s_day_label, GColorClear);
  text_layer_set_text_color(s_day_label, fg);
  layer_add_child(s_date_layer, text_layer_get_layer(s_day_label));

  s_hands_layer = layer_create(bounds);
  layer_set_update_proc(s_hands_layer, hands_update_proc);
  layer_add_child(window_layer, s_hands_layer);
}

static void window_unload(Window *window) {
  layer_destroy(s_simple_bg_layer);
  layer_destroy(s_date_layer);

  text_layer_destroy(s_day_label);
  text_layer_destroy(s_mon_label);

  layer_destroy(s_hands_layer);
}

static void bt_handler(bool connected) {
  bt_connected = connected;
  if (! connected) {
    vibes_long_pulse();
  }
  layer_mark_dirty(s_date_layer);
}

static void init() {
  fg = GColorBlack;
  bg = GColorWhite;
  window = window_create();
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  window_stack_push(window, true);

  s_day_buffer[0] = '\0';
  s_mon_buffer[0] = '\0';

  // init paths
  s_tic_path = gpath_create(&TIC_POINTS);
  s_second_arrow = gpath_create(&SECOND_HAND_POINTS);
  s_minute_arrow = gpath_create(&MINUTE_HAND_POINTS);
  s_hour_arrow = gpath_create(&HOUR_HAND_POINTS);

  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);
  GPoint center = grect_center_point(&bounds);
  gpath_move_to(s_tic_path, center);
  gpath_move_to(s_second_arrow, center);
  gpath_move_to(s_minute_arrow, center);
  gpath_move_to(s_hour_arrow, center);

  bluetooth_connection_service_subscribe(bt_handler);
  bt_connected = bluetooth_connection_service_peek();

  if (seconds) {
    tick_timer_service_subscribe(SECOND_UNIT, handle_second_tick);
  } else {
    tick_timer_service_subscribe(MINUTE_UNIT, handle_second_tick);
  }
}

static void deinit() {
  gpath_destroy(s_tic_path);
  gpath_destroy(s_second_arrow);
  gpath_destroy(s_minute_arrow);
  gpath_destroy(s_hour_arrow);

  tick_timer_service_unsubscribe();
  window_destroy(window);
}

int main() {
  init();
  app_event_loop();
  deinit();
}
