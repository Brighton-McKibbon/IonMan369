/*
 * File:   Speakerfxn.c
 * Author: brightonmckibbon
 *
 * Created on March 26, 2023, 3:12 PM
 */


#include <avr/io.h>

unsigned int num_samp = 40;
    
//creating output signal, triangle wave
unsigned int signal_data[] = {13, 26, 38, 51, 64, 77, 90, 102, 115, 128, 
    141, 154, 166, 179, 192, 205, 218, 230, 243, 255, 243, 230, 218, 205, 
    192, 179, 166, 154, 141, 128, 115, 102, 90, 77, 64, 51, 38, 26, 13, 0};




void beep(unsigned int timer_threshold, unsigned int sample) {
    
    while (TCA0.SINGLE.CNT <= timer_threshold) {
        //enabling DAC output
        DAC0.CTRLA = 0b01000001;
            
        //generating waveform approx 990Hz
        DAC0.DATAH = signal_data[sample];
        sample = (sample +1) % num_samp;                              
    }
    
    TCA0.SINGLE.CNT = 0;    //reset count
            
        
    while (TCA0.SINGLE.CNT <= timer_threshold) {
        //disabling DAC output
        DAC0.CTRLA = 0b01000000;     
    }
    
    TCA0.SINGLE.CNT = 0; //reset count
}
