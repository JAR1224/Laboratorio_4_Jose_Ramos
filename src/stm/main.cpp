#include <stdint.h>
#include <math.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/cm3/systick.h>

#include <setjmp.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/usart.h>
#include <libopencm3/stm32/iwdg.h>
#include <libopencm3/cm3/scb.h>
#include <libopencm3/cm3/cortex.h>

#include <libopencm3/stm32/spi.h>

#include <libopencm3/stm32/fsmc.h>

#include <libopencm3/stm32/adc.h>


#define LED_DISCO_GREEN_PORT GPIOG
#define LED_DISCO_GREEN_PIN GPIO13

#define LED_DISCO_RED_PORT GPIOG
#define LED_DISCO_RED_PIN GPIO14

#define CONSOLE_UART	USART1

#define d2r(d) ((d) * 6.2831853 / 360.0)

#define conversion_factor 0.00241758

//--------------------------------------------------------------------------
//adc.c
//--------------------------------------------------------------------------

static void adc_setup(void)
{
	gpio_mode_setup(GPIOA, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO0);
	//gpio_mode_setup(GPIOA, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO1);
	gpio_mode_setup(GPIOA, GPIO_MODE_INPUT, GPIO_PUPD_NONE, GPIO1);

	adc_power_off(ADC1);
	adc_disable_scan_mode(ADC1);
	adc_set_sample_time_on_all_channels(ADC1, ADC_SMPR_SMP_3CYC);

	adc_power_on(ADC1);

}

static void adc_init(void)
{

	rcc_periph_clock_enable(RCC_ADC1);
	/* green led for ticking */
	gpio_mode_setup(LED_DISCO_GREEN_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE,LED_DISCO_GREEN_PIN);
	gpio_mode_setup(LED_DISCO_RED_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE,LED_DISCO_RED_PIN);

}

static uint16_t read_adc_naiive(uint8_t channel)
{
	uint8_t channel_array[16];
	channel_array[0] = channel;
	adc_set_regular_sequence(ADC1, 1, channel_array);
	adc_start_conversion_regular(ADC1);
	while (!adc_eoc(ADC1));
	uint16_t reg16 = adc_read_regular(ADC1);
	return reg16;
}

//--------------------------------------------------------------------------
//adc.c
//--------------------------------------------------------------------------


//--------------------------------------------------------------------------
//font-7x12.c
//--------------------------------------------------------------------------

#ifndef MCMFONT_INC
#define MCMFONT_INC

/* Ascii 7 x 12 font
 *
 * This is a pretty generic 7 x 12 ASCII font that includes lower case
 * descenders (indicated by (data & 0x80) != 0 on the first data byte of the
 * character. Each row represents 9 "lines" of the font, the bits in the byte
 * represent columns. When a glyph descends below the base line it is rendered
 * 3 pixels lower (hence the height of 12 pixels rather than 9 even though the
 * data is only 9 rows tall)
 */

#define FONT_CHAR_WIDTH 7
#define FONT_CHAR_HEIGHT 12

