#pragma once

// TricorderData is the struct we send to mobile apps
// Any changes to this struct would require changes to mobile apps as well
typedef struct __attribute__((__packed__)) {
  uint32_t isPressedCount;      // 4 bytes
  time_t timestamp;        // 4 bytes
  uint16_t timestamp_ms;   // 2 bytes
} Data;           // 10 bytes

typedef struct IsPressedCounter IsPressedCounter;
typedef void (*IsPressedCounterUpdateHandler)(Data *data); 

IsPressedCounter* isPressedCounter_create(void);
void isPressedCounter_destroy(IsPressedCounter *isPressedCounter);

void isPressedCounter_start_logging(IsPressedCounter *isPressedCounter);
void isPressedCounter_stop_logging(IsPressedCounter *isPressedCounter);
bool isPressedCounter_is_logging(IsPressedCounter *isPressedCounter);

void isPressedCounter_reset_data(IsPressedCounter *isPressedCounter);
void isPressedCounter_set_update_handler(IsPressedCounter *isPressedCounter, IsPressedCounterUpdateHandler handler);