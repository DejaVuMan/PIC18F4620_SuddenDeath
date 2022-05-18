/*
 * File:   zad_5_chess_clock.c
 * Author: ADS
 *
 * Created on May 11, 2022, 2:11 PM
 */

// CONFIG1H
#pragma config OSC = HSPLL      // Oscillator Selection bits (HS oscillator, PLL enabled (Clock Frequency = 4 x FOSC1))
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enable bit (Fail-Safe Clock Monitor disabled)
#pragma config IESO = OFF       // Internal/External Oscillator Switchover bit (Oscillator Switchover mode disabled)

// CONFIG2L
#pragma config PWRT = ON        // Power-up Timer Enable bit (PWRT enabled)
#pragma config BOREN = OFF      // Brown-out Reset Enable bits (Brown-out Reset disabled in hardware and software)
#pragma config BORV = 3         // Brown Out Reset Voltage bits (Minimum setting)

// CONFIG2H
#pragma config WDT = OFF        // Watchdog Timer Enable bit (WDT disabled (control is placed on the SWDTEN bit))
#pragma config WDTPS = 32768    // Watchdog Timer Postscale Select bits (1:32768)

// CONFIG3H
#pragma config CCP2MX = PORTC   // CCP2 MUX bit (CCP2 input/output is multiplexed with RC1)
#pragma config PBADEN = OFF     // PORTB A/D Enable bit (PORTB<4:0> pins are configured as digital I/O on Reset)
#pragma config LPT1OSC = OFF    // Low-Power Timer1 Oscillator Enable bit (Timer1 configured for higher power operation)
#pragma config MCLRE = OFF // was ON       // MCLR Pin Enable bit (MCLR pin enabled; RE3 input pin disabled)

// CONFIG4L
#pragma config STVREN = OFF     // Stack Full/Underflow Reset Enable bit (Stack full/underflow will not cause Reset)
#pragma config LVP = OFF        // Single-Supply ICSP Enable bit (Single-Supply ICSP disabled)
#pragma config XINST = OFF      // Extended Instruction Set Enable bit (Instruction set extension and Indexed Addressing mode disabled (Legacy mode))

// CONFIG5L
#pragma config CP0 = OFF        // Code Protection bit (Block 0 (000800-003FFFh) not code-protected)
#pragma config CP1 = OFF        // Code Protection bit (Block 1 (004000-007FFFh) not code-protected)
#pragma config CP2 = OFF        // Code Protection bit (Block 2 (008000-00BFFFh) not code-protected)
#pragma config CP3 = OFF        // Code Protection bit (Block 3 (00C000-00FFFFh) not code-protected)

// CONFIG5H
#pragma config CPB = OFF        // Boot Block Code Protection bit (Boot block (000000-0007FFh) not code-protected)
#pragma config CPD = OFF        // Data EEPROM Code Protection bit (Data EEPROM not code-protected)

// CONFIG6L
#pragma config WRT0 = OFF       // Write Protection bit (Block 0 (000800-003FFFh) not write-protected)
#pragma config WRT1 = OFF       // Write Protection bit (Block 1 (004000-007FFFh) not write-protected)
#pragma config WRT2 = OFF       // Write Protection bit (Block 2 (008000-00BFFFh) not write-protected)
#pragma config WRT3 = OFF       // Write Protection bit (Block 3 (00C000-00FFFFh) not write-protected)

// CONFIG6H
#pragma config WRTC = OFF       // Configuration Register Write Protection bit (Configuration registers (300000-3000FFh) not write-protected)
#pragma config WRTB = OFF       // Boot Block Write Protection bit (Boot Block (000000-0007FFh) not write-protected)
#pragma config WRTD = OFF       // Data EEPROM Write Protection bit (Data EEPROM not write-protected)

// CONFIG7L
#pragma config EBTR0 = OFF      // Table Read Protection bit (Block 0 (000800-003FFFh) not protected from table reads executed in other blocks)
#pragma config EBTR1 = OFF      // Table Read Protection bit (Block 1 (004000-007FFFh) not protected from table reads executed in other blocks)
#pragma config EBTR2 = OFF      // Table Read Protection bit (Block 2 (008000-00BFFFh) not protected from table reads executed in other blocks)
#pragma config EBTR3 = OFF      // Table Read Protection bit (Block 3 (00C000-00FFFFh) not protected from table reads executed in other blocks)

// CONFIG7H
#pragma config EBTRB = OFF

#include <xc.h>
#include <stdbool.h>
#include "pic18f4620.h"

