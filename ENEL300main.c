/*
 * File:   speaker.c
 * Author: brightonmckibbon
 *
 * Created on March 20, 2023, 4:37 PM
 */


#include <avr/io.h>



int main(void) {
    
    //unlocking protected registers 
    CCP = 0xd8;
    
    //internal clock set to 8MH
    CLKCTRL.OSCHFCTRLA = 0b00010100;
    
    //making sure that main clock is not undergoing a switch. 
    //if has a switch stay in loop until the main clock stabilizes again.
    while( CLKCTRL.MCLKSTATUS & 0b00000001 );
    
    
    //SPEAKER
    
    //set VDD (5V) as reference voltage + ref voltage always on
    VREF.DAC0REF = 0b10000101;
    
    //writing 1 to outen pin to enable output (on PD6) + DAC disabled
    DAC0.CTRLA = 0b01000000;
    
    
    //enable and set 0.128ms counter
    TCA0.SINGLE.CTRLA = 0b00001111;

    //counts a total of 350ms
    unsigned int timer_threshold = 0.35 / 0.000128;

    //initializing sample values for wave
    unsigned int sample = 0;
    //variables to track which voltage thresholds have been passed
    unsigned int track_1 = 0, track_3 = 0, track_5 = 0;
    
    //END SPEAKER
    
    
    //ADC
    
    //Enabling interrupts
    SREG = 0b10000000;
    
    //Set reference voltage to VDD + reference voltage always on
    VREF.ADC0REF = 0b10000101;
    
    //Divide clock by 2 (smallest option for fastest conversions)
    //0.25us per conversion
    ADC0.CTRLC = 0x00;
    
    //Single ended mode (only need + values), 12 bit resolution, free-running mode
    ADC0.CTRLA = 0b00000011;
    
    //ADC interrupt when result is ready 
    ADC0.INTCTRL = 0b00000001;
    
    //Start first conversion.
    ADC0.COMMAND = 0x01;
    
    //Set PD2 as input pin for ADC
    ADC0.MUXPOS = 0x02;

    //Set PD2 to only input pin
    PORTD.DIRCLR = 0b00000100;
    
    //END ADC
    
    
    //Auto for filter
    PORTC.DIRSET= 0b00000001;

    
    
    while (1) {
        unsigned int count = 0;
        while (TCA0.SINGLE.CNT <= timer_threshold);
        


        //for sensor reading v >= 5
        if ((ADC0.RES >= 4095) & (track_5 != 1)) {
            
            //beep 3 times
            count = 0;
            while (count <= 3) {
                
                beep(timer_threshold, sample); 
                
                count++;
            }
            PORTC.OUT |= 0b00000001;    //auto turn on filter
            
            track_5 = 1; //set to 1 so that only beeps three times, not 3 times every while(1) loop
            track_3 = 1;
            track_1 = 1;
            
            //clear RESRDY flag
            ADC0.INTFLAGS = 0b00000001;
        }       
        
        //for sensor readings >= 3
        else if ((ADC0.RES >= 2457.6) & (track_3 != 1)) {
            count = 0;
            while (count <= 2) {
              
                beep(timer_threshold, sample);
                count++;
            }
            PORTC.OUT |= 0b00000001;    //auto turn on filter
            track_3 = 1;
            track_1 = 1;
            
            //clear RESRDY flag
            ADC0.INTFLAGS = 0b00000001;

        }
        

        
        //for sensor readings >=1
        //819.2 = 1V
        else if ((ADC0.RES >= 819.2) & (track_1 != 1)) {    
            while (count <= 1) {
                beep(timer_threshold, sample);
                count++;
            }
            track_1 = 1;
            
            //clear RESRDY flag
            ADC0.INTFLAGS = 0b00000001;
            
        }
        
        //reset track values after passing back below boundaries.
        //values lower than thresholds above to allow for noise
        
        if ((ADC0.RES <= 800) & (track_1 == 1)) 
            track_1 = 0;
        
        if ((ADC0.RES <= 2400) & (track_3 == 1))
            track_3 = 0;
       
        if ((ADC0.RES <= 4000) & (track_5 == 1)) 
            track_5 = 0;
    }
}

