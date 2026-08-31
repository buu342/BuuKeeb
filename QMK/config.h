// Comment this define to "overclock" the direct mode LEDs
// I will not be responsible for wrecked boards!
#define LIMIT_DIRECTMODE_BRIGHTNESS

#define WS2812_PWM_DRIVER PWMD15
#define WS2812_PWM_CHANNEL 2
#define WS2812_PWM_PAL_MODE 0
#define WS2812_PWM_DMA_STREAM STM32_DMA1_STREAM5
#define WS2812_PWM_DMA_CHANNEL 5

#define EECONFIG_USER_DATA_SIZE 324
#define EECONFIG_USER_DATA_VERSION 324