static const unsigned char mcm_font[] = {
	0x00, 0x00, 0x00, 0x00, 0x31, 0x4A, 0x44, 0x4A, 0x31,   /* 0 */
	0xBC, 0x22, 0x3B, 0x22, 0x22, 0x3b, 0x20, 0x20, 0x40,   /* 1 */
	0x80, 0x61, 0x12, 0x14, 0x18, 0x10, 0x30, 0x30, 0x30,   /* 2 */
	0x30, 0x48, 0x40, 0x40, 0x20, 0x30, 0x48, 0x48, 0x30,   /* 3 */
	0x00, 0x00, 0x18, 0x20, 0x40, 0x78, 0x40, 0x20, 0x30,   /* 4 */
	0x16, 0x0e, 0x10, 0x20, 0x40, 0x40, 0x38, 0x04, 0x18,   /* 5 */
	0x80, 0x2c, 0x52, 0x12, 0x12, 0x12, 0x02, 0x02, 0x02,   /* 6 */
	0x18, 0x24, 0x42, 0x42, 0x3e, 0x42, 0x42, 0x24, 0x18,   /* 7 */
	0x00, 0x00, 0x00, 0x40, 0x40, 0x40, 0x40, 0x48, 0x30,   /* 8 */
	0x00, 0x00, 0x40, 0x48, 0x50, 0x60, 0x50, 0x4A, 0x44,   /* 9 */
	0x40, 0x20, 0x10, 0x10, 0x10, 0x10, 0x18, 0x24, 0x42,   /* 10 */
	0x80, 0x48, 0x48, 0x48, 0x48, 0x74, 0x40, 0x40, 0x40,   /* 11 */
	0x00, 0x00, 0x00, 0x62, 0x22, 0x24, 0x28, 0x30, 0x20,   /* 12 */
	0x08, 0x1c, 0x20, 0x18, 0x20, 0x40, 0x3c, 0x02, 0x0c,   /* 13 */
	0x00, 0x00, 0x00, 0x18, 0x24, 0x42, 0x42, 0x24, 0x18,   /* 14 */
	0x00, 0x00, 0x00, 0x3f, 0x54, 0x24, 0x24, 0x24, 0x24,   /* 15 */

	0x98, 0x24, 0x42, 0x42, 0x64, 0x58, 0x40, 0x40, 0x40,   /* 16 */
	0x00, 0x00, 0x00, 0x1f, 0x24, 0x42, 0x42, 0x24, 0x18,   /* 17 */
	0x00, 0x00, 0x00, 0x3f, 0x48, 0x08, 0x08, 0x08, 0x08,   /* 18 */
	0x00, 0x00, 0x00, 0x62, 0x24, 0x24, 0x24, 0x24, 0x18,   /* 19 */
	0x10, 0x10, 0x38, 0x54, 0x54, 0x54, 0x38, 0x10, 0x10,   /* 20 */
	0x00, 0x00, 0x00, 0x00, 0x62, 0x14, 0x08, 0x14, 0x23,   /* 21 */
	0x80, 0x49, 0x2a, 0x2a, 0x2a, 0x1c, 0x08, 0x08, 0x08,   /* 22 */
	0x00, 0x00, 0x00, 0x22, 0x41, 0x49, 0x49, 0x49, 0x36,   /* 23 */
	0x00, 0x1c, 0x22, 0x41, 0x41, 0x41, 0x22, 0x22, 0x63,   /* 24 */
	0x0f, 0x10, 0x10, 0x10, 0x10, 0x10, 0x50, 0x30, 0x10,   /* 25 */
	0x00, 0x00, 0x04, 0x02, 0x7f, 0x02, 0x04, 0x00, 0x00,   /* 26 */
	0x00, 0x00, 0x10, 0x20, 0x7f, 0x20, 0x10, 0x00, 0x00,   /* 27 */
	0x08, 0x1c, 0x2a, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,   /* 28 */
	0x00, 0x00, 0x08, 0x00, 0x7f, 0x00, 0x08, 0x00, 0x00,   /* 29 */
	0x7f, 0x20, 0x10, 0x08, 0x06, 0x08, 0x10, 0x20, 0x7f,   /* 30 */
	0x00, 0x30, 0x45, 0x06, 0x30, 0x45, 0x06, 0x00, 0x00,   /* 31 */

	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,   /* 32 */
	0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x00, 0x00, 0x08,   /* 33 */
	0x12, 0x12, 0x12, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,   /* 34 */
	0x14, 0x14, 0x14, 0x7f, 0x14, 0x7f, 0x14, 0x14, 0x14,   /* 35 */
	0x08, 0x3f, 0x48, 0x48, 0x3e, 0x09, 0x09, 0x7e, 0x08,   /* 36 */
	0x20, 0x51, 0x22, 0x04, 0x08, 0x10, 0x22, 0x45, 0x02,   /* 37 */
	0x38, 0x44, 0x44, 0x28, 0x10, 0x29, 0x46, 0x46, 0x39,   /* 38 */
	0x20, 0x20, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,   /* 39 */
	0x08, 0x10, 0x20, 0x20, 0x20, 0x20, 0x20, 0x10, 0x08,   /* 40 */
	0x08, 0x04, 0x02, 0x02, 0x02, 0x02, 0x02, 0x04, 0x08,   /* 41 */
	0x80, 0x49, 0x2a, 0x1c, 0x7f, 0x1c, 0x2a, 0x49, 0x80,   /* 42 */
	0x00, 0x80, 0x80, 0x80, 0x7e, 0x80, 0x80, 0x80, 0x00,   /* 43 */
	0x80, 0x00, 0x00, 0x00, 0x00, 0x30, 0x10, 0x10, 0x20,   /* 44 */
	0x00, 0x00, 0x00, 0x00, 0x7f, 0x00, 0x00, 0x00, 0x00,   /* 45 */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10,   /* 46 */
	0x00, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x00,   /* 47 */

	0x3e, 0x41, 0x43, 0x45, 0x49, 0x51, 0x61, 0x41, 0x3e,   /* 48 */
	0x08, 0x18, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x3e,   /* 49 */
	0x3e, 0x41, 0x01, 0x02, 0x0c, 0x10, 0x20, 0x40, 0x7f,   /* 50 */
	0x3e, 0x41, 0x01, 0x01, 0x1e, 0x01, 0x01, 0x41, 0x3e,   /* 51 */
	0x02, 0x06, 0x0a, 0x12, 0x22, 0x7f, 0x02, 0x02, 0x02,   /* 52 */
	0x7f, 0x40, 0x40, 0x40, 0x7e, 0x01, 0x01, 0x41, 0x3e,   /* 53 */
	0x3e, 0x41, 0x40, 0x40, 0x7e, 0x41, 0x41, 0x41, 0x3e,   /* 54 */
	0x7f, 0x41, 0x02, 0x04, 0x08, 0x10, 0x10, 0x10, 0x10,   /* 55 */
	0x3e, 0x41, 0x41, 0x41, 0x3e, 0x41, 0x41, 0x41, 0x3e,   /* 56 */
	0x3f, 0x41, 0x41, 0x41, 0x3f, 0x01, 0x01, 0x41, 0x3e,   /* 57 */
	0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00,   /* 58 */
	0xa0, 0x00, 0x00, 0x00, 0x00, 0x30, 0x10, 0x10, 0x20,   /* 59 */
	0x04, 0x08, 0x10, 0x20, 0x40, 0x20, 0x10, 0x08, 0x04,   /* 60 */
	0x00, 0x00, 0x00, 0x7f, 0x00, 0x7f, 0x00, 0x00, 0x00,   /* 61 */
	0x10, 0x08, 0x04, 0x02, 0x01, 0x02, 0x04, 0x08, 0x10,   /* 62 */
	0x3e, 0x41, 0x41, 0x02, 0x04, 0x08, 0x08, 0x00, 0x08,   /* 63 */

	0x3e, 0x41, 0x41, 0x1d, 0x55, 0x5e, 0x40, 0x40, 0x3e,   /* 64 */
	0x1c, 0x22, 0x41, 0x41, 0x7f, 0x41, 0x41, 0x41, 0x41,   /* 65 */
	0x7e, 0x21, 0x21, 0x21, 0x3e, 0x21, 0x21, 0x21, 0x7e,   /* 66 */
	0x1e, 0x21, 0x40, 0x40, 0x40, 0x40, 0x40, 0x21, 0x1e,   /* 67 */
	0x7e, 0x21, 0x21, 0x21, 0x21, 0x21, 0x21, 0x21, 0x7e,   /* 68 */
	0x7f, 0x40, 0x40, 0x40, 0x78, 0x40, 0x40, 0x40, 0x7f,   /* 69 */
	0x7f, 0x40, 0x40, 0x40, 0x78, 0x40, 0x40, 0x40, 0x40,   /* 70 */
	0x1e, 0x21, 0x40, 0x40, 0x40, 0x47, 0x41, 0x21, 0x1e,   /* 71 */
	0x41, 0x41, 0x41, 0x41, 0x7f, 0x41, 0x41, 0x41, 0x41,   /* 72 */
	0x3e, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x3e,   /* 73 */
	0x1f, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x44, 0x38,   /* 74 */
	0x41, 0x42, 0x44, 0x48, 0x70, 0x48, 0x44, 0x42, 0x41,   /* 75 */
	0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x7f,   /* 76 */
	0x41, 0x63, 0x55, 0x49, 0x49, 0x41, 0x41, 0x41, 0x41,   /* 77 */
	0x41, 0x61, 0x51, 0x49, 0x45, 0x43, 0x41, 0x41, 0x41,   /* 78 */
	0x1c, 0x22, 0x41, 0x41, 0x41, 0x41, 0x41, 0x22, 0x1c,   /* 79 */

	0x7e, 0x41, 0x41, 0x41, 0x7e, 0x40, 0x40, 0x40, 0x40,   /* 80 */
	0x1c, 0x22, 0x41, 0x41, 0x41, 0x41, 0x45, 0x22, 0x1d,   /* 81 */
	0x7e, 0x41, 0x41, 0x41, 0x7e, 0x48, 0x44, 0x42, 0x41,   /* 82 */
	0x3e, 0x41, 0x40, 0x40, 0x3e, 0x01, 0x01, 0x41, 0x3e,   /* 83 */
	0x7f, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,   /* 84 */
	0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x3e,   /* 85 */
	0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x22, 0x14, 0x08,   /* 86 */
	0x41, 0x41, 0x41, 0x49, 0x49, 0x49, 0x55, 0x63, 0x41,   /* 87 */
	0x41, 0x41, 0x22, 0x14, 0x08, 0x14, 0x22, 0x41, 0x41,   /* 88 */
	0x41, 0x41, 0x22, 0x14, 0x08, 0x08, 0x08, 0x08, 0x08,   /* 89 */
	0x7f, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x7f,   /* 90 */
	0x1e, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x1e,   /* 91 */
	0x00, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01, 0x00,   /* 92 */
	0x3c, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x3c,   /* 93 */
	0x3e, 0x41, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,   /* 94 */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7f,   /* 95 */

	0x02, 0x02, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,   /* 96 */
	0x00, 0x00, 0x00, 0x1e, 0x22, 0x42, 0x41, 0x46, 0x3a,   /* 97 */
	0x40, 0x40, 0x40, 0x5c, 0x62, 0x42, 0x42, 0x62, 0x5c,   /* 98 */
	0x00, 0x00, 0x00, 0x3c, 0x42, 0x40, 0x40, 0x42, 0x3c,   /* 99 */
	0x02, 0x02, 0x02, 0x3a, 0x46, 0x42, 0x42, 0x46, 0x3a,   /* 100 */
	0x00, 0x00, 0x00, 0x3c, 0x42, 0x42, 0x7e, 0x40, 0x3e,   /* 101 */
	0x0c, 0x12, 0x10, 0x10, 0x7c, 0x10, 0x10, 0x10, 0x10,   /* 102 */
	0xba, 0x46, 0x42, 0x42, 0x46, 0x3a, 0x02, 0x42, 0x3c,   /* 103 */
	0x40, 0x40, 0x40, 0x58, 0x64, 0x42, 0x42, 0x42, 0x42,   /* 104 */
	0x00, 0x08, 0x00, 0x18, 0x08, 0x08, 0x08, 0x08, 0x08,   /* 105 */
	0x82, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x42, 0x3c,   /* 106 */
	0x40, 0x40, 0x40, 0x44, 0x48, 0x70, 0x48, 0x44, 0x42,   /* 107 */
	0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10,   /* 108 */
	0x00, 0x00, 0x00, 0x76, 0x49, 0x49, 0x49, 0x49, 0x49,   /* 109 */
	0x00, 0x00, 0x00, 0x5c, 0x62, 0x42, 0x42, 0x42, 0x42,   /* 110 */
	0x00, 0x00, 0x00, 0x3c, 0x42, 0x42, 0x42, 0x42, 0x3c,   /* 111 */

	0xdc, 0x62, 0x42, 0x42, 0x62, 0x5c, 0x40, 0x40, 0x40,   /* 112 */
	0xba, 0x46, 0x42, 0x42, 0x46, 0x3a, 0x02, 0x02, 0x02,   /* 113 */
	0x00, 0x00, 0x00, 0x5c, 0x62, 0x40, 0x40, 0x40, 0x40,   /* 114 */
	0x00, 0x00, 0x00, 0x3c, 0x42, 0x30, 0x0c, 0x42, 0x3c,   /* 115 */
	0x00, 0x10, 0x10, 0x7c, 0x10, 0x01, 0x10, 0x12, 0x0c,   /* 116 */
	0x00, 0x00, 0x00, 0x42, 0x42, 0x42, 0x42, 0x42, 0x3c,   /* 117 */
	0x00, 0x00, 0x00, 0x44, 0x44, 0x44, 0x44, 0x28, 0x10,   /* 118 */
	0x00, 0x00, 0x00, 0x41, 0x49, 0x49, 0x49, 0x49, 0x36,   /* 119 */
	0x00, 0x00, 0x00, 0x42, 0x24, 0x18, 0x18, 0x24, 0x42,   /* 120 */
	0xc2, 0x42, 0x42, 0x42, 0x46, 0x3a, 0x02, 0x42, 0x3c,   /* 121 */
	0x00, 0x00, 0x00, 0x7e, 0x04, 0x08, 0x10, 0x20, 0x7e,   /* 122 */
	0x0e, 0x10, 0x10, 0x10, 0x20, 0x10, 0x10, 0x10, 0x0e,   /* 123 */
	0x10, 0x10, 0x10, 0x00, 0x00, 0x10, 0x10, 0x10, 0x00,   /* 124 */
	0x38, 0x04, 0x04, 0x04, 0x02, 0x04, 0x04, 0x04, 0x38,   /* 125 */
	0x30, 0x49, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,   /* 126 */
	0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f    /* 127 */
};

#endif

//--------------------------------------------------------------------------
//font-7x12.c
//--------------------------------------------------------------------------


//--------------------------------------------------------------------------
//clock.c
//--------------------------------------------------------------------------
/* milliseconds since boot */
static volatile uint32_t system_millis;

/* Called when systick fires */
void sys_tick_handler(void)
{
	system_millis++;
}

/* simple sleep for delay milliseconds */
void msleep(uint32_t delay)
{
	uint32_t wake = system_millis + delay;
	while (wake > system_millis);
}

/* Getter function for the current time */
uint32_t mtime(void)
{
	return system_millis;
}

/*
 * clock_setup(void)
 *
 * This function sets up both the base board clock rate
 * and a 1khz "system tick" count. The SYSTICK counter is
 * a standard feature of the Cortex-M series.
 */
