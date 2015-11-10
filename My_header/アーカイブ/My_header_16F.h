/* 
 * File:   My_header_16F.h
 * Author: kerikun11
 *
 * Created on 2015/03/02, 12:45
 */

#ifndef MY_HEADER_16F_H
#define	MY_HEADER_16F_H

#ifndef _XTAL_FREQ
#define	_XTAL_FREQ 32000000
#endif

//*************************** timer0 ***************************//

void timer0_init(uint8_t prescaler) { // prescaler is 0~8
    if (prescaler == 0) {
        OPTION_REGbits.PSA = 1;
    } else {
        OPTION_REGbits.PSA = 0;
        OPTION_REGbits.PS = prescaler - 1;
    }
    OPTION_REGbits.TMR0CS = 0;
    OPTION_REGbits.TMR0SE = 0;
    INTCONbits.TMR0IE = 1;
    INTCONbits.PEIE = 1;
}
//*************************** timer1 ***************************//

void timer1_init(uint8_t prescaler) { // prescaler is 0~3
    T1CONbits.TMR1ON = 1;
    T1CONbits.TMR1CS = 0; //Fosc/4
    T1CONbits.T1CKPS = prescaler;
    TMR1H = 0x00;
    TMR1L = 0x00;
    PIE1bits.TMR1IE = 1;
    INTCONbits.PEIE = 1;
}

//*************************** adc ***************************//
#define FVR 3
#define VDD 0

void adc_init(uint8_t p_ref) {
    ADCON1bits.ADFM = 0; // 0 : H + L = 8 + 2
    ADCON1bits.ADCS = 6; // clock is Fosc/4
    ADCON1bits.ADNREF = 0; //Vref- = Vss
    ADCON1bits.ADPREF = p_ref; //Vref+ = p_ref
    if (p_ref == 3) FVRCON = 0x83; //4.096V
}

uint8_t adc(uint8_t ch) {
    ADCON0bits.CHS = ch;
    ADCON0bits.ADON = 1;
    __delay_us(5);
    ADCON0bits.GO_nDONE = 1;
    while (ADCON0bits.GO_nDONE);
    return ADRESH;
}

//*************************** UART ***************************//
#include <My_ringbuf.h>

ringbuf_t tx_buf;

void UART_init(void) {
    SPEN = 1;
    RCSTA = 0x90;
    BRG16 = 0;
    SPBRGH = 0;
    SPBRGL = 207;
    TXSTA = 0x24;
    PIE1bits.RCIE = 1;
    INTCONbits.PEIE = 1;
}

//void tx_send(uint8_t asciicode) {
//    ringbuf_push(&tx_buf, asciicode);
//    PIE1bits.TXIE = 1;
//}
//
//void tx_sends(const uint8_t asciicode[]) {
//    uint8_t i;
//    for (i = 0; asciicode[i]; i++) {
//        ringbuf_push(&tx_buf, asciicode[i]);
//    }
//    PIE1bits.TXIE = 1;
//}
//
//void interrupt_TXIF(void) {
//    if (TXIE && TXIF) {
//        TXIF = 0;
//        if (ringbuf_num(&tx_buf) > 0) {
//            TXREG = ringbuf_pop(&tx_buf);
//        } else {
//            PIE1bits.TXIE = 0;
//        }
//    }
//}

void interrupt_TXIF(void) {
    if (PIE1bits.TXIE && PIR1bits.TXIF) {
        PIR1bits.TXIF = 0;
        if (ringbuf_num(&tx_buf) > 0) {
            TXREG = ringbuf_pop(&tx_buf);
        } else {
            PIE1bits.TXIE = 0;
        }
    }
}

void tx_send(uint8_t asciicode) {
    ringbuf_push(&tx_buf, asciicode);
    PIE1bits.TXIE = 1;
}

void tx_sends(const uint8_t *asciicode) {
    while (*asciicode) {
        ringbuf_push(&tx_buf, *asciicode++);
    }
    PIE1bits.TXIE = 1;
}

void tx_sendn(uint16_t value, uint8_t digits) {
    while (digits--) {
        uint16_t temp = value;
        for (uint8_t i = 0; i < digits; i++) {
            temp /= 10;
        }
        temp %= 10;
        tx_send('0' + temp);
    }
}

//*************************** Delay_ms ***************************//

void Delay_ms(uint16_t ms) {
    while (ms--)__delay_ms(1);
}

//*************************** I2C ***************************//
#define ACK   0
#define NOACK 1
#define W_0  0
#define R_1  1

