/* 
 * File:   My_I2C.h
 * Author: kerikun11
 *
 * Created on 2015/02/26, 19:52
 */

#ifndef MY_I2C_H
#define	MY_I2C_H

#include<stdint.h>

//************************************************************************//
//*************************** PIC's I2C Module ***************************//
//************************************************************************//
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

/***********************************************************************/
/*************************** IC's I2C Module ***************************/
/***********************************************************************/

/*************************** I2C_LCD ST7032 ***************************/
#ifdef ST7032
#define ST7032_ADRES 0x3E      // �H���d�q��I2C�ڑ����^LCD���W���[���̃A�h���X

void I2C_LCD_Command(uint8_t c) {
    uint8_t ans;

    ans = I2C_Start(ST7032_ADRES, W_0); // �X�^�[�g�R���f�B�V�����𔭍s����
    if (ans == 0) {
        // command word �̑��M
        I2C_Send(0b10000000); // control byte �̑��M(�R�}���h���w��)
        I2C_Send(c); // data byte �̑��M
    }
    I2C_Stop(); // �X�g�b�v�R���f�B�V�����𔭍s����
    __delay_us(26);
}

void I2C_LCD_Clear(void) {
    I2C_LCD_Command(0x01); // Clear Display : ��ʑS�̂�20H�̽�߰��ŕ\���A���ق�col=0,row=0�Ɉړ�
    __delay_us(1100); // LCD������(1.08ms)����̂�҂��܂�
}

void I2C_LCD_SetCursor(uint8_t col, uint8_t row) {
    uint8_t row_offsets[] = {0x00, 0x40};

    I2C_LCD_Command(0x80 | (col + row_offsets[row])); // Set DDRAM Adddress : 00H-07H,40H-47H
}

void I2C_LCD_Putc(uint8_t c) {
    uint8_t ans;

    ans = I2C_Start(ST7032_ADRES, W_0); // �X�^�[�g�R���f�B�V�����𔭍s����
    if (ans == 0) {
        // command word �̑��M
        I2C_Send(0b11000000); // control byte �̑��M(�f�[�^���w��)
        I2C_Send(c); // data byte �̑��M
    }
    I2C_Stop(); // �X�g�b�v�R���f�B�V�����𔭍s����
    __delay_us(26);
}

void I2C_LCD_Puts(const uint8_t * s) {
    int ans;

    ans = I2C_Start(ST7032_ADRES, W_0); // �X�^�[�g�R���f�B�V�����𔭍s����
    if (ans == 0) {
        I2C_Send(0b01000000); // control byte �̑��M(�f�[�^���w��)
        while (*s) {
            I2C_Send(*s++); // data byte �̑��M(�A�����M)
            __delay_us(26);
        }
    }
    I2C_Stop(); // �X�g�b�v�R���f�B�V�����𔭍s����
}

void I2C_LCD_CreateChar(int p, char *dt) {
    uint8_t ans;

    ans = I2C_Start(ST7032_ADRES, W_0); // �X�^�[�g�R���f�B�V�����𔭍s����
    if (ans == 0) {
        //  LCD�ɃL�����ۑ���̃A�h���X���w������
        I2C_Send(0b10000000); // control byte �̑��M(�R�}���h���w��)
        I2C_Send(0x40 | (p << 3));
        __delay_us(26);
        //  LCD�ɃL�����f�[�^�𑗐M����
        I2C_Send(0b01000000); // control byte �̑��M(�f�[�^���w��)
        for (uint8_t i = 0; i < 7; i++) {
            I2C_Send(*dt++);
            __delay_us(26);
        }
    }
    I2C_Stop(); // �X�g�b�v�R���f�B�V�����𔭍s����
}

void I2C_LCD_init(void) {
    Delay_ms(40); // �d���n�m��40ms�܂ő҂��Ă��珉����
    I2C_LCD_Command(0x38); // function set           : �f�[�^����8�{�E�\���͂Q�s�E�t�H���g��5x8�h�b�g
    I2C_LCD_Command(0x39); // function set           : �g���R�}���h�̐ݒ��L���ɂ���
    I2C_LCD_Command(0x14); // Internal OSC frequency : �o�C�A�X�̑I���Ɠ���OSC���g���̒���
    I2C_LCD_Command(0x70); // Contrast set           : �R���g���X�g�����f�[�^(����4�r�b�g)
    I2C_LCD_Command(0x56); // Contrast set           : ������H�L���A�R���g���X�g�����f�[�^(���2�r�b�g)
    I2C_LCD_Command(0x6C); // Follower control       : �t�H���A��H��ON�A�������̒������s��
    Delay_ms(200); // �d�͂����肷��܂ő҂�
    I2C_LCD_Command(0x38); // function set           : �g���R�}���h��ݒ�𖳌��ɂ���
    I2C_LCD_Command(0x0C); // display control        : ��ʕ\����ON�E�J�[�\���\����OFF�E�J�[�\���_�ł�OFF
    I2C_LCD_Command(0x06); // entry mode set         : ������\���������ɃJ�[�\�����ړ�������w��
    I2C_LCD_Clear(); // Clear Display          : ��ʂ���������
}
#endif /* I2C_LCD */