void clock_setup(void)
{
	/* Base board frequency, set to 168Mhz */
	rcc_clock_setup_pll(&rcc_hse_8mhz_3v3[RCC_CLOCK_3V3_168MHZ]);

	/* clock rate / 168000 to get 1mS interrupt rate */
	systick_set_reload(168000);
	systick_set_clocksource(STK_CSR_CLKSOURCE_AHB);
	systick_counter_enable();

	/* this done last */
	systick_interrupt_enable();
}
//--------------------------------------------------------------------------
//clock.c
//--------------------------------------------------------------------------


//--------------------------------------------------------------------------
//console.c
//--------------------------------------------------------------------------
/* This is a ring buffer to holding characters as they are typed
 * it maintains both the place to put the next character received
 * from the UART, and the place where the last character was
 * read by the program. See the README file for a discussion of
 * the failure semantics.
 */
#define RECV_BUF_SIZE	128		/* Arbitrary buffer size */
char recv_buf[RECV_BUF_SIZE];
volatile int recv_ndx_nxt;		/* Next place to store */
volatile int recv_ndx_cur;		/* Next place to read */

/* For interrupt handling we add a new function which is called
 * when recieve interrupts happen. The name (usart1_isr) is created
 * by the irq.json file in libopencm3 calling this interrupt for
 * USART1 'usart1', adding the suffix '_isr', and then weakly binding
 * it to the 'do nothing' interrupt function in vec.c.
 *
 * By defining it in this file the linker will override that weak
 * binding and instead bind it here, but you have to get the name
 * right or it won't work. And you'll wonder where your interrupts
 * are going.
 */
void usart1_isr(void)
{
	uint32_t reg;
	int i;

	do {
		reg = USART_SR(CONSOLE_UART);
		if (reg & USART_SR_RXNE) {
			recv_buf[recv_ndx_nxt] = USART_DR(CONSOLE_UART);
#ifdef RESET_ON_CTRLC
			/*
			 * This bit of code will jump to the ResetHandler if you
			 * hit ^C
			 */
			if (recv_buf[recv_ndx_nxt] == '\003') {
				scb_reset_system();
				return; /* never actually reached */
			}
#endif
			/* Check for "overrun" */
			i = (recv_ndx_nxt + 1) % RECV_BUF_SIZE;
			if (i != recv_ndx_cur) {
				recv_ndx_nxt = i;
			}
		}
	} while ((reg & USART_SR_RXNE) != 0); /* can read back-to-back
						 interrupts */
}

/*
 * console_putc(char c)
 *
 * Send the character 'c' to the USART, wait for the USART
 * transmit buffer to be empty first.
 */
void console_putc(char c)
{
	uint32_t	reg;
	do {
		reg = USART_SR(CONSOLE_UART);
	} while ((reg & USART_SR_TXE) == 0);
	USART_DR(CONSOLE_UART) = (uint16_t) c & 0xff;
}

/*
 * char = console_getc(int wait)
 *
 * Check the console for a character. If the wait flag is
 * non-zero. Continue checking until a character is received
 * otherwise return 0 if called and no character was available.
 *
 * The implementation is a bit different however, now it looks
 * in the ring buffer to see if a character has arrived.
 */
char console_getc(int wait)
{
	char		c = 0;

	while ((wait != 0) && (recv_ndx_cur == recv_ndx_nxt));
	if (recv_ndx_cur != recv_ndx_nxt) {
		c = recv_buf[recv_ndx_cur];
		recv_ndx_cur = (recv_ndx_cur + 1) % RECV_BUF_SIZE;
	}
	return c;
}

/*
 * void console_puts(char *s)
 *
 * Send a string to the console, one character at a time, return
 * after the last character, as indicated by a NUL character, is
 * reached.
 */
void console_puts(char *s)
{
	while (*s != '\000') {
		console_putc(*s);
		/* Add in a carraige return, after sending line feed */
		if (*s == '\n') {
			console_putc('\r');
		}
		s++;
	}
}

/*
 * int console_gets(char *s, int len)
 *
 * Wait for a string to be entered on the console, limited
 * support for editing characters (back space and delete)
 * end when a <CR> character is received.
 */
int console_gets(char *s, int len)
{
	char *t = s;
	char c;

	*t = '\000';
	/* read until a <CR> is received */
	while ((c = console_getc(1)) != '\r') {
		if ((c == '\010') || (c == '\127')) {
			if (t > s) {
				/* send ^H ^H to erase previous character */
				console_puts("\010 \010");
				t--;
			}
		} else {
			*t = c;
			console_putc(c);
			if ((t - s) < len) {
				t++;
			}
		}
		/* update end of string with NUL */
		*t = '\000';
	}
	return t - s;
}

/*
 * console_setup(int baudrate)
 *
 * Set the pins and clocks to create a console that we can
 * use for serial messages and getting text from the user.
 */
void console_setup(int baud)
{
	/* MUST enable the GPIO clock in ADDITION to the USART clock */
	rcc_periph_clock_enable(RCC_GPIOA);

	/* This example uses PA9 and PA10 for Tx and Rx respectively
	 * but other pins are available for this role on USART1 (our chosen
	 * USART) as well, we are using these because they are connected to the
	 * programmer through some jumpers.
	 */
	gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO9 | GPIO10);

	/* Actual Alternate function number (in this case 7) is part
	 * depenedent, CHECK THE DATA SHEET for the right number to
	 * use.
	 */
	gpio_set_af(GPIOA, GPIO_AF7, GPIO9 | GPIO10);


	/* This then enables the clock to the USART1 peripheral which is
	 * attached inside the chip to the APB1 bus. Different peripherals
	 * attach to different buses, and even some UARTS are attached to
	 * APB1 and some to APB2, again the data sheet is useful here.
	 * We are using rcc_periph_clock_enable that knows which peripheral is
	 * on which clock bus and sets up everything accordingly.
	 */
	rcc_periph_clock_enable(RCC_USART1);

	/* Set up USART/UART parameters using the libopencm3 helper functions */
	usart_set_baudrate(CONSOLE_UART, baud);
	usart_set_databits(CONSOLE_UART, 8);
	usart_set_stopbits(CONSOLE_UART, USART_STOPBITS_1);
	usart_set_mode(CONSOLE_UART, USART_MODE_TX_RX);
	usart_set_parity(CONSOLE_UART, USART_PARITY_NONE);
	usart_set_flow_control(CONSOLE_UART, USART_FLOWCONTROL_NONE);
	usart_enable(CONSOLE_UART);

	/* Enable interrupts from the USART */
	nvic_enable_irq(NVIC_USART1_IRQ);

	/* Specifically enable recieve interrupts */
	usart_enable_rx_interrupt(CONSOLE_UART);
}
//--------------------------------------------------------------------------
//console.c
//--------------------------------------------------------------------------



//--------------------------------------------------------------------------
//spi.c
//--------------------------------------------------------------------------
/*
 * Functions defined for accessing the SPI port 8 bits at a time
 */
uint16_t read_reg(int reg);
void write_reg(uint8_t reg, uint8_t value);
uint8_t read_xyz(int16_t vecs[3]);
void spi_init(void);

void spi_init() {
  spi_set_master_mode(SPI5);
  spi_set_baudrate_prescaler(SPI5, SPI_CR1_BR_FPCLK_DIV_64);
  spi_set_clock_polarity_0(SPI5);
  spi_set_clock_phase_0(SPI5);
  spi_set_full_duplex_mode(SPI5);
  spi_set_unidirectional_mode(SPI5); /* bidirectional but in 3-wire */
  //spi_set_data_size(SPI5, SPI_CR2_DS_8BIT);
  spi_enable_software_slave_management(SPI5);
  spi_send_msb_first(SPI5);
  spi_set_nss_high(SPI5);
  //spi_enable_ss_output(SPI5);
  //spi_fifo_reception_threshold_8bit(SPI5);
  //SPI_I2SCFGR(SPI5) &= ~SPI_I2SCFGR_I2SMOD;
  spi_enable(SPI5);
  
}

/*
 * Chart of the various SPI ports (1 - 6) and where their pins can be:
 *
 *	 NSS		  SCK			MISO		MOSI
 *	 --------------   -------------------   -------------   ---------------
 * SPI1  PA4, PA15	  PA5, PB3		PA6, PB4	PA7, PB5
 * SPI2  PB9, PB12, PI0   PB10, PB13, PD3, PI1  PB14, PC2, PI2  PB15, PC3, PI3
 * SPI3  PA15*, PA4*	  PB3*, PC10*		PB4*, PC11*	PB5*, PD6, PC12*
 * SPI4  PE4,PE11	  PE2, PE12		PE5, PE13	PE6, PE14
 * SPI5  PF6, PH5	  PF7, PH6		PF8		PF9, PF11, PH7
 * SPI6  PG8		  PG13			PG12		PG14
 *
 * Pin name with * is alternate function 6 otherwise use alternate function 5.
 *
 * MEMS uses SPI5 - SCK (PF7), MISO (PF8), MOSI (PF9),
 * MEMS CS* (PC1)  -- GPIO
 * MEMS INT1 = PA1, MEMS INT2 = PA2
 */

void put_status(char *m);

/*
 * put_status(char *)
 *
 * This is a helper function I wrote to watch the status register
 * it decodes the bits and prints them on the console. Sometimes
 * the SPI port comes up with the MODF flag set, you have to re-read
 * the status port and re-write the control register to clear that.
 */
