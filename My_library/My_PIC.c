#include "My_PIC.h"

//*************************** timer0 ***************************//
#if defined(PIC18F27J53)

void timer0_init(uint8_t prescaler) { // prescaler is 0~8
    T0CONbits.TMR0ON = 1;
    T0CONbits.T08BIT = 1; // 8-bit timer
    T0CONbits.T0CS = 0; // use intOSC
    if (prescaler == 0) {
        T0CONbits.PSA = 1; // Not use prescaler
    } else {
        T0CONbits.PSA = 0; // use prescaler
        T0CONbits.T0PS = prescaler - 1; // 1 => 1:4
    }
    INTCONbits.T0IE = 1;
    INTCONbits.TMR0IF = 0;
    INTCON2bits.TMR0IP = 0; // low priority
    INTCONbits.PEIE = 1;
}
#endif /* PIC18F27J53 */
#if defined(PIC16F1827)

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
#endif /* PIC16F1827 */

//*************************** timer1 ***************************//
#if defined(PIC18F27J53)

void timer1_init(uint8_t prescaler, uint8_t clock_select) { // prescaler is 0~3
    T1CONbits.TMR1CS = clock_select;
    if (clock_select == T1OSC) T1CONbits.T1OSCEN = 1; // Drive Crystal
    else T1CONbits.T1OSCEN = 0;
    T1CONbits.T1CKPS = prescaler;
    T1CONbits.nT1SYNC = 1; // No Sync
    T1CONbits.RD16 = 0; //Timer1=16bit timer
    T1CONbits.TMR1ON = 1; //enable
    IPR1bits.TMR1IP = 0; // low priority
    PIR1bits.TMR1IF = 0;
    PIE1bits.TMR1IE = 1;
    INTCONbits.PEIE = 1;
}
#endif /* PIC18F27J53 */

#if defined(PIC16F1827)

void timer1_init(uint8_t prescaler) { // prescaler is 0~3
    T1CONbits.TMR1ON = 1;
    T1CONbits.TMR1CS = 0; //Fosc/4
    T1CONbits.T1CKPS = prescaler;
    TMR1H = 0x00;
    TMR1L = 0x00;
    PIE1bits.TMR1IE = 1;
    INTCONbits.PEIE = 1;
}
#endif /* PIC16F1827 */

//*************************** timer3 ***************************//
#if defined(PIC18F27J53)

void timer3_init(uint8_t prescaler) { // prescaler is 0~3
    T3CONbits.TMR3CS = 0; //Clock = Fosc/4
    T3CONbits.T3OSCEN = 0; // Not Drive Crystal
    T3CONbits.T3CKPS = prescaler;
    T3CONbits.RD163 = 0; //Timer3=16bit timer
    T3CONbits.TMR3ON = 1; //enable
    IPR2bits.TMR3IP = 0; // low priority
    PIR2bits.TMR3IF = 0;
    PIE2bits.TMR3IE = 1;
    INTCONbits.PEIE = 1;
}
#endif /* PIC18F27J53 */

//*************************** timer5 ***************************//
#if defined(PIC18F27J53)

void timer5_init(uint8_t prescaler) { // prescaler is 0~3
    T5CONbits.TMR5CS = 0; //Clock = Fosc/4
    T5CONbits.T5OSCEN = 0; // Not Drive Crystal
    T5CONbits.T5CKPS = prescaler;
    T5CONbits.RD165 = 0; //Timer1=16bit timer
    T5CONbits.TMR5ON = 1; //enable
    IPR5bits.TMR5IP = 0; // low priority
    PIR5bits.TMR5IF = 0;
    PIE5bits.TMR5IE = 1;
    INTCONbits.PEIE = 1;
}
#endif /* PIC18F27J53 */

//*************************** OSC ***************************//
#ifdef PIC18F27J53
//Fosc = 48MHz
//CONFIG : #pragma config OSC = INTOSCPLL

void OSC_init(void) {
    OSCCONbits.IRCF = 7;
    OSCTUNEbits.PLLEN = 1;
    OSCCONbits.SCS = 0;
}
#endif /* PIC18F27J53 */

#if defined(PIC18F26K22)
// freq = 0 ~ 6 : 1MHz ~ 64MHz

void OSC_init(uint8_t freq) {
    if (freq > 4) {
        OSCCONbits.IRCF = freq + 1; // 6 => 8MHz
        OSCCON2bits.PLLRDY = 1;
        OSCTUNEbits.PLLEN = 1; // clock x 4
    } else {
        OSCCONbits.IRCF = freq + 3; // 6 => 8MHz
        OSCCON2bits.PLLRDY = 0;
        OSCTUNEbits.PLLEN = 0; // clock x 1
    }
}
#endif /* PIC18F26K22 */

//*************************** ADC ***************************//
#ifdef PIC18F27J53

void ADC_init(uint8_t p_ref) {
    ADCON0bits.VCFG1 = 0; // Vref- = Vss
    ADCON0bits.VCFG0 = p_ref; // Vref+

    ADCON1bits.ADFM = 1; // 1 : H + L = 4 + 8
    ADCON1bits.ADCAL = 0; // normal ADC
    ADCON1bits.ADCS = 6; // clock is Fosc/64
}
#endif /* PIC18F27J53 */

#if defined(PIC16F1827) || defined(PIC12F1822)