// R/W Goes to Ground
#define LENA  PORTEbits.RE1 // E (Enable?)
#define LDAT  PORTEbits.RE2 // RS
#define LPORT PORTD
#define AINPUTS 0xffcf

#define L_ON	0x0F
#define L_OFF	0x08
#define L_CLR	0x01
#define L_L1	0x80
#define L_L2	0xC0
#define L_CR	0x0F		
#define L_NCR	0x0C	

#define L_CFG   0x38

void delay(unsigned int ms)
{
    unsigned int i;
    unsigned char j;
    
 for (i =0; i< ms; i++)
 {
 
  for (j =0 ; j < 200; j++)
   {
      Nop();
      Nop();
      Nop();
      Nop();
      Nop();
   }
 }
}

void lcd_wr(unsigned char val)
{
  LPORT=val;
}

void lcd_cmd(unsigned char val)
{
    // E = LENA
    // RS = LDAT
	LENA=1;
        lcd_wr(val);
        LDAT=0;
        //delay(3);
        LENA=0;
        //delay(3);
	LENA=1;
}

void lcd_init(void)
{
	LENA=0;
	LDAT=0;
	delay(20);
	LENA=1;
	
	lcd_cmd(L_CFG); // 0x38
	delay(5);
	lcd_cmd(L_CFG); // 0x38
        delay(1);
	lcd_cmd(L_CFG); //Configure 0x38
	lcd_cmd(L_OFF); // 0x08
	lcd_cmd(L_ON); //Initialize with 0x0F
	lcd_cmd(L_CLR); //Clear with 0x01
	lcd_cmd(L_CFG); //Configure
    lcd_cmd(L_L1);
}

void lcd_dat(unsigned char val)
{
	LENA=1;
        lcd_wr(val);
        LDAT=1;
        //delay(3);
        LENA=0;
        //delay(3);
	LENA=1;
}

void lcd_str(const char* str)
{
 unsigned char i=0;
  
 while(str[i] != 0 )
 {
   lcd_dat(str[i]);
   i++;
 }  
}

unsigned int adc(unsigned char kanal)
{
    switch(kanal)
    {
        case 0: ADCON0=0x01; break; //P1
        case 1: ADCON0=0x05; break; //P2
        case 2: ADCON0=0x09; break; 
    }
    
    ADCON0bits.GO=1;
    while(ADCON0bits.GO == 1);

   return ((((unsigned int)ADRESH)<<2)|(ADRESL>>6));
}

bool win_display(bool is_player1)
{
    unsigned int i;
    bool line_flip = true;
    
    if(is_player1)
    {
        for(i=0; i < 15; i++)
        {
            lcd_cmd(L_CLR);
            if(line_flip)
            {
                
                lcd_cmd(L_L1);
                lcd_str("P1      By      ");
                lcd_cmd(L_L2);
                lcd_str("    Win    Time!");
                line_flip = !line_flip;
            }
            else
            {
                lcd_cmd(L_L1);
                lcd_str("    Win    Time!");  
                lcd_cmd(L_L2);
                lcd_str("P1      By      ");  
                line_flip = !line_flip;
            }
            delay(1000);
        }
        return false;
    }
    for(i=0; i < 15; i++)
    {
        lcd_cmd(L_CLR);
        if(line_flip)
        {

            lcd_cmd(L_L1);
            lcd_str("P2      By      ");
            lcd_cmd(L_L2);
            lcd_str("    Win    Time!"); 
            line_flip = !line_flip;
        }
        else
        {
            lcd_cmd(L_L1);
            lcd_str("    Win    Time!");  
            lcd_cmd(L_L2);
            lcd_str("P2      By      ");   
            line_flip = !line_flip;
        }
        delay(1000);
    }
    return false;
}

void time_display_update(char* array, unsigned int p_1_t, unsigned int p_2_t)
{
    unsigned int p1_minutes = p_1_t / 60;
    unsigned int p1_seconds = p_1_t - (p1_minutes * 60);

    unsigned int p2_minutes = p_2_t / 60;
    unsigned int p2_seconds = p_2_t - (p2_minutes * 60);
    
    array[3] = p1_minutes - (p1_minutes / 10 * 10) + '0';
    array[4] = ':';
    array[5] = p1_seconds/10 + '0';
    array[6] = p1_seconds - (p1_seconds / 10 * 10) + '0';

    array[9] = p2_minutes - (p2_minutes / 10 * 10) + '0';
    array[10] = ':';
    array[11] = p2_seconds/10 + '0';
    array[12] = p2_seconds - (p2_seconds / 10 * 10) + '0';
}

