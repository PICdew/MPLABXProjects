#include <xc.h>
#include <stdint.h>
// CONFIG1
#pragma config FOSC = INTOSC    // Oscillator Selection (INTOSC oscillator: I/O function on CLKIN pin)
#pragma config WDTE = OFF       // Watchdog Timer Enable (WDT disabled)
#pragma config PWRTE = OFF      // Power-up Timer Enable (PWRT disabled)
#pragma config MCLRE = OFF      // MCLR Pin Function Select (MCLR/VPP pin function is digital input)
#pragma config CP = OFF         // Flash Program Memory Code Protection (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Memory Code Protection (Data memory code protection is disabled)
#pragma config BOREN = OFF      // Brown-out Reset Enable (Brown-out Reset disabled)
#pragma config CLKOUTEN = OFF   // Clock Out Enable (CLKOUT function is disabled. I/O or oscillator function on the CLKOUT pin)
#pragma config IESO = OFF       // Internal/External Switchover (Internal/External Switchover mode is disabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enable (Fail-Safe Clock Monitor is disabled)
// CONFIG2
#pragma config WRT = OFF        // Flash Memory Self-Write Protection (Write protection off)
#pragma config PLLEN = ON       // PLL Enable (4x PLL disabled)
#pragma config STVREN = OFF     // Stack Overflow/Underflow Reset Enable (Stack Overflow or Underflow will not cause a Reset)
#pragma config BORV = LO        // Brown-out Reset Voltage Selection (Brown-out Reset Voltage (Vbor), low trip point selected.)
#pragma config LVP = OFF        // Low-Voltage Programming Enable (High-voltage on MCLR/VPP must be used for programming)
#define _XTAL_FREQ 32000000
#include "My_header_16F.h"

button_t sw_RA0;
button_t sw_RA1;

unsigned char cnt_t1;
unsigned char ss;
unsigned char mm;
unsigned char hh;
unsigned char EE;
unsigned char DD;
unsigned char MM;
unsigned char YY;

void interrupt isr(void);
uint8_t month_length(uint8_t year, uint8_t month);
void timeset(uint8_t *value, uint8_t min_num, uint8_t max_num, uint8_t Reg, uint8_t column, uint8_t row);
void display(void);

void interrupt isr(void) {
    interrupt_TXIF();
    if (RCIF) {
        RCIF = 0;
        tx_send(RCREG);
    }
    if (TMR1IF == 1) {
        TMR1IF = 0;
        cnt_t1++;
        if (cnt_t1 % 16 == 0)LATB0 = ~LATB0;
        button_timer_interrupt(&sw_RA0, !RA0);
        button_timer_interrupt(&sw_RA1, !RA1);
    }
}

uint8_t month_length(uint8_t year, uint8_t month) {
    switch (month) {
        case 1:
        case 3:
        case 5:
        case 7:
        case 8:
        case 10:
        case 12:
            return 31;
        case 4:
        case 6:
        case 9:
        case 11:
            return 30;
        case 2:
            if (year % 4 == 0) return 29;
            else return 28;
    }
    return 0;
}

void timeset(uint8_t *value, uint8_t min_num, uint8_t max_num, uint8_t Reg, uint8_t column, uint8_t row) {
    sw_RA0.flags = 0;
    sw_RA1.flags = 0;
    while (!sw_RA0.flag.press) {
        if (cnt_t1 % 16 >= 8) {
            LCD_SetCursor(column, row); // 表示位置を設定する
            LCD_Puts("  ");
        } else {
            *value = RTC_Read(Reg);
            display();
            LCD_SetCursor(column, row); // 表示位置を設定する
            LCD_Putc(*value / 16 + '0');
            LCD_Putc(*value % 16 + '0');
        }
        if (sw_RA1.flag.press) {
            sw_RA1.flag.press = 0;
            uint8_t t = *value % 16 + (*value / 16)*10;
            if (t >= max_num) {
                t = min_num;
            } else {
                t++;
            }
            *value = t % 10 + (t / 10)* 16;
            RTC_Write(Reg, *value);
        }
        if (sw_RA1.flag.long_holding_1) {
            sw_RA1.flag.long_holding_1 = 0;
            uint8_t t = *value % 16 + (*value / 16)*10;
            if (t + 9 >= max_num) {
                t = t % 10;
            } else {
                t += 10;
            }
            *value = t % 10 + (t / 10)* 16;
            RTC_Write(Reg, *value);
        }
    }
    sw_RA0.flags = 0;
}