void put_status(char *m)
{
	uint16_t stmp;

	console_puts(m);
	console_puts(" Status: ");
	stmp = SPI_SR(SPI5);
	if (stmp & SPI_SR_TXE) {
		console_puts("TXE, ");
	}
	if (stmp & SPI_SR_RXNE) {
		console_puts("RXNE, ");
	}
	if (stmp & SPI_SR_BSY) {
		console_puts("BUSY, ");
	}
	if (stmp & SPI_SR_OVR) {
		console_puts("OVERRUN, ");
	}
	if (stmp & SPI_SR_MODF) {
		console_puts("MODE FAULT, ");
	}
	if (stmp & SPI_SR_CRCERR) {
		console_puts("CRC, ");
	}
	if (stmp & SPI_SR_UDR) {
		console_puts("UNDERRUN, ");
	}
	console_puts("\n");
}

/*
 * read_reg(int reg)
 *
 * This reads the MEMs registers. The chip registers are 16 bits
 * wide, but I read it as two 8 bit bytes. Originally I tried
 * swapping between an 8 bit and 16 bit wide bus but that confused
 * both my code and the chip after a while so this was found to
 * be a more stable solution.
 */
uint16_t
read_reg(int reg)
{
	uint16_t d1, d2;

	d1 = 0x80 | (reg & 0x3f); /* Read operation */
	/* Nominallly a register read is a 16 bit operation */
	gpio_clear(GPIOC, GPIO1);
	spi_send(SPI5, d1);
	d2 = spi_read(SPI5);
	d2 <<= 8;
	/*
	 * You have to send as many bits as you want to read
	 * so we send another 8 bits to get the rest of the
	 * register.
	 */
	spi_send(SPI5, 0);
	d2 |= spi_read(SPI5);
	gpio_set(GPIOC, GPIO1);
	return d2;
}

/*
 * uint8_t status = read_xyz(int16_t [])
 *
 * This function exploits the fact that you can do a read +
 * auto increment of the SPI registers. It starts at the
 * address of the X register and reads 6 bytes.
 *
 * Then the status register is read and returned.
 */
uint8_t
read_xyz(int16_t vecs[3])
{
	uint8_t	 buf[7];
	int		 i;

	gpio_clear(GPIOC, GPIO1); /* CS* select */
	spi_send(SPI5, 0xc0 | 0x28);
	(void) spi_read(SPI5);
	for (i = 0; i < 6; i++) {
		spi_send(SPI5, 0);
		buf[i] = spi_read(SPI5);
	}
	gpio_set(GPIOC, GPIO1); /* CS* deselect */
	vecs[0] = (buf[1] << 8 | buf[0]);
	vecs[1] = (buf[3] << 8 | buf[2]);
	vecs[2] = (buf[5] << 8 | buf[4]);
	return read_reg(0x27); /* Status register */
}

/*
 * void write_reg(uint8_t register, uint8_t value)
 *
 * This code then writes into a register on the chip first
 * selecting it and then writing to it.
 */
void
write_reg(uint8_t reg, uint8_t value)
{
	gpio_clear(GPIOC, GPIO1); /* CS* select */
	spi_send(SPI5, reg);
	(void) spi_read(SPI5);
	spi_send(SPI5, value);
	(void) spi_read(SPI5);
	gpio_set(GPIOC, GPIO1); /* CS* deselect */
	return;
}

int print_decimal(int);

/*
 * int len = print_decimal(int value)
 *
 * Very simple routine to print an integer as a decimal
 * number on the console.
 */
int
print_decimal(int num)
{
	int		ndx = 0;
	char	buf[10];
	int		len = 0;
	char	is_signed = 0;

	if (num < 0) {
		is_signed++;
		num = 0 - num;
	}
	buf[ndx++] = '\000';
	do {
		buf[ndx++] = (num % 10) + '0';
		num = num / 10;
	} while (num != 0);
	ndx--;
	if (is_signed != 0) {
		console_putc('-');
		len++;
	}
	while (buf[ndx] != '\000') {
		console_putc(buf[ndx--]);
		len++;
	}
	return len; /* number of characters printed */
}



char *axes[] = { "X: ", "Y: ", "Z: " };
//--------------------------------------------------------------------------
//spi.c
//--------------------------------------------------------------------------


//--------------------------------------------------------------------------
//sdram.c
//--------------------------------------------------------------------------

#ifndef __SDRAM_H
#define __SDRAM_H

#define SDRAM_BASE_ADDRESS ((uint8_t *)(0xd0000000))

#ifndef NULL
#define NULL	(void *)(0)
#endif

#endif

/*
 * This is just syntactic sugar but it helps, all of these
 * GPIO pins get configured in exactly the same way.
 */
static struct {
	uint32_t	gpio;
	uint16_t	pins;
} sdram_pins[6] = {
	{GPIOB, GPIO5 | GPIO6 },
	{GPIOC, GPIO0 },
	{GPIOD, GPIO0 | GPIO1 | GPIO8 | GPIO9 | GPIO10 | GPIO14 | GPIO15},
	{GPIOE, GPIO0 | GPIO1 | GPIO7 | GPIO8 | GPIO9 | GPIO10 |
			GPIO11 | GPIO12 | GPIO13 | GPIO14 | GPIO15 },
	{GPIOF, GPIO0 | GPIO1 | GPIO2 | GPIO3 | GPIO4 | GPIO5 | GPIO11 |
			GPIO12 | GPIO13 | GPIO14 | GPIO15 },
	{GPIOG, GPIO0 | GPIO1 | GPIO4 | GPIO5 | GPIO8 | GPIO15}
};

static struct sdram_timing timing = {
	.trcd = 2,		/* RCD Delay */
	.trp = 2,		/* RP Delay */
	.twr = 2,		/* Write Recovery Time */
	.trc = 7,		/* Row Cycle Delay */
	.tras = 4,		/* Self Refresh Time */
	.txsr = 7,		/* Exit Self Refresh Time */
	.tmrd = 2,		/* Load to Active Delay */
};

/*
 * Initialize the SD RAM controller.
 */
void
sdram_init(void) {
	int i;
	uint32_t cr_tmp, tr_tmp; /* control, timing registers */

	/*
	* First all the GPIO pins that end up as SDRAM pins
	*/
	rcc_periph_clock_enable(RCC_GPIOB);
	rcc_periph_clock_enable(RCC_GPIOC);
	rcc_periph_clock_enable(RCC_GPIOD);
	rcc_periph_clock_enable(RCC_GPIOE);
	rcc_periph_clock_enable(RCC_GPIOF);
	rcc_periph_clock_enable(RCC_GPIOG);

	for (i = 0; i < 6; i++) {
		gpio_mode_setup(sdram_pins[i].gpio, GPIO_MODE_AF,
				GPIO_PUPD_NONE, sdram_pins[i].pins);
		gpio_set_output_options(sdram_pins[i].gpio, GPIO_OTYPE_PP,
					GPIO_OSPEED_50MHZ, sdram_pins[i].pins);
		gpio_set_af(sdram_pins[i].gpio, GPIO_AF12, sdram_pins[i].pins);
	}

	/* Enable the SDRAM Controller */
#if 1
	rcc_periph_clock_enable(RCC_FSMC);
#else
	rcc_peripheral_enable_clock(&RCC_AHB3ENR, RCC_AHB3ENR_FMCEN);
#endif

	/* Note the STM32F429-DISCO board has the ram attached to bank 2 */
	/* Timing parameters computed for a 168Mhz clock */
	/* These parameters are specific to the SDRAM chip on the board */

	cr_tmp  = FMC_SDCR_RPIPE_1CLK;
	cr_tmp |= FMC_SDCR_SDCLK_2HCLK;
	cr_tmp |= FMC_SDCR_CAS_3CYC;
	cr_tmp |= FMC_SDCR_NB4;
	cr_tmp |= FMC_SDCR_MWID_16b;
	cr_tmp |= FMC_SDCR_NR_12;
	cr_tmp |= FMC_SDCR_NC_8;

	/* We're programming BANK 2, but per the manual some of the parameters
	 * only work in CR1 and TR1 so we pull those off and put them in the
	 * right place.
	 */
	FMC_SDCR1 |= (cr_tmp & FMC_SDCR_DNC_MASK);
	FMC_SDCR2 = cr_tmp;

	tr_tmp = sdram_timing(&timing);
	FMC_SDTR1 |= (tr_tmp & FMC_SDTR_DNC_MASK);
	FMC_SDTR2 = tr_tmp;

	/* Now start up the Controller per the manual
	 *	- Clock config enable
	 *	- PALL state
	 *	- set auto refresh
	 *	- Load the Mode Register
	 */
	sdram_command(SDRAM_BANK2, SDRAM_CLK_CONF, 1, 0);
	msleep(1); /* sleep at least 100uS */
	sdram_command(SDRAM_BANK2, SDRAM_PALL, 1, 0);
	sdram_command(SDRAM_BANK2, SDRAM_AUTO_REFRESH, 4, 0);
	tr_tmp = SDRAM_MODE_BURST_LENGTH_2				|
				SDRAM_MODE_BURST_TYPE_SEQUENTIAL	|
				SDRAM_MODE_CAS_LATENCY_3		|
				SDRAM_MODE_OPERATING_MODE_STANDARD	|
				SDRAM_MODE_WRITEBURST_MODE_SINGLE;
	sdram_command(SDRAM_BANK2, SDRAM_LOAD_MODE, 1, tr_tmp);

	/*
	 * set the refresh counter to insure we kick off an
	 * auto refresh often enough to prevent data loss.
	 */
	FMC_SDRTR = 683;
	/* and Poof! a 8 megabytes of ram shows up in the address space */
}

