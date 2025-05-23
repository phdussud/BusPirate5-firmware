#include <stdio.h>
#include "pico/stdlib.h"
#include "pirate.h"
#include "platform/bpi5-rev8.h"

//TODO: move all this nonsense to the system config
uint16_t hw_adc_raw[HW_ADC_COUNT];
uint32_t hw_adc_voltage[HW_ADC_COUNT];
uint32_t hw_adc_avgsum_voltage[HW_ADC_COUNT];

const char hw_pin_label_ordered[][5] = {
    "Vout",
    "IO0",
    "IO1",
    "IO2",
    "IO3",
    "IO4",
    "IO5",
    "IO6",
    "IO7",
    "GND"
};
const uint32_t hw_pin_label_ordered_color[][2] = {
    {BP_COLOR_FULLBLACK, BP_COLOR_RED},
    {BP_COLOR_FULLBLACK, BP_COLOR_ORANGE},
    {BP_COLOR_FULLBLACK, BP_COLOR_YELLOW},
    {BP_COLOR_FULLBLACK, BP_COLOR_GREEN},
    {BP_COLOR_FULLBLACK, BP_COLOR_BLUE},
    {BP_COLOR_FULLBLACK, BP_COLOR_PURPLE},
    {BP_COLOR_FULLBLACK, BP_COLOR_BROWN},
    {BP_COLOR_FULLBLACK, BP_COLOR_GREY},
    {BP_COLOR_FULLBLACK, BP_COLOR_WHITE},
    {BP_COLOR_FULLWHITE, BP_COLOR_FULLBLACK}
};
