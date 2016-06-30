#include <pebble.h>
#include "counter.h"

#define DEFAULT_TAG 42
#define DEFAULT_DELAY_INTERVAL 500

static void prv_delay_timer_callback(void *data);

typedef struct IsPressedCounter {
  Data *data;
  DataLoggingSessionRef logging_session;
  uint32_t tag;
  AppTimer *delay_timer;
  uint32_t delay_interval;
  IsPressedCounterUpdateHandler update_handler; //unsure what this does
} IsPressedCounter;

IsPressedCounter* isPressedCounter_create(void) {
  //allocate memory for counter object isPressedCounter
  IsPressedCounter *isPressedCounter = malloc(sizeof(IsPressedCounter));
  if (!isPressedCounter) {
    APP_LOG(APP_LOG_LEVEL_ERROR, "Failed to allocate memory");
    return NULL;
  }

  *isPressedCounter = (IsPressedCounter) {
    .tag = DEFAULT_TAG,
    .delay_interval = DEFAULT_DELAY_INTERVAL,
  };

  //allocate memory for counter object isPressedCounter's data
  isPressedCounter->data = malloc(sizeof(Data));
  if (!isPressedCounter->data) {
    APP_LOG(APP_LOG_LEVEL_ERROR, "Failed to allocate memory for counter data");
    free(isPressedCounter);
    return NULL;
  }
  *isPressedCounter->data = (Data) {0};
  return isPressedCounter;
}

void isPressedCounter_destroy(IsPressedCounter *isPressedCounter) {
  if (!isPressedCounter) return;

  app_timer_cancel(isPressedCounter->delay_timer);
  if (isPressedCounter->logging_session) {
    data_logging_finish(isPressedCounter->logging_session);
  }
  free(isPressedCounter->data);
  free(isPressedCounter);
}

void isPressedCounter_start_logging(IsPressedCounter *isPressedCounter) {
  if (!isPressedCounter) return;
  if(isPressedCounter->logging_session) return;

  isPressedCounter_reset_data(isPressedCounter);

  isPressedCounter->logging_session = data_logging_create(isPressedCounter->tag, 
                                                          DATA_LOGGING_BYTE_ARRAY,
                                                          sizeof(Data), true);
  if (!isPressedCounter->logging_session) {
    APP_LOG(APP_LOG_LEVEL_ERROR, "Failed to create datalogging session");
  }
  isPressedCounter->delay_timer = app_timer_register(0, prv_delay_timer_callback, isPressedCounter);
  //end
}

void isPressedCounter_stop_logging(IsPressedCounter *isPressedCounter) {
  if (!isPressedCounter) return;
  if (!isPressedCounter->logging_session) return;

  app_timer_cancel(isPressedCounter->delay_timer);
  data_logging_finish(isPressedCounter->logging_session);
  isPressedCounter->logging_session = NULL;
}

bool isPressedCounter_is_logging(IsPressedCounter *isPressedCounter) {
  if (!isPressedCounter) return false;

  return isPressedCounter->logging_session != NULL;
}

void isPressedCounter_reset_data(IsPressedCounter *isPressedCounter) {
  if (!isPressedCounter) return;
  *isPressedCounter->data = (Data) {0};

  if(isPressedCounter->update_handler != NULL) {
    isPressedCounter->update_handler(isPressedCounter->data);
  }
}

void isPressedCounter_set_update_handler(IsPressedCounter *isPressedCounter, IsPressedCounterUpdateHandler handler) {
  if (!isPressedCounter) return;
  isPressedCounter->update_handler = handler;
}

static void prv_update_data(IsPressedCounter *isPressedCounter) {
  if(!isPressedCounter) {
    APP_LOG(APP_LOG_LEVEL_ERROR, "Failed during prv_update_data");
    return;
  }

  isPressedCounter->data->isPressedCount++;
  time_ms(&isPressedCounter->data->timestamp, &isPressedCounter->data->timestamp_ms);

  if(isPressedCounter->update_handler != NULL) {
    isPressedCounter->update_handler(isPressedCounter->data);
  }
}

static void prv_log_data(IsPressedCounter *isPressedCounter) {
  if (!isPressedCounter) return;

  printf("========================================");
  printf("counter: \t\t%d", (int) isPressedCounter->data->isPressedCount);
  printf("time: \t\t%d.%d", (int) isPressedCounter->data->timestamp, 
                            (int) isPressedCounter->data->timestamp_ms);
  printf("========================================");
}

static void prv_add_data(IsPressedCounter *isPressedCounter) {
  if (!isPressedCounter) return;

  DataLoggingResult result = data_logging_log(isPressedCounter->logging_session, isPressedCounter->data, 1);
  if(result != DATA_LOGGING_SUCCESS) {
    APP_LOG(APP_LOG_LEVEL_ERROR, "Failed to add data to logging session: %d", (int)result);
  }
}

static void prv_delay_timer_callback(void *data) {
  if (!data) return;

  IsPressedCounter *isPressedCounter = data;

  prv_update_data(isPressedCounter);
  prv_log_data(isPressedCounter);
  prv_add_data(isPressedCounter);
  isPressedCounter->delay_timer = app_timer_register(isPressedCounter->delay_interval, prv_delay_timer_callback, data);
}