//--------------------------------------------------------------------------
//sdram.c
//--------------------------------------------------------------------------


//--------------------------------------------------------------------------
//lcd-spi.c
//--------------------------------------------------------------------------

#ifndef __LCD_SPI_H
#define LCD_SPI_H

/* Color definitions */
#define	LCD_BLACK   0x0000
#define	LCD_BLUE    0x1F00
#define	LCD_RED     0x00F8
#define	LCD_GREEN   0xE007
#define LCD_CYAN    0xFF07
#define LCD_MAGENTA 0x1FF8
#define LCD_YELLOW  0xE0FF
#define LCD_WHITE   0xFFFF
#define LCD_GREY    0xc339

/*
 * SPI Port and GPIO Defined - for STM32F4-Disco
 */
/* #define LCD_RESET   PA3  not used */
#define LCD_CS      PC2     /* CH 1 */
#define LCD_SCK     PF7     /* CH 2 */
#define LCD_DC      PD13    /* CH 4 */
#define LCD_MOSI    PF9     /* CH 3 */

#define LCD_SPI     SPI5

#define LCD_WIDTH   240
#define LCD_HEIGHT  320

#define FRAME_SIZE  (LCD_WIDTH * LCD_HEIGHT)
#define FRAME_SIZE_BYTES    (FRAME_SIZE * 2)
#endif

/* forward prototypes for some helper functions */
static int print_decimal(int v);
static int print_hex(int v);

/* Simple double buffering, one frame is displayed, the
 * other being built.
 */
uint16_t *cur_frame;
uint16_t *display_frame;


/*
 * Drawing a pixel consists of storing a 16 bit value in the
 * memory used to hold the frame. This code computes the address
 * of the word to store, and puts in the value we pass to it.
 */
void
lcd_draw_pixel(int x, int y, uint16_t color)
{
	*(cur_frame + x + y * LCD_WIDTH) = color;
}

/*
 * Fun fact, same SPI port as the MEMS example but different
 * I/O pins. Clearly you can't use both the SPI port and the
 * MEMS chip at the same time in this example.
 *
 * For the STM32-DISCO board, SPI pins in use:
 *  N/C - RESET
 *  PC2 - CS (could be NSS but won't be)
 *  PF7 - SCLK (AF5) SPI5
 *  PD13 - DATA / CMD*
 *  PF9 - MOSI (AF5) SPI5
 */

/*
 * This structure defines the sequence of commands to send
 * to the Display in order to initialize it. The AdaFruit
 * folks do something similar, it helps when debugging the
 * initialization sequence for the display.
 */
struct tft_command {
	uint16_t delay;		/* If you need a delay after */
	uint8_t cmd;		/* command to send */
	uint8_t n_args;		/* How many arguments it has */
};


/* prototype for lcd_command */
static void lcd_command(uint8_t cmd, int delay, int n_args,
						const uint8_t *args);

/*
 * void lcd_command(cmd, delay, args, arg_ptr)
 *
 * All singing all dancing 'do a command' feature. Basically it
 * sends a command, and if args are present it sets 'data' and
 * sends those along too.
 */
static void
lcd_command(uint8_t cmd, int delay, int n_args, const uint8_t *args)
{
	int i;

	gpio_clear(GPIOC, GPIO2);	/* Select the LCD */
	(void) spi_xfer(LCD_SPI, cmd);
	if (n_args) {
		gpio_set(GPIOD, GPIO13);	/* Set the D/CX pin */
		for (i = 0; i < n_args; i++) {
			(void) spi_xfer(LCD_SPI, *(args+i));
		}
	}
	gpio_set(GPIOC, GPIO2);		/* Turn off chip select */
	gpio_clear(GPIOD, GPIO13);	/* always reset D/CX */
	if (delay) {
		msleep(delay);		/* wait, if called for */
	}
}

/*
 * This creates a 'script' of commands that can be played
 * to the LCD controller to initialize it.
 * One array holds the 'argument' bytes, the other
 * the commands.
 * Keeping them in sync is essential
 */
static const uint8_t cmd_args[] = {
	0x00, 0x1B,
	0x0a, 0xa2,
	0x10,
	0x10,
	0x45, 0x15,
	0x90,
/*    0xc8,*/                 /* original */
/*                  11001000 = MY, MX, BGR */
	0x08,
	0xc2,
	0x55,
	0x0a, 0xa7, 0x27, 0x04,
	0x00, 0x00, 0x00, 0xef,
	0x00, 0x00, 0x01, 0x3f,
/*    0x01, 0x00, 0x06,*/         /* original */
	0x01, 0x00, 0x00,           /* modified to remove RGB mode */
	0x01,
	0x0F, 0x29, 0x24, 0x0C, 0x0E,
	0x09, 0x4E, 0x78, 0x3C, 0x09,
	0x13, 0x05, 0x17, 0x11, 0x00,
	0x00, 0x16, 0x1B, 0x04, 0x11,
	0x07, 0x31, 0x33, 0x42, 0x05,
	0x0C, 0x0A, 0x28, 0x2F, 0x0F,
};

/*
 * These are the commands we're going to send to the
 * display to initialize it. We send them all, in sequence
 * with occasional delays. Commands that require data bytes
 * as arguments, indicate how many bytes to pull out the
 * above array to include.
 *
 * The sequence was pieced together from the ST Micro demo
 * code, the data sheet, and other sources on the web.
 */
const struct tft_command  initialization[] = {
	{   0, 0xb1, 2 },	/* 0x00, 0x1B, */
	{   0, 0xb6, 2 },	/* 0x0a, 0xa2, */
	{   0, 0xc0, 1 },	/* 0x10, */
	{   0, 0xc1, 1 },	/* 0x10, */
	{   0, 0xc5, 2 },	/* 0x45, 0x15, */
	{   0, 0xc7, 1 },	/* 0x90, */
	{   0, 0x36, 1 },	/* 0xc8, */
	{   0, 0xb0, 1 },	/* 0xc2, */
	{   0, 0x3a, 1 },	/* 0x55 **added, pixel format 16 bpp */
	{   0, 0xb6, 4 },	/* 0x0a, 0xa7, 0x27, 0x04, */
	{   0, 0x2A, 4 },	/* 0x00, 0x00, 0x00, 0xef, */
	{   0, 0x2B, 4 },	/* 0x00, 0x00, 0x01, 0x3f, */
	{   0, 0xf6, 3 },	/* 0x01, 0x00, 0x06, */
	{ 200, 0x2c, 0 },
	{   0, 0x26, 1},	/* 0x01, */
	{   0, 0xe0, 15 },	/* 0x0F, 0x29, 0x24, 0x0C, 0x0E, */
				/* 0x09, 0x4E, 0x78, 0x3C, 0x09, */
				/* 0x13, 0x05, 0x17, 0x11, 0x00, */
	{   0, 0xe1, 15 },	/* 0x00, 0x16, 0x1B, 0x04, 0x11, */
				/* 0x07, 0x31, 0x33, 0x42, 0x05, */
				/* 0x0C, 0x0A, 0x28, 0x2F, 0x0F, */
	{ 200, 0x11, 0 },
	{   0, 0x29, 0 },
	{   0,    0, 0 }	/* cmd == 0 indicates last command */
};

/* prototype for initialize_display */
static void initialize_display(const struct tft_command cmds[]);

/*
 * void initialize_display(struct cmds[])
 *
 * This is the function that sends the entire list. It also puts
 * the commands it is sending to the console.
 */
static void
initialize_display(const struct tft_command cmds[])
{
	int i = 0;
	int arg_offset = 0;
	int j;

	/* Initially arg offset is zero, so each time we 'consume'
	 * a few bytes in the args array the offset is moved and
	 * that changes the pointer we send to the command function.
	 */
	while (cmds[i].cmd) {


		lcd_command(cmds[i].cmd, cmds[i].delay, cmds[i].n_args,
			&cmd_args[arg_offset]);
		arg_offset += cmds[i].n_args;
		i++;
	}
	//console_puts("Done.\n");
}

/* prototype for test_image */
static void test_image(void);

/*
 * Interesting questions:
 *   - How quickly can I write a full frame?
 *      * Take the bits sent (16 * width * height)
 *        and divide by the  baud rate (10.25Mhz)
 *      * Tests in main.c show that yes, it taks 74ms.
 *
 * Create non-random data in the frame buffer. In our case
 * a black background and a grid 16 pixels x 16 pixels of
 * white lines. No line on the right edge and bottom of screen.
 */
static void
test_image(void)
{
	int		x, y;
	uint16_t	pixel;

	for (x = 0; x < LCD_WIDTH; x++) {
		for (y = 0; y < LCD_HEIGHT; y++) {
			pixel = 0;			/* all black */
			if ((x % 16) == 0) {
				pixel = 0xffff;		/* all white */
			}
			if ((y % 16) == 0) {
				pixel = 0xffff;		/* all white */
			}
			lcd_draw_pixel(x, y, pixel);
		}
	}
}

