
#include <pico/stdlib.h>
#include <string.h>
#include "hardware/clocks.h"
#include "pirate.h"
#include "queue.h"
#include "usb_rx.h"
#include "usb_tx.h"
#include "system_config.h"
//#include "bytecode.h" //needed because modes.h has some functions that use it TODO: move all the opt args and bytecode stuff to a single helper file
//#include "opt_args.h" //needed for same reason as bytecode and needs same fix
//#include "modes.h"
// #include "mode/binio.h"
//#include "pirate/psu.h"
// #include "binio_helpers.h"
#include "binmode/binmodes.h"
#include "binmode/logicanalyzer.h"
#include "binmode/binio.h"

#include "tusb.h"

#define CDC_INTF 1

static uint32_t mode_frequency=1000000;
static uint32_t mode_oversample=8;

void fala_setup(void) {
    system_config.binmode_usb_rx_queue_enable = false;
    system_config.binmode_usb_tx_queue_enable = false;
    if(!logicanalyzer_setup()) {
        printf("Logic analyzer setup error, out of memory?\r\n");
    }
}

void fala_cleanup(void) {
    logic_analyzer_cleanup();
    system_config.binmode_usb_rx_queue_enable = true;
    system_config.binmode_usb_tx_queue_enable = true;
}

const char fala_name[] = "Follow along logic analyzer";


void fala_start(void) {
    // start the logic analyzer
    // configure and arm the logic analyzer
    logic_analyzer_configure(mode_frequency*mode_oversample, DMA_BYTES_PER_CHUNK*LA_DMA_COUNT, 0x00, 0x00, false);
    logic_analyzer_arm(false);
}

void fala_stop(void) {
    // stop the logic analyzer
    // but keeps data available for dump
    logic_analyser_done();
}

void fala_reset(void) {
    // reset the logic analyzer
    // logic_analyzer_cleanup();
}

void fala_set_freq(uint32_t freq) {
    // set the sampling rate
    //store in fala struct for easy oversample adjustment
    mode_frequency = freq;
    printf("\r\nFollow Along Logic Analyzer capture: %dHz (%dx oversampling)\r\n", freq*mode_oversample, mode_oversample);
    printf("Use the 'logic' command to change capture settings.\r\n");
}

// set oversampling rate
void fala_set_oversample(uint32_t oversample_rate) {
    // set the sampling rate
    mode_oversample = oversample_rate;
}
#define MAX_UART_PKT 64
// send notification packet at end of capture
void fala_notify(void){
    logic_analyzer_reset_ptr(); //put pointer back to end of data buffer (last sample first)
    uint32_t fala_samples=logic_analyzer_get_ptr();
    // send notification packet 
    //$FALADATA;{pins};{trigger pins};{trigger mask};{edge trigger (bool)}; {capture speed in hz};{samples};{pre-samples (for trigger line)};
    if (tud_cdc_n_connected(CDC_INTF)) {
        uint8_t buf[MAX_UART_PKT];
        uint8_t len = snprintf(buf, sizeof(buf), "$FALADATA;%d;%d;%d;%c;%d;%d;%d;\n", 8, 0, 0,'N', mode_frequency*mode_oversample, fala_samples, 0);
        if (tud_cdc_n_write_available(CDC_INTF) >= sizeof(buf)) {
            tud_cdc_n_write(CDC_INTF, buf, len);
            tud_cdc_n_write_flush(CDC_INTF);
        }
    }
}

void fala_print_result(void){
    //send notification packet
    fala_notify();    
    // get samples count
    logic_analyzer_reset_ptr(); //put pointer back to end of data buffer (last sample first)
    uint32_t fala_samples=logic_analyzer_get_ptr();
    // show some info about the logic capture
    printf("Logic Analyzer: %d samples captured\r\n", fala_samples);

    //print an 8 line logic analyzer graph of the last 80 samples
    printf("Logic Analyzer Graph:\r\n");
    fala_samples = fala_samples < 80 ? fala_samples : 80;
    for(int bits=0; bits<8; bits++) {
        logic_analyzer_reset_ptr();
        uint8_t val;
        for(int i=0; i<fala_samples; i++) {
            logicanalyzer_dump(&val);
            if(val & (1<<bits)) {
                printf("-"); // high
            } else {
                printf("_"); // low
            }
        }
        printf("\r\n");
    }
}

static uint32_t fala_dump_count;

static uint fala_tx8(uint8_t* buf, uint len) {
    uint32_t i, count;
    count = fala_dump_count;
    for (i = 0; i < len && count > 0; i++, count--) {
        logicanalyzer_dump(&buf[i]);
    }
    fala_dump_count -= i;
    return i;
}

void fala_dump(void){
    // dump the buffer
    uint8_t buf[MAX_UART_PKT];

    if (tud_cdc_n_connected(CDC_INTF)) { //TODO: make co-op multitask
        while(fala_dump_count){
            if (tud_cdc_n_write_available(CDC_INTF) >= sizeof(buf)) {
                uint8_t len = fala_tx8(buf, sizeof(buf));
                tud_cdc_n_write(CDC_INTF, buf, len);
                tud_cdc_n_write_flush(CDC_INTF);
            }
        }

    }
}


void fala_service(void) {
    if (tud_cdc_n_connected(CDC_INTF)) {
        if(tud_cdc_n_available(CDC_INTF)){
            uint8_t buf[64];
            uint8_t len = tud_cdc_n_read(CDC_INTF, buf, sizeof(buf));
            if(len){
                //printf("Got %u bytes %c\r\n", len, buf[0]);
                for(uint8_t i = 0; i < len; i++){
                    switch(buf[i]){
                        case '?':
                            //printf("$FALAOK;\r\n");
                            fala_notify();
                            break;
                        case '+':
                            //dump the buffer
                            logic_analyzer_reset_ptr(); //put pointer back to end of data buffer (last sample first)
                            fala_dump_count=logic_analyzer_get_ptr();
                            fala_dump();
                            break;

                    }
                }
            }
        }

    }
}
