#ifndef Config_h
#define Config_h


// ----- Debug -----
#define DEBUG false // false
#define DEBUG_MENU false // false
#define SERIAL_ENCODER false // false
#define SERIAL_LCD false // false
#define NO_RTC false // false


// ----- Pins -----
#define PIN_THERM A0 // A0
#define PIN_PH A1 // A1
#define PIN_EC A2 // A2
#define PIN_PUMP 4 // 4
#define PIN_SOL_1 5 // 5
#define PIN_SOL_2 6 // 6
#define PIN_SOL_3 7 // 7
#define PIN_LED_1 8 // 8
#define PIN_LED_2 11 // 11
#define PIN_LED_3 12 // 12
#define PIN_ENC_DATA 2 // 2
#define PIN_ENC_CLOCK 3 // 3
#define PIN_ENC_SWITCH 9 // 9
#define PIN_SPEAKER 10 // 10


// ----- LCD Display -----
#define LCD_COLS 20 // 20
#define LCD_ROWS 4 // 4


// ----- LCDLine -----
#define MAX_LINE_VARIABLES 4 // 4
#define MAX_LINE_STRING 20 // 20
#define MAX_CHOICE_CHOICES 5 // 5


// ----- LCDScreen -----
#define MAX_SCREEN_LINES 50 // 50
#define CURSOR_HIDDEN 0 // 0
#define CURSOR_LEFT 1 // 1
#define CURSOR_RIGHT 2 // 2


// ----- System -----
#define NUMBER_OF_LEVELS 3 // 3
#define MAX_QUEUE_SIZE 12 // 12
#define SOL_SWITCH_SECONDS 1 // 1
// CLOCK
#define CLOCK_MODE_12 0
#define CLOCK_MODE_24 1
#define DEFAULT_CLOCK_MODE CLOCK_MODE_12
#define SECONDS_IN_ONE_DAY 86400 // 86400 (24hr)
#define ENCODER_HOLD_MS 1000 // 1000 (1s)
#define ENCODER_DEBOUNCE_MS 50 // 50 (0.05s)
#define TICK_MS 300 // 300 (0.3s)
#define MAX_RTC_DISCONNECT 10000 // 10000 (10s)
// TIMEOUT
#define SCREENSAVER_TIMEOUT 0 // 60 (1min)
#define LED_OVERRIDE_TIMEOUT 300 // 300 (5min)
#define SENSOR_REFRESH_RATE 5 // 5 (every 1 second)
// LEVEL SPECIFIC FD TIMINGS
#define FD_CYCLE_FLOOD_SECONDS_1 60 // 60 (60s)
#define FD_CYCLE_FLOOD_SECONDS_2 80 // 80 (80s)
#define FD_CYCLE_FLOOD_SECONDS_3 90 // 90 (90s)
#define FD_CYCLE_DRAIN_SECONDS_1 480 // 600 (10m)
#define FD_CYCLE_DRAIN_SECONDS_2 480 // 600 (10m)
#define FD_CYCLE_DRAIN_SECONDS_3 480 // 600 (10m)


// ----- SystemLevel -----
// LED CONSTRAINTS
#define LED_MIN_PERIOD_SECONDS 1800 // 1800 (30min)
#define LED_MAX_PERIOD_SECONDS 86400 // 86400 (1day)
#define LED_DELAY_STEP_SECONDS 900 // 900 (15min)
#define LED_PERIOD_STEP_SECONDS 1800 // 1800 (30min)
// LED DEFAULTS
#define LED_DEFAULT_ON_SECONDS 43200 // 43200 (12hr)
#define LED_DEFAULT_OFF_SECONDS 43200 // 43200 (12hr)
#define LED_DEFAULT_START_SECONDS 7200 // 7200 (2hr)
#define LED_DEFAULT_STATE true // true (on)
// FD CONSTRAINTS
#define FD_MIN_PERIOD_SECONDS 1800 // 1800 (30min)
#define FD_MAX_PERIOD_SECONDS 345600 // 345600 (4days)
#define FD_PERIOD_STEP_SECONDS 1800 // 1800 (30min)
#define FD_MAX_STEP_SECONDS 86400 // 86400 (1day)
#define FD_DELAY_STEP_SECONDS 900 // 900 (15min)
#define FD_MIN_FLOOD_SECONDS 10 // 10
#define FD_MAX_FLOOD_SECONDS 120 // 120 (2min)
#define FD_MIN_DRAIN_SECONDS 60 // 60 (1min)
#define FD_MAX_DRAIN_SECONDS 1200 // 1200 (20min)
// FD DEFAULTS
#define FD_DEFAULT_PERIOD_SECONDS 28800 // 28800 (8hr)
#define FD_DEFAULT_START_SECONDS 21600 // 21600 (6hr)
#define FD_DEFAULT_FLOOD_SECONDS 60 // 60 (1min)
#define FD_DEFAULT_DRAIN_SECONDS 180 // 180 (3min)

// SAVING
#define SYSTEM_ADDRESS_WIDTH 10
#define SYSTEM_LEVEL_ADDRESS_WIDTH 20


#endif