/*
 * void lcd_show_frame(void)
 *
 * Dump an entire frame to the LCD all at once. In theory you
 * could call this with DMA but that is made more difficult by
 * the implementation of SPI and the modules interpretation of
 * D/CX line.
 */
void lcd_show_frame(void)
{
	uint16_t	*t;
	uint8_t size[4];

	t = display_frame;
	display_frame = cur_frame;
	cur_frame = t;
	/*  */
	size[0] = 0;
	size[1] = 0;
	size[2] = (LCD_WIDTH >> 8) & 0xff;
	size[3] = (LCD_WIDTH) & 0xff;
	lcd_command(0x2A, 0, 4, (const uint8_t *)&size[0]);
	size[0] = 0;
	size[1] = 0;
	size[2] = (LCD_HEIGHT >> 8) & 0xff;
	size[3] = LCD_HEIGHT & 0xff;
	lcd_command(0x2B, 0, 4, (const uint8_t *)&size[0]);
	lcd_command(0x2C, 0, FRAME_SIZE_BYTES, (const uint8_t *)display_frame);
}

/*
 * void lcd_spi_init(void)
 *
 * Initialize the SPI port, and the through that port
 * initialize the LCD controller. Note that this code
 * will expect to be able to draw into the SDRAM on
 * the board, so the sdram much be initialized before
 * calling this function.
 *
 * SPI Port and GPIO Defined - for STM32F4-Disco
 *
 * LCD_CS      PC2
 * LCD_SCK     PF7
 * LCD_DC      PD13
 * LCD_MOSI    PF9
 * LCD_SPI     SPI5
 * LCD_WIDTH   240
 * LCD_HEIGHT  320
 */
void
lcd_spi_init(void)
{

	/* Enable the GPIO ports whose pins we are using */
	rcc_periph_clock_enable(RCC_SPI5);
	rcc_periph_clock_enable(RCC_GPIOF);
	rcc_periph_clock_enable(RCC_GPIOC);
	rcc_periph_clock_enable(RCC_GPIOD);

	gpio_mode_setup(GPIOF, GPIO_MODE_AF, GPIO_PUPD_NONE,
			GPIO7 | GPIO8 | GPIO9);
	gpio_set_af(GPIOF, GPIO_AF5, GPIO7 | GPIO8 | GPIO9);
	gpio_set_output_options(GPIOF, GPIO_OTYPE_PP, GPIO_OSPEED_25MHZ,
				GPIO7 | GPIO9);

	gpio_mode_setup(GPIOD, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO13);

	/* Chip select line */
	gpio_mode_setup(GPIOC, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO1 | GPIO2);
	gpio_set(GPIOC, GPIO1);

	/*
	 * Set up the GPIO lines for the SPI port and
	 * control lines on the display.
	 */
	//rcc_periph_clock_enable(RCC_GPIOC | RCC_GPIOD | RCC_GPIOF);

	//gpio_mode_setup(GPIOC, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO2);
	//gpio_mode_setup(GPIOD, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO13);

	//gpio_mode_setup(GPIOF, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO7 | GPIO9);
	//gpio_set_af(GPIOF, GPIO_AF5, GPIO7 | GPIO9);

	cur_frame = (uint16_t *)(SDRAM_BASE_ADDRESS);
	display_frame = cur_frame + (LCD_WIDTH * LCD_HEIGHT);

	//rcc_periph_clock_enable(RCC_SPI5);
	//spi_init_master(LCD_SPI, SPI_CR1_BAUDRATE_FPCLK_DIV_4,
	//				SPI_CR1_CPOL_CLK_TO_0_WHEN_IDLE,
	//				SPI_CR1_CPHA_CLK_TRANSITION_1,
	//				SPI_CR1_DFF_8BIT,
	//				SPI_CR1_MSBFIRST);
	//spi_enable_ss_output(LCD_SPI);
	//spi_enable(LCD_SPI);

	//put_status("\nBefore init: ");
	//SPI_CR2(SPI5) |= SPI_CR2_SSOE;
	//SPI_CR1(SPI5) = cr_tmp;
	spi_init();
	//put_status("After init: ");

	/* Set up the display */
	console_puts("Initialize the display.\n");
	initialize_display(initialization);

	/* create a test image */
	//console_puts("Generating Test Image\n");
	test_image();

	/* display it on the LCD */
	//console_puts("And ... voila\n");
	lcd_show_frame();
}

/*
 * int len = print_decimal(int value)
 *
 * Very simple routine to print an integer as a decimal
 * number on the console.
 */


/*
 * int print_hex(int value)
 *
 * Very simple routine for printing out hex constants.
 */
static int print_hex(int v)
{
	int		ndx = 0;
	char	buf[10];
	int		len;

	buf[ndx++] = '\000';
	do {
		char	c = v & 0xf;
		buf[ndx++] = (c > 9) ? '7' + c : '0' + c;
		v = (v >> 4) & 0x0fffffff;
	} while (v != 0);
	ndx--;
	console_puts("0x");
	len = 2;
	while (buf[ndx] != '\000') {
		console_putc(buf[ndx--]);
		len++;
	}
	return len; /* number of characters printed */
}

//--------------------------------------------------------------------------
//lcd-spi.c
//--------------------------------------------------------------------------


//--------------------------------------------------------------------------
//gfx.c
//--------------------------------------------------------------------------

#ifndef _GFX_H
#define _GFX_H

#define swap(a, b) { int16_t t = a; a = b; b = t; }

#define pgm_read_byte(addr) (*(const unsigned char *)(addr))

//struct gfx_state __gfx_state;

#define GFX_WIDTH   320
#define GFX_HEIGHT  240

struct gfx_state {
	int16_t _width, _height, cursor_x, cursor_y;
	uint16_t textcolor, textbgcolor;
	uint8_t textsize, rotation;
	uint8_t wrap;
	void (*drawpixel)(int, int, uint16_t);
};

struct gfx_state __gfx_state;

#define GFX_COLOR_WHITE          0xFFFF
#define GFX_COLOR_BLACK          0x0000
#define GFX_COLOR_GREY           0xF7DE
#define GFX_COLOR_BLUE           0x001F
#define GFX_COLOR_BLUE2          0x051F
#define GFX_COLOR_RED            0xF800
#define GFX_COLOR_MAGENTA        0xF81F
#define GFX_COLOR_GREEN          0x07E0
#define GFX_COLOR_CYAN           0x7FFF
#define GFX_COLOR_YELLOW         0xFFE0

#endif /* _ADAFRUIT_GFX_H */


void
gfx_drawPixel(int x, int y, uint16_t color)
{
	if ((x < 0) || (x >= __gfx_state._width) ||
	    (y < 0) || (y >= __gfx_state._height)) {
		return; /* off screen so don't draw it */
	}
	(__gfx_state.drawpixel)(x, y, color);
}

/* Bresenham's algorithm - thx wikpedia */
void gfx_drawLine(int16_t x0, int16_t y0,
			    int16_t x1, int16_t y1,
			    uint16_t color)
{
	int16_t steep = abs(y1 - y0) > abs(x1 - x0);
	if (steep) {
		swap(x0, y0);
		swap(x1, y1);
	}

	if (x0 > x1) {
		swap(x0, x1);
		swap(y0, y1);
	}

	int16_t dx, dy;
	dx = x1 - x0;
	dy = abs(y1 - y0);

	int16_t err = dx / 2;
	int16_t ystep;

	if (y0 < y1) {
		ystep = 1;
	} else {
		ystep = -1;
	}

	for (; x0 <= x1; x0++) {
		if (steep) {
			gfx_drawPixel(y0, x0, color);
		} else {
			gfx_drawPixel(x0, y0, color);
		}
		err -= dy;
		if (err < 0) {
			y0 += ystep;
			err += dx;
		}
	}
}

void gfx_drawFastHLine(int16_t x, int16_t y,
		       int16_t w, uint16_t color)
{
	/* Update in subclasses if desired! */
	gfx_drawLine(x, y, x + w - 1, y, color);
}

#define true 1

void
gfx_init(void (*pixel_func)(int, int, uint16_t), int width, int height)
{
	__gfx_state._width    = width;
	__gfx_state._height   = height;
	__gfx_state.rotation  = 0;
	__gfx_state.cursor_y  = __gfx_state.cursor_x    = 0;
	__gfx_state.textsize  = 1;
	__gfx_state.textcolor = 0;
	__gfx_state.textbgcolor = 0xFFFF;
	__gfx_state.wrap      = true;
	__gfx_state.drawpixel = pixel_func;
}

/* Draw a circle outline */
void gfx_drawCircle(int16_t x0, int16_t y0, int16_t r,
		    uint16_t color)
{
	int16_t f = 1 - r;
	int16_t ddF_x = 1;
	int16_t ddF_y = -2 * r;
	int16_t x = 0;
	int16_t y = r;

	gfx_drawPixel(x0  , y0+r, color);
	gfx_drawPixel(x0  , y0-r, color);
	gfx_drawPixel(x0+r, y0  , color);
	gfx_drawPixel(x0-r, y0  , color);

	while (x < y) {
		if (f >= 0) {
			y--;
			ddF_y += 2;
			f += ddF_y;
		}
		x++;
		ddF_x += 2;
		f += ddF_x;

		gfx_drawPixel(x0 + x, y0 + y, color);
		gfx_drawPixel(x0 - x, y0 + y, color);
		gfx_drawPixel(x0 + x, y0 - y, color);
		gfx_drawPixel(x0 - x, y0 - y, color);
		gfx_drawPixel(x0 + y, y0 + x, color);
		gfx_drawPixel(x0 - y, y0 + x, color);
		gfx_drawPixel(x0 + y, y0 - x, color);
		gfx_drawPixel(x0 - y, y0 - x, color);
	}
}

