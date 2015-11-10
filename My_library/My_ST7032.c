#include "My_ST7032.h"

#include "My_I2C.h" // for I2C_...()
#include "My_PIC.h" // for _XTAL_FREQ

/*************************** I2C_LCD ST7032 ***************************/

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
    for (uint8_t i = 0; i < 40; i++) __delay_ms(1); // �d���n�m��40ms�܂ő҂��Ă��珉����
    I2C_LCD_Command(0x38); // function set           : �f�[�^����8�{�E�\���͂Q�s�E�t�H���g��5x8�h�b�g
    I2C_LCD_Command(0x39); // function set           : �g���R�}���h�̐ݒ��L���ɂ���
    I2C_LCD_Command(0x14); // Internal OSC frequency : �o�C�A�X�̑I���Ɠ���OSC���g���̒���
    I2C_LCD_Command(0x70); // Contrast set           : �R���g���X�g�����f�[�^(����4�r�b�g)
    I2C_LCD_Command(0x56); // Contrast set           : ������H�L���A�R���g���X�g�����f�[�^(���2�r�b�g)
    I2C_LCD_Command(0x6C); // Follower control       : �t�H���A��H��ON�A�������̒������s��
    for (uint8_t i = 0; i < 40; i++) __delay_ms(1); // �d�͂����肷��܂ő҂�
    I2C_LCD_Command(0x38); // function set           : �g���R�}���h��ݒ�𖳌��ɂ���
    I2C_LCD_Command(0x0C); // display control        : ��ʕ\����ON�E�J�[�\���\����OFF�E�J�[�\���_�ł�OFF
    I2C_LCD_Command(0x06); // entry mode set         : ������\���������ɃJ�[�\�����ړ�������w��
    I2C_LCD_Clear(); // Clear Display          : ��ʂ���������
}
