#include "My_EEPROM.h"

#include <My_PIC.h>
#include <My_I2C.h>
//*************************** I2C_EEPROM ***************************//

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
    return (uint32_t) ((uint32_t) d[3] << 24)+((uint32_t) d[2] << 16)+(d[1] << 8) + d[0];
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