void gfx_drawCircleHelper(int16_t x0, int16_t y0,
			  int16_t r, uint8_t cornername, uint16_t color)
{
	int16_t f     = 1 - r;
	int16_t ddF_x = 1;
	int16_t ddF_y = -2 * r;
	int16_t x     = 0;
	int16_t y     = r;

	while (x < y) {
		if (f >= 0) {
			y--;
			ddF_y += 2;
			f     += ddF_y;
		}
		x++;
		ddF_x += 2;
		f     += ddF_x;
		if (cornername & 0x4) {
			gfx_drawPixel(x0 + x, y0 + y, color);
			gfx_drawPixel(x0 + y, y0 + x, color);
		}
		if (cornername & 0x2) {
			gfx_drawPixel(x0 + x, y0 - y, color);
			gfx_drawPixel(x0 + y, y0 - x, color);
		}
		if (cornername & 0x8) {
			gfx_drawPixel(x0 - y, y0 + x, color);
			gfx_drawPixel(x0 - x, y0 + y, color);
		}
		if (cornername & 0x1) {
			gfx_drawPixel(x0 - y, y0 - x, color);
			gfx_drawPixel(x0 - x, y0 - y, color);
		}
	}
}

void gfx_drawFastVLine(int16_t x, int16_t y,
		       int16_t h, uint16_t color)
{
	/* Update in subclasses if desired! */
	gfx_drawLine(x, y, x, y + h - 1, color);
}

/* Used to do circles and roundrects */
void gfx_fillCircleHelper(int16_t x0, int16_t y0, int16_t r,
			  uint8_t cornername, int16_t delta, uint16_t color)
{
	int16_t f     = 1 - r;
	int16_t ddF_x = 1;
	int16_t ddF_y = -2 * r;
	int16_t x     = 0;
	int16_t y     = r;

	while (x < y) {
		if (f >= 0) {
			y--;
			ddF_y += 2;
			f     += ddF_y;
		}
		x++;
		ddF_x += 2;
		f     += ddF_x;

		if (cornername & 0x1) {
			gfx_drawFastVLine(x0+x, y0-y, 2*y+1+delta, color);
			gfx_drawFastVLine(x0+y, y0-x, 2*x+1+delta, color);
		}
		if (cornername & 0x2) {
			gfx_drawFastVLine(x0-x, y0-y, 2*y+1+delta, color);
			gfx_drawFastVLine(x0-y, y0-x, 2*x+1+delta, color);
		}
	}
}

void gfx_fillCircle(int16_t x0, int16_t y0, int16_t r,
		    uint16_t color)
{
	gfx_drawFastVLine(x0, y0 - r, 2*r+1, color);
	gfx_fillCircleHelper(x0, y0, r, 3, 0, color);
}


/* Draw a rectangle */
void gfx_drawRect(int16_t x, int16_t y,
		  int16_t w, int16_t h,
		  uint16_t color)
{
	gfx_drawFastHLine(x, y, w, color);
	gfx_drawFastHLine(x, y + h - 1, w, color);
	gfx_drawFastVLine(x, y, h, color);
	gfx_drawFastVLine(x + w - 1, y, h, color);
}


void gfx_fillRect(int16_t x, int16_t y, int16_t w, int16_t h,
		  uint16_t color)
{
	/* Update in subclasses if desired! */
	int16_t i;
	for (i = x; i < x + w; i++) {
		gfx_drawFastVLine(i, y, h, color);
	}
}

void gfx_fillScreen(uint16_t color)
{
	gfx_fillRect(0, 0, __gfx_state._width, __gfx_state._height, color);
}

/* Draw a rounded rectangle */
void gfx_drawRoundRect(int16_t x, int16_t y, int16_t w,
		       int16_t h, int16_t r, uint16_t color)
{
	/* smarter version */
	gfx_drawFastHLine(x + r    , y        , w - 2 * r, color); /* Top */
	gfx_drawFastHLine(x + r    , y + h - 1, w - 2 * r, color); /* Bottom */
	gfx_drawFastVLine(x        , y + r    , h - 2 * r, color); /* Left */
	gfx_drawFastVLine(x + w - 1, y + r    , h - 2 * r, color); /* Right */
	/* draw four corners */
	gfx_drawCircleHelper(x + r        , y + r        , r, 1, color);
	gfx_drawCircleHelper(x + w - r - 1, y + r        , r, 2, color);
	gfx_drawCircleHelper(x + w - r - 1, y + h - r - 1, r, 4, color);
	gfx_drawCircleHelper(x + r        , y + h - r - 1, r, 8, color);
}

/* Fill a rounded rectangle */
void gfx_fillRoundRect(int16_t x, int16_t y, int16_t w,
		       int16_t h, int16_t r, uint16_t color) {
	/* smarter version */
	gfx_fillRect(x + r, y, w - 2 * r, h, color);

	/* draw four corners */
	gfx_fillCircleHelper(x + w - r - 1, y + r, r, 1, h - 2 * r - 1, color);
	gfx_fillCircleHelper(x + r        , y + r, r, 2, h - 2 * r - 1, color);
}

/* Draw a triangle */
void gfx_drawTriangle(int16_t x0, int16_t y0,
		      int16_t x1, int16_t y1,
		      int16_t x2, int16_t y2, uint16_t color)
{
	gfx_drawLine(x0, y0, x1, y1, color);
	gfx_drawLine(x1, y1, x2, y2, color);
	gfx_drawLine(x2, y2, x0, y0, color);
}

/* Fill a triangle */
void gfx_fillTriangle(int16_t x0, int16_t y0,
		      int16_t x1, int16_t y1,
		      int16_t x2, int16_t y2, uint16_t color)
{
	int16_t a, b, y, last;

	/* Sort coordinates by Y order (y2 >= y1 >= y0) */
	if (y0 > y1) {
		swap(y0, y1); swap(x0, x1);
	}
	if (y1 > y2) {
		swap(y2, y1); swap(x2, x1);
	}
	if (y0 > y1) {
		swap(y0, y1); swap(x0, x1);
	}

	/* Handle awkward all-on-same-line case as its own thing */
	if (y0 == y2) {
		a = b = x0;
		if (x1 < a) {
			a = x1;
		} else if (x1 > b) {
			b = x1;
		}
		if (x2 < a) {
			a = x2;
		} else if (x2 > b) {
			b = x2;
		}
		gfx_drawFastHLine(a, y0, b - a + 1, color);
		return;
	}

	int16_t
	dx01 = x1 - x0,
	dy01 = y1 - y0,
	dx02 = x2 - x0,
	dy02 = y2 - y0,
	dx12 = x2 - x1,
	dy12 = y2 - y1,
	sa   = 0,
	sb   = 0;

	/* For upper part of triangle, find scanline crossings for segments
	 * 0-1 and 0-2.  If y1=y2 (flat-bottomed triangle), the scanline y1
	 * is included here (and second loop will be skipped, avoiding a /0
	 * error there), otherwise scanline y1 is skipped here and handled
	 * in the second loop...which also avoids a /0 error here if y0=y1
	 * (flat-topped triangle).
	 */
	if (y1 == y2) {
		last = y1;   /* Include y1 scanline */
	} else {
		last = y1 - 1; /* Skip it */
	}

	for (y = y0; y <= last; y++) {
		a   = x0 + sa / dy01;
		b   = x0 + sb / dy02;
		sa += dx01;
		sb += dx02;
		/* longhand:
		   a = x0 + (x1 - x0) * (y - y0) / (y1 - y0);
		   b = x0 + (x2 - x0) * (y - y0) / (y2 - y0);
		   */
		if (a > b) {
			swap(a, b);
		}
		gfx_drawFastHLine(a, y, b - a + 1, color);
	}

	/* For lower part of triangle, find scanline crossings for segments
	 * 0-2 and 1-2.  This loop is skipped if y1=y2.
	 */
	sa = dx12 * (y - y1);
	sb = dx02 * (y - y0);
	for (; y <= y2; y++) {
		a   = x1 + sa / dy12;
		b   = x0 + sb / dy02;
		sa += dx12;
		sb += dx02;
		/* longhand:
		   a = x1 + (x2 - x1) * (y - y1) / (y2 - y1);
		   b = x0 + (x2 - x0) * (y - y0) / (y2 - y0);
		   */
		if (a > b) {
			swap(a, b);
		}
		gfx_drawFastHLine(a, y, b - a + 1, color);
	}
}

void gfx_drawBitmap(int16_t x, int16_t y,
		    const uint8_t *bitmap, int16_t w, int16_t h,
		    uint16_t color)
{
	int16_t i, j, byteWidth = (w + 7) / 8;

	for (j = 0; j < h; j++) {
		for (i = 0; i < w; i++) {
			if (pgm_read_byte(bitmap + j * byteWidth + i / 8) &
					 (128 >> (i & 7))) {
				gfx_drawPixel(x + i, y + j, color);
			}
		}
	}
}

