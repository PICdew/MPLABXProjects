#include "My_DS1307.h"

#include <My_I2C.h>

//*************************** I2C_RTC DS1307 ***************************//

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