//*************************** I2C_RTC DS1307 ***************************//
#ifdef DS1307
#define DS1307_ADRES 0x68

void RTC_Write(uint8_t Reg, uint8_t data) {
    uint8_t ans;

    ans = I2C_Start(DS1307_ADRES, W_0); // �X�^�[�g�R���f�B�V�����𔭍s����
    if (ans == 0) {
        // command word �̑��M
        I2C_Send(Reg); // control byte �̑��M(�R�}���h���w��)
        I2C_Send(data); // data byte �̑��M
    }
    I2C_Stop(); // �X�g�b�v�R���f�B�V�����𔭍s����
    __delay_us(26);
}

uint8_t RTC_Read(uint8_t Reg) {
    uint8_t ans;
    uint8_t data;

    ans = I2C_Start(DS1307_ADRES, W_0); // �X�^�[�g�R���f�B�V�����𔭍s����
    if (ans == 0) {
        I2C_Send(Reg); // control byte �̑��M(�R�}���h���w��)
    }
    ans = I2C_rStart(DS1307_ADRES, R_1); // re�X�^�[�g�R���f�B�V�����𔭍s����
    if (ans == 0) {
        data = I2C_Receive(1);
    }
    I2C_Stop(); // �X�g�b�v�R���f�B�V�����𔭍s����
    __delay_us(26);
    return data;
}
#endif /* DS1307 */

//*************************** I2C_ThermoMeter TM ***************************//
#ifdef MCP9803
#define MCP9803_ADRES 0x48

void TM_init(uint8_t config) {
    uint8_t ans;
    ans = I2C_Start(MCP9803_ADRES, W_0); // �X�^�[�g�R���f�B�V�����𔭍s����
    if (ans == 0) {
        I2C_Send(0x01); // control byte �̑��M(�R�}���h���w��)
        I2C_Send(config);
    }
    I2C_Stop(); // �X�g�b�v�R���f�B�V�����𔭍s����
    __delay_us(26);
}

uint16_t TM_Read() {
    uint8_t dataH;
    uint8_t dataL;
    uint8_t ans;
    ans = I2C_Start(MCP9803_ADRES, W_0); // �X�^�[�g�R���f�B�V�����𔭍s����
    if (ans == 0) {
        I2C_Send(0x00); // control byte �̑��M(�R�}���h���w��)
    }
    ans = I2C_rStart(MCP9803_ADRES, R_1); // �X�^�[�g�R���f�B�V�����𔭍s����
    if (ans == 0) {
        dataH = I2C_Receive(0); // control byte �̑��M(�R�}���h���w��)
        dataL = I2C_Receive(1); // control byte �̑��M(�R�}���h���w��)
    }
    I2C_Stop(); // �X�g�b�v�R���f�B�V�����𔭍s����
    __delay_us(26);
    /*
    if (ans)tx_sends("Failed\n");
    else tx_sends("Successful\n");
     */
    return ((0xFF00 & (dataH << 8)) + (0xFF & dataL));
}
#endif /* MCP9803 */

//*************************** I2C_EEPROM ***************************//
#ifdef EEPROM24LC64
#define EEPROM24LC64_ADRES 0x50

void EEP_write(uint8_t Reg_h, uint8_t Reg_l, uint8_t data) {
    uint8_t ans;
    ans = I2C_Start(EEPROM24LC64_ADRES, W_0); // �X�^�[�g�R���f�B�V�����𔭍s����
    if (ans == 0) {
        // command word �̑��M
        I2C_Send(Reg_h); // control byte �̑��M(�R�}���h���w��)
        I2C_Send(Reg_l); // control byte �̑��M(�R�}���h���w��)
        I2C_Send(data); // data byte �̑��M
    }
    I2C_Stop(); // �X�g�b�v�R���f�B�V�����𔭍s����
    __delay_us(26);
}

