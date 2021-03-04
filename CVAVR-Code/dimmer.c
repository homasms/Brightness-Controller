/*
 * dimmer.c
 *
 * Created: 1/3/2021 12:43:42 PM
 * Author: ASA
 */

#include <io.h>
#include <delay.h>
#include <Mega32.h>
#include <stdlib.h>
#include <lcd.h>
#asm
   .equ __lcd_port=0x18
#endasm
#define ADC_VREF_TYPE 0xC0  //Volt.Ref ==> 2.56v
unsigned minute,hour,second, fifthSecond;
int timeOfOpeningCurtain = 0; // the time that curtain gets open

void darkDay(float);
void lightDay(float);
void controlLED(float);

float read_adc(unsigned char adc_input)
{
    //int result;
    ADMUX=adc_input | (ADC_VREF_TYPE & 0xff);
    delay_us(10);   // Delay nseeded for the stabilization of the ADC input voltage
    ADCSRA|=0x40; // Start the AD conversion
    while ((ADCSRA & 0x10)==0); // Wait for the AD conversion to complete
    ADCSRA|=0x10;    
    //result = ADCW; 
    return ADCW;
}

void main(void)
{
    char buffer[32];
    float T;     
    
    lcd_init(16);
    hour = 10;
    DDRC = (1<<0) | (1<<1);
    DDRD = (1<<0);  
    DDRA = 0<<0; 
    ADMUX=ADC_VREF_TYPE & 0xff;
    ADCSRA=0x85;
    
    // timer0
    TCCR0=0x03;
    TCNT0=6;
    TIMSK=0x01;
    #asm("sei");
    
while (1)
    {
    T=read_adc(0);
    T=(T*2.56)/1024; 
    lcd_gotoxy(1,0);ftoa(second,2,buffer);lcd_puts(buffer);     
    lcd_gotoxy(0,1);ftoa(minute,2,buffer);lcd_puts(buffer);
    //delay_ms(100);
    //lcd_clear();   
    if(T < 0.45 ){ 
        if(hour > 7 & hour < 18){
            darkDay(T);
        }              
        else  
            //PORTC.0 = 1;
            controlLED(T);
    }
    if(T > 0.45){
        if(hour > 7 & hour < 18)
            lightDay(T);
                
        else            
            //PORTC.0 = 0;
            controlLED(T);
        }
    }
}

void controlLED(float T){
    int onTime, offTime;
    onTime = (5 - T);  
    offTime = (T)/ 100;
    PORTC.0 = 1;
    delay_ms(onTime);
    PORTC.0 = 0;
    delay_ms(offTime);
}

void openCurtain(float T, float neededTimeForOpenCurtain){
    int onTime; 
    int remainTimeForOpeningCurtain = neededTimeForOpenCurtain - timeOfOpeningCurtain;
    // stop opening curtain when that is completely open
    if(10 < remainTimeForOpeningCurtain)
        onTime = 10 - T + T;     
    else
        onTime = remainTimeForOpeningCurtain;
    PORTC.1 = 1;
    delay_ms(onTime);
    PORTC.1 = 0;
    delay_ms(onTime); 
    timeOfOpeningCurtain += onTime;
}

void closeCurtain(float T){
    int onTime; 
    // if curtain is not opened at all, dont try to close it
    if(timeOfOpeningCurtain > 0){
        if(timeOfOpeningCurtain > 10)
            onTime = 10 - T + T;
        else
            onTime = timeOfOpeningCurtain;
        PORTD.0 = 1;
        delay_ms(onTime);
        PORTD.0 = 0; 
        delay_ms(onTime);
        timeOfOpeningCurtain -= onTime;  
    }
}

// the light is low, on day
void darkDay(float T){
    float neededTimeForOpenCurtain = 2000; 
    // curtain is compeletly open
    if(timeOfOpeningCurtain >= neededTimeForOpenCurtain)
        controlLED(T);               
    // curtain is not completely open
    else
        openCurtain(T, neededTimeForOpenCurtain);
}

// the light is too much, on day
void lightDay(float T){
    // if LED is on, turn it off
    if(PORTC.0 == 1)           
        PORTC.0 = 0;           
    // if LED is off, close the curtain
    else
        closeCurtain(T);
}


interrupt [TIM0_OVF] void timer0_ovf_isr(void)
{
    
    TCNT0=6;
    
    if(fifthSecond == 500){
        fifthSecond = 0;    
        if(second==59){
            second=0;
            if(minute==59){
                minute=0;
                if(hour==23)
                    hour=0;
                else
                    hour++;
            }
            else
                minute++;
        }
        else
            second ++;
    }
    else
        fifthSecond++;
}