void display(void) {
    ss = RTC_Read(0x00);
    mm = RTC_Read(0x01);
    hh = RTC_Read(0x02);
    EE = RTC_Read(0x03);
    DD = RTC_Read(0x04);
    MM = RTC_Read(0x05);
    YY = RTC_Read(0x06);
    LCD_SetCursor(0, 0); // 表示位置を設定する
    LCD_Putc(YY / 16 + '0');
    LCD_Putc(YY % 16 + '0');
    LCD_Putc('/');
    LCD_Putc(MM / 16 + '0');
    LCD_Putc(MM % 16 + '0');
    LCD_Putc('/');
    LCD_Putc(DD / 16 + '0');
    LCD_Putc(DD % 16 + '0');
    LCD_SetCursor(0, 1); // 表示位置を設定する
    LCD_Putc(hh / 16 + '0');
    LCD_Putc(hh % 16 + '0');
    LCD_Putc(':');
    LCD_Putc(mm / 16 + '0');
    LCD_Putc(mm % 16 + '0');
    LCD_Putc('-');
    LCD_Putc(ss / 16 + '0');
    LCD_Putc(ss % 16 + '0');
}

void main(void) {
    OSCCON = 0b11110000; // 内部クロックは８ＭＨｚとする
    OPTION_REG = 0b00000000; // デジタルI/Oに内部プルアップ抵抗を使用する
    ANSELA = 0b00000000; // AN0-AN4は使用しない全てデジタルI/Oとする
    ANSELB = 0b00000000; // AN5-AN11は使用しない全てデジタルI/Oとする
    TRISA = 0b00000011; // ピン(RA)は全て出力に割当てる(RA5は入力のみとなる)
    TRISB = 0b00010110; // ピン(RB)はRB4(SCL1)/RB1(SDA1)のみ入力
    WPUB = 0b00010010; // RB1/4は内部プルアップ抵抗を指定する
    PORTA = 0b00000000; // RA出力ピンの初期化(全てLOWにする)
    PORTB = 0b00000000; // RB出力ピンの初期化(全てLOWにする)
    APFCON0bits.RXDTSEL = 1;
    APFCON1bits.TXCKSEL = 1;

    T1CON = 0x21; //Fosc/4, ps:1/4
    TMR1H = 0x00;
    TMR1L = 0x00;
    PIE1bits.TMR1IE = 1;

    I2C_init();
    LCD_init();
    UART_init(PIC16F1827);

    INTCONbits.PEIE = 1;
    INTCONbits.GIE = 1;

    uint8_t buf[70];
    ringbuf_init(&tx_buf, buf, sizeof (buf));

    RTC_Write(0x07, 0x00);

    while (1) {
        display();
        if (sw_RA1.flag.press) {
            sw_RA1.flag.press=0;
            tx_send('2');
            tx_send('0');
            tx_send(((YY & 0xF0) >> 4) + '0');
            tx_send((YY & 0x0F) + '0');
            tx_send('/');
            tx_send(((MM & 0xF0) >> 4) + '0');
            tx_send((MM & 0x0F) + '0');
            tx_send('/');
            tx_send(((DD & 0xF0) >> 4) + '0');
            tx_send((DD & 0x0F) + '0');
            tx_send('(');
            tx_send((EE & 0x0F) + '0');
            tx_send(')');
            tx_send(((hh & 0xF0) >> 4) + '0');
            tx_send((hh & 0x0F) + '0');
            tx_send(':');
            tx_send(((mm & 0xF0) >> 4) + '0');
            tx_send((mm & 0x0F) + '0');
            tx_send('-');
            tx_send(((ss & 0xF0) >> 4) + '0');
            tx_send((ss & 0x0F) + '0');
            tx_send('.');
            tx_send('\n');
            tx_sends("Real Time Clock\n");
        }
        if (sw_RA0.flag.long_holding_1) {
            sw_RA0.flag.long_holding_1 = 0;
            timeset(&YY, 0, 99, 0x06, 0, 0);
            timeset(&MM, 1, 12, 0x05, 3, 0);
            timeset(&DD, 1, month_length(YY, MM), 0x04, 6, 0);
            timeset(&hh, 0, 23, 0x02, 0, 1);
            timeset(&mm, 0, 59, 0x01, 3, 1);
            timeset(&ss, 0, 59, 0x00, 6, 1);
        }
    }
}

