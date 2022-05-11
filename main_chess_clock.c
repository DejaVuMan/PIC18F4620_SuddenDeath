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
    
    delay(1000);
    lcd_cmd(L_CLR);
    lcd_cmd(L_L1); //Ustawienie karetki w pierwszej linii
    lcd_str(" RB0 to set time "); //napis
    delay(3000);
    lcd_cmd(L_CLR);
    
    while(1)
    {
        delay(1000); // 1s delay
        if(!PORTBbits.RB0)
        {
            switch(mode_counter)
            {
                case 0:
                    player_1_time = 300;
                    player_2_time = 300;
                    mode_counter++;
                    break;
                case 1:
                    player_1_time = 180;
                    player_2_time = 180;
                    mode_counter++;
                    break;
                case 2:
                    player_1_time = 60;
                    player_2_time = 60;
                    mode_counter = 0;
                    break;
                default:
                    mode_counter = 0; // if mode counter is out of bounds, reset
                    break;
            }
        }
        
        if(!PORTBbits.RB1)
        {
            is_player_1 = true; //activate P1 time
        }
        
        if(!PORTBbits.RB2)
        {
            is_player_1 = false; // activate P2 time
        }
        
        if(is_player_1)
        {
            player_1_time--;
        }
        else
        {
            player_2_time--;
        }
        
        
        int p1_minutes = player_1_time / 60;
        int p1_seconds = player_1_time - (p1_minutes * 60);
        
        int p2_minutes = player_2_time / 60;
        int p2_seconds = player_2_time - (p2_minutes * 60);
        
        char player_1_display[] = "P1:             ";
        player_1_display[12] = p1_minutes - (p1_minutes / 10 * 10) + '0';
        player_1_display[13] = ':';
        player_1_display[14] = p1_seconds/10 + '0';
        player_1_display[15] = p1_seconds - (p1_seconds / 10 * 10) + '0';
        
        char player_2_display[] = "P2:             ";
        player_2_display[12] = p2_minutes - (p2_minutes / 10 * 10) + '0';
        player_2_display[13] = ':';
        player_2_display[14] = p2_seconds/10 + '0';
        player_2_display[15] = p2_seconds - (p2_seconds / 10 * 10) + '0';
        
        lcd_cmd(L_L1);
        lcd_str(player_1_display);
        lcd_cmd(L_L2);
        lcd_str(player_2_display);
    }
    return;
}
