/*
Count Logger
Allows user to log the time that the SELECT button is pressed

Should output [time, count]
where time ==> formatted time output 
count ==> # times SELECT is pressed

Once the log is finished logging, the data shall be pushed to the companion iOS app.
Each log will be represented in a list in the companion app;
Tapping each list item will go to a DETAILS view where detailed log data will be listed

Data shall persist on the iOS companion app.
*/

#include <pebble.h>

static Window *window;
static TextLayer *text_layer;
static uint32_t s_packet_id;

//counter variable for # times SELECT is pressed
int select_count = 0;
//datalogging
#define TIMESTAMP_LOG 1

//method for handling "select" button press
static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
  select_count++;
  text_layer_set_text(text_layer, "Counting...");
  APP_LOG(APP_LOG_LEVEL_DEBUG, "%d counts", select_count); //Pebble log output
}

static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
}

//creates window upon watchApp load
static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  text_layer = text_layer_create((GRect) { .origin = { 0, 72 }, .size = { bounds.size.w, 20 } });
  text_layer_set_text(text_layer, "Press a button");
  text_layer_set_text_alignment(text_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(text_layer));
}

static void window_unload(Window *window) {
  text_layer_destroy(text_layer);
}

static void init(void) {
  window = window_create();
  window_set_click_config_provider(window, click_config_provider);
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });

  const bool animated = true;
  window_stack_push(window, animated);
}

static void deinit(void) {
  window_destroy(window);
}

int main(void) {
  init();
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Done initializing, pushed window: %p", window);
  app_event_loop();
  deinit();
}