void main(void) {
    
    ADCON0=0x01;
    ADCON1=0x0B;
    ADCON2=0x01;
    
    TRISA=0xC3;
    TRISB=0x3F;   
    TRISC=0x01;
    TRISD=0x00;
    TRISE=0x00;
    
    lcd_init(); 
    // Initialize LCD Screen with ~1s delay to account for init time,
    // and set mode to 0x38 (8bit, 2x16)
    lcd_cmd(L_CLR); // Clear LCD Screen using 0x01 command 
    
    unsigned int player_1_time = 300;
    unsigned int player_2_time = 300; // 300s 5M, 180s 3M, 60s 1M
    unsigned int mode_counter = 0; // 5, 3, 1 = 0,1,2
    
    bool is_player_1 = false; // boolean type provided by stdbool.h
    bool is_active = false;
    bool dot_side = false;
    
    unsigned int tmp = 0;
    unsigned int prev = 0;
        
    delay(1000);
    lcd_cmd(L_CLR);
    lcd_cmd(L_L1); //Ustawienie karetki w pierwszej linii
    lcd_str(" Use Potent. P2  "); //napis
    lcd_cmd(L_L2);
    lcd_str("  To set time  ");
    delay(3000);
    lcd_cmd(L_CLR);
    
    while(1)
    {
        delay(1000); // 1s delay
        tmp = ((unsigned int)adc(1) / 10); // get value from P2 potentiometer
        
        if(!is_active)
        {
            if(tmp > 66 && tmp != prev)
            {
               prev = tmp;
               player_1_time = 60;
               player_2_time = 60;
               lcd_cmd(L_CLR);
               lcd_cmd(L_L1); //Ustawienie karetki w pierwszej linii
               lcd_str("1 Minute time"); //napis
               lcd_cmd(L_L2);
               lcd_str("mode selected");
               delay(2000);
            }
            else if(tmp > 33 && tmp != prev)
            {
                prev = tmp;
                player_1_time = 180;
                player_2_time = 180;
                lcd_cmd(L_CLR);
                lcd_cmd(L_L1); //Ustawienie karetki w pierwszej linii
                lcd_str("3 Minute time"); //napis
                lcd_cmd(L_L2);
                lcd_str("mode selected");
                delay(2000);
            }
            else if(tmp < 33 && tmp != prev)
            {
                prev = tmp;
                player_1_time = 300;
                player_2_time = 300;
                lcd_cmd(L_CLR);
                lcd_cmd(L_L1); //Ustawienie karetki w pierwszej linii
                lcd_str("5 Minute time"); //napis
                lcd_cmd(L_L2);
                lcd_str("mode selected");
                delay(2000);
            } 
        }
        
        // press and hold might be required due to how PicGenios board works
        // alternatively might be due to delay(1000) being called on first thing
        if(!PORTBbits.RB5) 
        {
            is_player_1 = true; //activate P1 time
            is_active = true; // disable potent. value check during runtime
        }
        
        if(!PORTBbits.RB3)
        {
            is_player_1 = false; // activate P2 time
            is_active = true;
        }
        
        if(is_player_1 && is_active)
        {
            player_1_time--;
        }
        else if(is_active)
        {
            player_2_time--;
        }
        
        if(player_1_time == 0)
        {
            is_active = win_display(false); // reset is_active so potentiometer can be used
            return;
        }
        if(player_2_time == 0)
        {
            is_active = win_display(true);
            return;
        }
        
        
        unsigned int p1_minutes = player_1_time / 60;
        unsigned int p1_seconds = player_1_time - (p1_minutes * 60);
        
        unsigned int p2_minutes = player_2_time / 60;
        unsigned int p2_seconds = player_2_time - (p2_minutes * 60);
        
        char name_display[] = "    P1    P2    "; // display player name at top
        
        char time_display[] = "                "; // time disp updated every 1s
        time_display_update(time_display, player_1_time, player_2_time);
        
        if(dot_side && is_active) // side-to-side activity indicator
        {
            name_display[0] = 'o'; // enable on left side
            dot_side = !dot_side; // bit flip to allow right side next time
        }
        else
        {
            if(is_active)
            {
                name_display[15] = 'o'; // enable on right side
                dot_side = !dot_side;   // bit flip again to allow left side
            }
        }
        
        lcd_cmd(L_L1);
        lcd_str(name_display);
        lcd_cmd(L_L2);
        lcd_str(time_display);
    }
}