uint8_t EEP_read(uint8_t Reg_h, uint8_t Reg_l) {
    uint8_t ans;
    uint8_t data;

    ans = I2C_Start(EEPROM24LC64_ADRES, W_0); // �X�^�[�g�R���f�B�V�����𔭍s����
    if (ans == 0) {
        I2C_Send(Reg_h); // control byte �̑��M(�R�}���h���w��)
        I2C_Send(Reg_l); // control byte �̑��M(�R�}���h���w��)
    }
    ans = I2C_rStart(EEPROM24LC64_ADRES, R_1); // re�X�^�[�g�R���f�B�V�����𔭍s����
    if (ans == 0) {
        data = I2C_Receive(1);
    }
    I2C_Stop(); // �X�g�b�v�R���f�B�V�����𔭍s����
    __delay_us(26);
    return data;
}

uint16_t EEP_read16(uint8_t Reg_h, uint8_t Reg_l) {
    uint8_t ans;
    uint8_t d[4];
    uint16_t data;

    ans = I2C_Start(EEPROM24LC64_ADRES, W_0); // �X�^�[�g�R���f�B�V�����𔭍s����
    if (ans == 0) {
        I2C_Send(Reg_h); // control byte �̑��M(�R�}���h���w��)
        I2C_Send(Reg_l); // control byte �̑��M(�R�}���h���w��)
    }
    ans = I2C_rStart(EEPROM24LC64_ADRES, R_1); // re�X�^�[�g�R���f�B�V�����𔭍s����
    if (ans == 0) {
        d[3] = I2C_Receive(0);
        d[2] = I2C_Receive(0);
        d[1] = I2C_Receive(0);
        d[0] = I2C_Receive(1);
    }
    I2C_Stop(); // �X�g�b�v�R���f�B�V�����𔭍s����
    __delay_us(26);
    data = (uint16_t) ((d[1] << 8) + d[0]);
    return data;
}

uint32_t EEP_read32(uint8_t Reg_h, uint8_t Reg_l) {
    uint8_t ans;
    uint8_t d[4];
    uint32_t data;

    ans = I2C_Start(EEPROM24LC64_ADRES, W_0); // �X�^�[�g�R���f�B�V�����𔭍s����
    if (ans == 0) {
        I2C_Send(Reg_h); // control byte �̑��M(�R�}���h���w��)
        I2C_Send(Reg_l); // control byte �̑��M(�R�}���h���w��)
    }
    ans = I2C_rStart(EEPROM24LC64_ADRES, R_1); // re�X�^�[�g�R���f�B�V�����𔭍s����
    if (ans == 0) {
        d[3] = I2C_Receive(0);
        d[2] = I2C_Receive(0);
        d[1] = I2C_Receive(0);
        d[0] = I2C_Receive(1);
    }
    I2C_Stop(); // �X�g�b�v�R���f�B�V�����𔭍s����
    __delay_us(26);
    return (uint32_t) (d[3] << 24)+(d[2] << 16)+(d[1] << 8) + d[0];
}

void EEP_write16(uint8_t Reg_h, uint8_t Reg_l, uint16_t data) {
    uint8_t ans;
    ans = I2C_Start(EEPROM24LC64_ADRES, W_0); // �X�^�[�g�R���f�B�V�����𔭍s����
    if (ans == 0) {
        // command word �̑��M
        I2C_Send(Reg_h); // control byte �̑��M(�R�}���h���w��)
        I2C_Send(Reg_l); // control byte �̑��M(�R�}���h���w��)
        I2C_Send(data >> 8); // data byte �̑��M
        I2C_Send(data >> 0); // data byte �̑��M
    }
    I2C_Stop(); // �X�g�b�v�R���f�B�V�����𔭍s����
    __delay_us(26);
}

void EEP_write32(uint8_t Reg_h, uint8_t Reg_l, uint32_t data) {
    uint8_t ans;
    ans = I2C_Start(EEPROM24LC64_ADRES, W_0); // �X�^�[�g�R���f�B�V�����𔭍s����
    if (ans == 0) {
        // command word �̑��M
        I2C_Send(Reg_h); // control byte �̑��M(�R�}���h���w��)
        I2C_Send(Reg_l); // control byte �̑��M(�R�}���h���w��)
        I2C_Send(data >> 24); // data byte �̑��M
        I2C_Send(data >> 16); // data byte �̑��M
        I2C_Send(data >> 8); // data byte �̑��M
        I2C_Send(data >> 0); // data byte �̑��M
    }
    I2C_Stop(); // �X�g�b�v�R���f�B�V�����𔭍s����
    __delay_us(26);
}
#endif /* EEPROM24LC64 */

#endif	/* MY_I2C_H */

