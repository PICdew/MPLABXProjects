// PIC24FJ64GB002 Configuration Bit Settings
// 2015.04.12

#include <xc.h>
#include <stdint.h>

_CONFIG1(WDTPS_PS1 & FWPSA_PR32 & WINDIS_OFF & FWDTEN_OFF & ICS_PGx1 & GWRP_OFF & GCP_OFF & JTAGEN_OFF)
_CONFIG2(POSCMOD_NONE & I2C1SEL_PRI & IOL1WAY_OFF & OSCIOFNC_ON & FCKSM_CSDCMD & FNOSC_FRCPLL & PLL96MHZ_ON & PLLDIV_DIV2 & IESO_OFF)
_CONFIG3(WPFP_WPFP0 & SOSCSEL_IO & WUTSEL_LEG & WPDIS_WPDIS & WPCFG_WPCFGDIS & WPEND_WPENDMEM)
//_CONFIG3(WPFP_WPFP0 & SOSCSEL_SOSC & WUTSEL_LEG & WPDIS_WPDIS & WPCFG_WPCFGDIS & WPEND_WPENDMEM)
_CONFIG4(DSWDTPS_DSWDTPS3 & DSWDTOSC_LPRC & RTCOSC_SOSC & DSBOREN_OFF & DSWDTEN_OFF)

#define _XTAL_FREQ 32000000
#define CPU_CLOCK 4000000                        // �N���b�N [Hz]
#define CPU_PLL   8                                // PLL
#define FCY       ( ( CPU_CLOCK * CPU_PLL ) / 2 )  // ���߃T�C�N���N���b�N [Hz]

#define BAUDRATE  9600                          // �{�[���[�g [bps]

#include <uart.h>

void __attribute__((interrupt, auto_psv)) _U1RXInterrupt(void) {
    IFS0bits.U1RXIF = 0;
    char ascii = U1RXREG;
    tx_send(ascii);
}

void __attribute__((interrupt, auto_psv)) _U1TXInterrupt(void) {
    if (IFS0bits.U1TXIF && IEC0bits.U1TXIE) {
        IFS0bits.U1TXIF = 0;
        if (ringbuf_num(&tx_buf) > 0) {
            U1TXREG = ringbuf_pop(&tx_buf);
        } else {
            IEC0bits.U1TXIE = 0;
        }
    }
}

int main(void) {
    OSCCONbits.COSC = 1;
    OSCCONbits.NOSC = 1;
    CLKDIVbits.PLLEN = 1;

    OSCCONbits.IOLOCK = 0;
    if (OSCCONbits.IOLOCK != 0) {
        OSCCON = (OSCCON & 0xFF00) + 0x0046;
        OSCCON = (OSCCON & 0xFF00) + 0x0057;
        OSCCONbits.IOLOCK = 0;
    }
    RPINR18bits.U1RXR = 9;
    RPOR4bits.RP8R = 3;

    AD1PCFG = 0xFFFF;
    TRISA = 0x0000;
    TRISB = 0x0200;

    const unsigned int Mode
            = UART_EN // UART���W���[�� - �L��
            & UART_IDLE_STOP // �A�C�h�����[�h - �����~
            & UART_DIS_WAKE // �E�F�C�N�A�b�v - ����
            & UART_DIS_LOOPBACK // ���[�v�o�b�N - ����
            & UART_DIS_ABAUD // �����{�[���[�g - ����
            & UART_NO_PAR_8BIT // �p���e�B�Ȃ�/�f�[�^ 8�r�b�g
            & UART_1STOPBIT; // �X�g�b�v�r�b�g - 1�r�b�g

    const unsigned int Status
            = UART_TX_ENABLE // ���M - �L��
            & UART_INT_TX_BUF_EMPTY // ���M�̊��荞�ݏ��� - ���M�o�b�t�@����
            & UART_INT_RX_CHAR // ��M�̊��荞�ݏ��� - ��M���邽��
            & UART_TX_PIN_NORMAL // ���M�u���[�N - �ʏ�
            & UART_ADR_DETECT_DIS // �A�h���X���o - ����
            & UART_RX_OVERRUN_CLEAR; // ��M�o�b�t�@�I�[�o�[�����G���[ - �N���A

    const double Baudrate = (double) FCY / (16 * BAUDRATE) - 1;

    // �{�[���[�g�̏����_�ȉ����l�̌ܓ�����
    unsigned int baudrate = (unsigned int) (Baudrate + 0.5);

    OpenUART1(Mode, Status, baudrate);

    uint16_t i;

    while (1) {
        LATAbits.LATA0 = 1;
        i = 0xFFFF;
        while (i--);
        LATAbits.LATA0 = 0;
        i = 0xFFFF;
        while (i--);
    }

    return 0;
}