/* Draw a character */
void gfx_drawChar(int16_t x, int16_t y, unsigned char c,
		  uint16_t color, uint16_t bg, uint8_t size)
{
	int8_t i, j, line;
	int8_t descender;
	unsigned const char *glyph;

	glyph = &mcm_font[(c & 0x7f) * 9];

	descender = (*glyph & 0x80) != 0;

	for (i = 0; i < 12; i++) {
		line = 0x00;
		if (descender) {
			if (i > 2) {
				line = *(glyph + (i - 3));
			}
		} else {
			if (i < 9) {
				line = *(glyph + i);
			}
		}
		line &= 0x7f;
		for (j = 0; j < 8; j++) {
			if (line & 0x80) {
				if (size == 1) /* default size */
					gfx_drawPixel(x+j, y+i, color);
				else {  /* big size */
					gfx_fillRect(x+(j*size), y+(i*size),
						     size, size, color);
				}
			} else if (bg != color) {
				if (size == 1) /* default size */
					gfx_drawPixel(x+j, y+i, bg);
				else {  /* big size */
					gfx_fillRect(x+j*size, y+i*size,
						     size, size, bg);
				}
			}
			line <<= 1;
		}
	}
}

void gfx_write(uint8_t c)
{
	if (c == '\n') {
		__gfx_state.cursor_y += __gfx_state.textsize * 12;
		__gfx_state.cursor_x  = 0;
	} else if (c == '\r') {
		/* skip em */
	} else {
		gfx_drawChar(__gfx_state.cursor_x, __gfx_state.cursor_y,
			     c, __gfx_state.textcolor, __gfx_state.textbgcolor,
			     __gfx_state.textsize);
		__gfx_state.cursor_x += __gfx_state.textsize * 8;
		if (__gfx_state.wrap &&
		    (__gfx_state.cursor_x > (__gfx_state._width -
					     __gfx_state.textsize*8))) {
			__gfx_state.cursor_y += __gfx_state.textsize * 12;
			__gfx_state.cursor_x = 0;
		}
	}
}

void gfx_puts(char *s)
{
	while (*s) {
		gfx_write(*s);
		s++;
	}
}

gfx_decimal(int num)
{
	int		ndx = 0;
	char	buf[10];
	char 	num_[10];
	int		len = 0;
	char	is_signed = 0;
	int 	cnt = 0;

	if (num < 0) {
		is_signed++;
		num = 0 - num;
	}
	buf[ndx++] = '\000';
	do {
		buf[ndx++] = (num % 10) + '0';
		num = num / 10;
	} while (num != 0);
	ndx--;
	if (is_signed != 0) {
		gfx_puts('-');
		len++;
	}
	while (buf[ndx] != '\000') {
		num_[cnt++] = buf[ndx--];
		len++;
	}
	num_[cnt] = buf[ndx];
	gfx_puts(num_);
	return len; /* number of characters printed */
}

void gfx_setCursor(int16_t x, int16_t y)
{
	__gfx_state.cursor_x = x;
	__gfx_state.cursor_y = y;
}

void gfx_setTextSize(uint8_t s)
{
	__gfx_state.textsize = (s > 0) ? s : 1;
}

void gfx_setTextColor(uint16_t c, uint16_t b)
{
	__gfx_state.textcolor   = c;
	__gfx_state.textbgcolor = b;
}

void gfx_setTextWrap(uint8_t w)
{
	__gfx_state.wrap = w;
}

uint8_t gfx_getRotation(void)
{
	return __gfx_state.rotation;
}

void gfx_setRotation(uint8_t x)
{
	__gfx_state.rotation = (x & 3);
	switch (__gfx_state.rotation) {
	case 0:
	case 2:
		__gfx_state._width  = GFX_WIDTH;
		__gfx_state._height = GFX_HEIGHT;
		break;
	case 1:
	case 3:
		__gfx_state._width  = GFX_HEIGHT;
		__gfx_state._height = GFX_WIDTH;
		break;
	}
}

/* Return the size of the display (per current rotation) */
uint16_t gfx_width(void)
{
	return __gfx_state._width;
}

uint16_t gfx_height(void)
{
	return __gfx_state._height;
}

//--------------------------------------------------------------------------
//gfx.c
//--------------------------------------------------------------------------



int main(void)
{
	uint16_t adc_value;
	int16_t vecs[3];
	int tmp, i, j;
	int count;
	float battery_voltage;
	int skip = 0;
	int battery_uni = 0;
	int battery_deci = 0;
	int uart_flag = 0;
	int alarm_flag = 0;
	int button_flag = 0;
	int button_count = 0;
	int button_count2 = 0;

	clock_setup();
	console_setup(115200);

	sdram_init();
	console_puts("SDRAM initialized\n");
	lcd_spi_init();
	console_puts("LCD initialized\n");
	adc_init();
	adc_setup();
	console_puts("ADC initialized\n");


	gfx_init(lcd_draw_pixel, 240, 320);
	gfx_fillScreen(LCD_GREY);
	gfx_fillRoundRect(10, 10, 220, 300, 5, LCD_WHITE);
	//gfx_drawRoundRect(10, 10, 220, 300, 5, LCD_GREEN);
	gfx_setTextSize(2);
	gfx_setCursor(15, 25);
	gfx_puts("X:");
	//gfx_setCursor(35, 49);
	//gfx_puts("45");
	gfx_setCursor(15, 95);
	gfx_puts("Y:");
	gfx_setCursor(15, 165);
	gfx_puts("Z:");
	gfx_setCursor(15, 235);
	gfx_puts("Bateria:");
	lcd_show_frame();


	//Configure gyroscope
	write_reg(0x20, 0xcf);  /* Normal mode */
	write_reg(0x21, 0x07);  /* standard filters */
	write_reg(0x23, 0xb0);  /* 250 dps */


	count = 0;
	while (1) {
		adc_value = read_adc_naiive(0);
		tmp = read_xyz(vecs);
		if (uart_flag == 1) {
			if (button_count2 == 10) {
				gpio_toggle(LED_DISCO_RED_PORT, LED_DISCO_RED_PIN);
			}
			for (i = 0; i < 3; i++) {
				print_decimal(vecs[i]);
				console_putc(',');
			}
			//print_decimal(adc_value);
			//console_putc(',');
			print_decimal(battery_voltage);
			console_putc('\n');
		} else {
			gpio_clear(LED_DISCO_RED_PORT, LED_DISCO_RED_PIN);
		}
		if (alarm_flag == 1) {
			gpio_set(LED_DISCO_GREEN_PORT, LED_DISCO_GREEN_PIN);
		} else {
			gpio_clear(LED_DISCO_GREEN_PORT, LED_DISCO_GREEN_PIN);
		}

		battery_voltage = conversion_factor*100*adc_value;

		if (conversion_factor*adc_value <= 7)
		{
			alarm_flag = 1;
		} else {
			alarm_flag = 0;
		}

		if (skip++ == 100) {
			skip=0;
			battery_uni = battery_voltage/100;
			battery_deci = (battery_voltage-(battery_uni*100))/10;
			gfx_fillScreen(LCD_GREY);
			gfx_fillRoundRect(10, 10, 220, 300, 5, LCD_WHITE);
			//gfx_drawRoundRect(10, 10, 220, 300, 5, LCD_GREEN);
			gfx_setTextSize(2);
			gfx_setCursor(15, 25);
			gfx_puts("X:");
			gfx_setCursor(35, 49);
			gfx_decimal(vecs[0]);
			gfx_setCursor(15, 95);
			gfx_puts("Y:");
			gfx_setCursor(35, 119);
			gfx_decimal(vecs[1]);
			gfx_setCursor(15, 165);
			gfx_puts("Z:");
			gfx_setCursor(35, 189);
			gfx_decimal(vecs[2]);
			gfx_setCursor(15, 235);
			gfx_puts("Bateria:");
			gfx_setCursor(35, 259);
			gfx_decimal(battery_uni);
			gfx_setCursor(55, 259);
			gfx_puts(".");
			gfx_setCursor(65, 259);
			gfx_decimal(battery_uni);
			gfx_setCursor(85, 259);
			gfx_puts("V");
			if (alarm_flag == 1)
			{
				gfx_fillCircle(117, 267, 10, LCD_RED);
			} else {
				gfx_fillCircle(117, 267, 10, LCD_GREEN);
			}
			if (uart_flag == 1)
			{
				gfx_fillCircle(157, 267, 10, LCD_GREEN);
			} else {
				gfx_fillCircle(157, 267, 10, LCD_RED);
			}
			
			lcd_show_frame();
		}

		/* LED on/off */
		//gpio_toggle(LED_DISCO_GREEN_PORT, LED_DISCO_GREEN_PIN);
		//gpio_toggle(LED_DISCO_GREEN_PORT, LED_DISCO_RED_PIN);
		//msleep(1000);

		if (gpio_get(GPIOA, GPIO1)) {
			if (button_flag==0) {
				uart_flag=uart_flag==1 ? 0 : 1;
				button_flag = 1;
				button_count = 0;
			}
		}

		for (j = 0; j < 1250000; j++) { /* Wait a bit. 25000000*/
			__asm__("NOP");
		}
		if (button_count2 == 10) {
			button_count2=0;
		}
		button_count2++;
		if (button_flag == 1) {
			button_count++;
		}
		if (button_count==5) {
			button_flag = 0;
		}

	}

}