void I2C_IdleCheck(char mask) {
    while ((SSP1CON2 & 0x1F) | (SSP1STAT & mask));
}

void I2C_init(void) {
    SSP1STAT = 0b10000000; // �W�����x���[�h�ɐݒ肷��(100kHz)
    SSP1CON1 = 0b00101000; // SDA/SCL�s����I2C�Ŏg�p���A�}�X�^�[���[�h�Ƃ���
    SSP1ADD = 0x13; // �N���b�N=FOSC/((SSPADD + 1)*4) 8MHz/((0x13+1)*4)=0.1(100KHz)
    //SSP1IE = 1; // SSP(I2C)���荞�݂�������
    //BCL1IE = 1; // MSSP(I2C)�o�X�Փˊ��荞�݂�������
    SSP1IF = 0; // SSP(I2C)���荞�݃t���O���N���A����
    BCL1IF = 0; // MSSP(I2C)�o�X�Փˊ��荞�݃t���O���N���A����
}

uint8_t I2C_Start(uint8_t adrs, uint8_t rw) {
    // �X�^�[�g(START CONDITION)
    I2C_IdleCheck(0x5);
    SSP1CON2bits.SEN = 1;
    // [�X���[�u�̃A�h���X]�𑗐M����
    I2C_IdleCheck(0x5);
    SSP1IF = 0;
    SSP1BUF = (uint8_t) ((adrs << 1) + rw); // �A�h���X + R/W�𑗐M
    while (!SSP1IF); // ���肩���ACK�ԓ���҂�
    return SSP1CON2bits.ACKSTAT;
}

uint8_t I2C_rStart(int adrs, int rw) {
    // ���s�[�g�E�X�^�[�g(REPEATED START CONDITION)
    I2C_IdleCheck(0x5);
    SSP1CON2bits.RSEN = 1;
    // [�X���[�u�̃A�h���X]�𑗐M����
    I2C_IdleCheck(0x5);
    SSP1IF = 0;
    SSP1BUF = (char) ((adrs << 1) + rw); // �A�h���X + R/W�𑗐M
    while (!SSP1IF); // ���肩���ACK�ԓ���҂�
    return SSP1CON2bits.ACKSTAT;
}

void I2C_Stop(void) {
    // �X�g�b�v(STOP CONDITION)
    I2C_IdleCheck(0x5);
    SSP1CON2bits.PEN = 1;
}

uint8_t I2C_Send(uint8_t data) {
    I2C_IdleCheck(0x5);
    SSP1IF = 0;
    SSP1BUF = data; // �f�[�^�𑗐M
    while (!SSP1IF); // ���肩���ACK�ԓ���҂�
    return SSP1CON2bits.ACKSTAT;
}

uint8_t I2C_Receive(uint8_t ack) {
    uint8_t data;

    I2C_IdleCheck(0x5);
    SSP1CON2bits.RCEN = 1; // ��M��������
    I2C_IdleCheck(0x4);
    data = SSP1BUF; // �f�[�^�̎�M
    I2C_IdleCheck(0x5);
    SSP1CON2bits.ACKDT = ack; // ACK�f�[�^�̃Z�b�g
    SSP1CON2bits.ACKEN = 1; // ACK�f�[�^��Ԃ�
    return data;
}

//*************************** SPI ***************************//
#define Master 0
#define Slave 1

void SPI_init(uint8_t mode) {
    SSP1CON1bits.SSPEN = 1;
    SSP1CON1bits.CKP = 1;
    SSP1STATbits.CKE = 0;
    if (mode == Master) {
        SSP1CON1bits.SSPM = 2;
    } else if (mode == Slave) {
        SSP1CON1bits.SSPM = 4;
        SSP1STATbits.SMP = 0;
    }
    if (mode == Slave) {
        SSP1IF = 0;
        PIE1bits.SSP1IE = 1;
        INTCONbits.PEIE = 1;
    }
}

uint8_t SPI_Transfer(uint8_t txdata) {
    uint8_t rxdata;

    rxdata = SSP1BUF;
    SSP1IF = 0;
    SSP1BUF = txdata;
    while (!SSP1IF);
    rxdata = SSP1BUF;
    return rxdata;
}
//*************************** stdlib.h ***************************//

void utoa(uint8_t *str, uint32_t value, uint8_t digits) {
    uint32_t v;
    uint8_t d;
    while (digits--) {
        v = value;
        d = digits;
        while (d--) {
            v /= 10;
        }
        v %= 10;
        *str++ = '0' + v;
    }
    *str = '\0';
}

#endif	/* MY_HEADER_16F_H */

