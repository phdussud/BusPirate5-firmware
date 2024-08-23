#include <string.h>
#include "pico/stdlib.h"
#include "pirate.h"
#include "binmode/binmodes.h"
#include "system_config.h"
#include "binmode/sump.h"
#include "binmode/dirtyproto.h"
#include "binmode/fala.h"
#include "lib/arduino-ch32v003-swio/arduino_ch32v003.h"

void binmode_null_func_voi(void) {
    return;
}

void binmode_null_func_uint32(uint32_t temp) {
    return;
}

const binmode_t binmodes[]={
    {   sump_logic_analyzer_name, 
        &sump_logic_analyzer_setup,
        &sump_logic_analyzer_service,
        &sump_logic_analyzer_cleanup, 
        &binmode_null_func_uint32,
        &binmode_null_func_voi,
        &binmode_null_func_voi,
        &binmode_null_func_voi, 
    },
    {   dirtyproto_mode_name,
        &binmode_null_func_voi,
        &dirtyproto_mode,
        &binmode_null_func_voi, 
        &binmode_null_func_uint32,
        &binmode_null_func_voi,
        &binmode_null_func_voi,
        &binmode_null_func_voi,
    },
    {   arduino_ch32v003_name,
        &binmode_null_func_voi,
        &arduino_ch32v003,
        &binmode_null_func_voi,
        &binmode_null_func_uint32,
        &binmode_null_func_voi,
        &binmode_null_func_voi,
        &binmode_null_func_voi,                   
    },
    {
        fala_name, 
        &fala_setup,
        &fala_service,
        &fala_cleanup, 
        &fala_set_freq,
        &fala_start,
        &fala_stop,
        &fala_print_result 
    },    
};

inline void binmode_setup(void){
    binmodes[system_config.binmode_select].binmode_setup();
}

inline void binmode_service(void){
    binmodes[system_config.binmode_select].binmode_service();
}

inline void binmode_cleanup(void){
    binmodes[system_config.binmode_select].binmode_cleanup();
}