void ADC_init(uint8_t p_ref) {
    ADCON1bits.ADFM = 0; // 0 : H + L = 8 + 2
    ADCON1bits.ADCS = 6; // clock is Fosc/64
    ADCON1bits.ADNREF = 0; //Vref- = Vss
    ADCON1bits.ADPREF = p_ref; //Vref+ = p_ref
    if (p_ref == FVR) FVRCON = 0x83; //4.096V
}
#endif /* PIC16F1827 || PIC12F1822 */

// Common Function

uint16_t ADC(uint8_t ch) {
    ADCON0bits.CHS = ch;
    ADCON0bits.ADON = 1;
    //__delay_us(5);
    PIR1bits.ADIF = 0;
    ADCON0bits.GO_nDONE = 1;
    while (!PIR1bits.ADIF);
    return ADRES;
}

//*************************** UART ***************************//
// UART_ISR();
// �����荞�݊֐����ɏ����Ă������ƁB
// RCIF,TXIF �t���O�̉���̓��W�X�^�ǂݍ��݂܂��͏㏑���ɂ��s����B
ringbuf_t tx_buf;
ringbuf_t rx_buf;

#if defined(PIC18F27J53)

void UART_init(void) {
    TXSTA1bits.TX9 = 0; // 0:8-bit
    TXSTA1bits.TXEN = 1; //1:enable
    TXSTA1bits.SYNC = 0; // 0:Asynchronous mode
    TXSTA1bits.BRGH = 0; // 1:High Speed
    RCSTA1bits.SPEN = 1; // 1:Serial Port enable
    RCSTA1bits.RX9 = 0; // 0:8-bit
    RCSTA1bits.CREN = 1; // 1:continuous receive enable
    BAUDCON1bits.BRG16 = 0; // 1:use 16-bit SPBRG
    SPBRG1 = _XTAL_FREQ / 64 / 9600 - 1;
    SPBRGH1 = 0;
    IPR1bits.RC1IP = 0; //0:low priority
    PIE1bits.RCIE = 1;
    INTCONbits.PEIE = 1;
}

void UART_ISR(void) {
    if (PIE1bits.TXIE && PIR1bits.TXIF) {
        if (ringbuf_num(&tx_buf)) {
            TXREG1 = ringbuf_pop(&tx_buf);
        } else {
            PIE1bits.TXIE = 0;
        }
    }
    if (PIE1bits.RCIE && PIR1bits.RCIF) {
        char ascii = RCREG1;
        ringbuf_put(&rx_buf, ascii);
    }
}

void tx_send(const char asciicode) {
    ringbuf_put(&tx_buf, asciicode);
    PIE1bits.TXIE = 1;
}

void tx_sends(const char *asciicode) {
    while (*asciicode) {
        ringbuf_put(&tx_buf, *asciicode++);
    }
    PIE1bits.TXIE = 1;
}
#endif /* PIC18F27J53 */

#if defined(PIC16F1827)

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
    ringbuf_put(&tx_buf, asciicode);
    PIE1bits.TXIE = 1;
}

void tx_sends(const uint8_t *asciicode) {
    while (*asciicode) {
        ringbuf_put(&tx_buf, *asciicode++);
    }
    PIE1bits.TXIE = 1;
}
#endif /* PIC16F1827 */

void tx_sendn(const uint16_t value, uint8_t digits) {
    while (digits--) {
        uint16_t temp = value;
        for (uint8_t i = 0; i < digits; i++) {
            temp /= 10;
        }
        temp %= 10;
        tx_send('0' + temp);
    }
}

//*************************** CTMU ***************************//
#if defined(PIC18F27J53) || defined(PIC18F26K22)

void CTMU_init(void) {
    CTMUCONH = 0x00;
    CTMUCONL = 0x90;
    CTMUICON = 0x01;

    ADCON1bits.ADFM = 1;
    ADCON1bits.ADCAL = 0;
    ADCON1bits.ACQT = 1;
    ADCON1bits.ADCS = 6;

    ADCON0bits.VCFG0 = 0;
    ADCON0bits.VCFG1 = 0;
    ADCON0bits.ADON = 1;
}

uint16_t CTMU_read(uint8_t ch) {
    uint32_t sum = 0;
    for (uint8_t i = 0; i < CTMU_AVERAGE_NUMBER; i++) {
        ADCON0bits.CHS = ch;
        CTMUCONHbits.CTMUEN = 1;
        CTMUCONLbits.EDG1STAT = 0;
        CTMUCONLbits.EDG2STAT = 0;

        // �[���d�ʂ���J�n������ׂɓd�ׂ���d������
        CTMUCONHbits.IDISSEN = 1;
        __delay_us(CTMU_DISCHARGE_TIME);
        CTMUCONHbits.IDISSEN = 0;
        // �d�ɂɏ[�d���s��
        CTMUCONLbits.EDG1STAT = 1;
        __delay_us(CTMU_CHARGE_TIME);
        CTMUCONLbits.EDG1STAT = 0;
        // �d�ɂ̓d��(AN0)��ǂݎ��
        PIR1bits.ADIF = 0;
        ADCON0bits.GO = 1;
        while (!PIR1bits.ADIF);
        sum += ADRES;
    }
    return sum / CTMU_AVERAGE_NUMBER / 4;
}
#endif  /* PIC18F27J53 || PIC18F26K22 */

//*************************** Delay_ms ***************************//

void Delay_ms(uint16_t ms) {
    while (ms--)__delay_ms(1);
}

//*************************** millis ***************************//
uint32_t ms;

void millis_timer() {
    static uint16_t cnt;
    if (cnt) {
        cnt;
    } else {
        cnt = 47;
        ms++;
    }
}

uint32_t millis(void) {
    return ms;
}
