#include <xc.h>
#include <stdint.h>
__CONFIG(CLKOUTEN_OFF & FOSC_INTOSC & FCMEN_OFF & IESO_OFF & BOREN_ON &
        PWRTE_ON & WDTE_OFF & MCLRE_OFF & CP_OFF & CPD_OFF);
__CONFIG(PLLEN_OFF & STVREN_ON & WRT_OFF & BORV_HI & LVP_OFF);
#define _XTAL_FREQ 8000000    // �g�p����PIC���ɂ�蓮����g���l��ݒ肷��
#define ST7032_ADRES 0x3E      // �H���d�q��I2C�ڑ����^LCD���W���[���̃A�h���X
#define DS1307_ADRES 0x68
#define ACK   0
#define NOACK 1
#define W_0  0
#define R_1  1
#define LONG_HOLD_LEVEL1  5
#define LONG_HOLD_LEVEL2  100

typedef struct button {
    uint8_t cnt_sw;
    uint8_t press_flag;
    uint8_t long_hold_flag1;
    uint8_t long_hold_flag2;
} button_t;
int AckCheck;
unsigned char cnt_sw;
unsigned char press_flag;
unsigned char long_hold_flag1;
unsigned char long_hold_flag2;
unsigned char cnt_swx;
unsigned char press_flagx;
unsigned char long_hold_flag1x;
unsigned char long_hold_flag2x;
unsigned char cnt_t1;
unsigned char ss;
unsigned char mm;
unsigned char hh;
unsigned char EE;
unsigned char DD;
unsigned char MM;
unsigned char YY;

void interrupt InterFunction(void);
void I2C_IdleCheck(char mask);
void InitI2C_Master(void);
int I2C_Start(int adrs, int rw);
int I2C_rStart(int adrs, int rw);
void I2C_Stop(void);
int I2C_Send(char dt);
char I2C_Receive(int ack);
void command(unsigned char c);
void LCD_Clear(void);
void LCD_SetCursor(int col, int row);
void LCD_Putc(char c);
void LCD_Puts(const char * s);
void LCD_CreateChar(int p, char *dt);
void LCD_Init(void);

void interrupt InterFunction(void) {
    if (SSP1IF == 1) { // SSP(I2C)���荞�ݔ������H
        if (AckCheck == 1) AckCheck = 0;
        SSP1IF = 0; // �t���O�N���A
    }
    if (BCL1IF == 1) { // MSSP(I2C)�o�X�Փˊ��荞�ݔ������H
        BCL1IF = 0; // ����̓t���O�̂݃N���A����(������)
    }
    if (TMR1IF == 1) {
        TMR1IF = 0;
        cnt_t1++;

        if (RA0 == 0) {
            if (cnt_sw < LONG_HOLD_LEVEL2 + 1) cnt_sw++;
        } else {
            if (cnt_sw >= LONG_HOLD_LEVEL2)long_hold_flag2 = 1;
            else if (cnt_sw >= LONG_HOLD_LEVEL1 && cnt_sw < LONG_HOLD_LEVEL2)long_hold_flag1 = 1;
            else if (cnt_sw >= 1 && cnt_sw < LONG_HOLD_LEVEL1)press_flag = 1;
            cnt_sw = 0;
        }
        if (RA1 == 0) {
            if (cnt_swx < LONG_HOLD_LEVEL2 + 1) cnt_swx++;
        } else {
            if (cnt_swx >= LONG_HOLD_LEVEL2)long_hold_flag2x = 1;
            else if (cnt_swx >= LONG_HOLD_LEVEL1 && cnt_swx < LONG_HOLD_LEVEL2)long_hold_flag1x = 1;
            else if (cnt_swx >= 1 && cnt_swx < LONG_HOLD_LEVEL1)press_flagx = 1;
            cnt_swx = 0;
        }
    }
}

void I2C_IdleCheck(char mask) {
    while ((SSP1CON2 & 0x1F) | (SSP1STAT & mask));
}

void InitI2C_Master(void) {
    SSP1STAT = 0b10000000; // �W�����x���[�h�ɐݒ肷��(100kHz)
    SSP1CON1 = 0b00101000; // SDA/SCL�s����I2C�Ŏg�p���A�}�X�^�[���[�h�Ƃ���
    SSP1ADD = 0x13; // �N���b�N=FOSC/((SSPADD + 1)*4) 8MHz/((0x13+1)*4)=0.1(100KHz)
    SSP1IE = 1; // SSP(I2C)���荞�݂�������
    BCL1IE = 1; // MSSP(I2C)�o�X�Փˊ��荞�݂�������
    PEIE = 1; // ���ӑ��u���荞�݂�������
    GIE = 1; // �S���荞�ݏ�����������
    SSP1IF = 0; // SSP(I2C)���荞�݃t���O���N���A����
    BCL1IF = 0; // MSSP(I2C)�o�X�Փˊ��荞�݃t���O���N���A����
}

int I2C_Start(int adrs, int rw) {
    // �X�^�[�g(START CONDITION)
    I2C_IdleCheck(0x5);
    SSP1CON2bits.SEN = 1;
    // [�X���[�u�̃A�h���X]�𑗐M����
    I2C_IdleCheck(0x5);
    AckCheck = 1;
    SSP1BUF = (char) ((adrs << 1) + rw); // �A�h���X + R/W�𑗐M
    while (AckCheck); // ���肩���ACK�ԓ���҂�
    return SSP1CON2bits.ACKSTAT;
}

int I2C_rStart(int adrs, int rw) {
    // ���s�[�g�E�X�^�[�g(REPEATED START CONDITION)
    I2C_IdleCheck(0x5);
    SSP1CON2bits.RSEN = 1;
    // [�X���[�u�̃A�h���X]�𑗐M����
    I2C_IdleCheck(0x5);
    AckCheck = 1;
    SSP1BUF = (char) ((adrs << 1) + rw); // �A�h���X + R/W�𑗐M
    while (AckCheck); // ���肩���ACK�ԓ���҂�
    return SSP1CON2bits.ACKSTAT;
}

void I2C_Stop(void) {
    // �X�g�b�v(STOP CONDITION)
    I2C_IdleCheck(0x5);
    SSP1CON2bits.PEN = 1;
}

int I2C_Send(char dt) {
    I2C_IdleCheck(0x5);
    AckCheck = 1;
    SSP1BUF = dt; // �f�[�^�𑗐M
    while (AckCheck); // ���肩���ACK�ԓ���҂�
    return SSP1CON2bits.ACKSTAT;
}

char I2C_Receive(int ack) {
    char dt;

    I2C_IdleCheck(0x5);
    SSP1CON2bits.RCEN = 1; // ��M��������
    I2C_IdleCheck(0x4);
    dt = SSP1BUF; // �f�[�^�̎�M
    I2C_IdleCheck(0x5);
    SSP1CON2bits.ACKDT = ack; // ACK�f�[�^�̃Z�b�g
    SSP1CON2bits.ACKEN = 1; // ACK�f�[�^��Ԃ�
    return dt;
}

void command(unsigned char c) {
    int ans;

    ans = I2C_Start(ST7032_ADRES, W_0); // �X�^�[�g�R���f�B�V�����𔭍s����
    if (ans == 0) {
        // command word �̑��M
        I2C_Send(0b10000000); // control byte �̑��M(�R�}���h���w��)
        I2C_Send(c); // data byte �̑��M
    }
    I2C_Stop(); // �X�g�b�v�R���f�B�V�����𔭍s����
    __delay_us(26);
}

void LCD_Clear(void) {
    command(0x01); // Clear Display : ��ʑS�̂�20H�̽�߰��ŕ\���A���ق�col=0,row=0�Ɉړ�
    __delay_us(1100); // LCD������(1.08ms)����̂�҂��܂�
}

void LCD_SetCursor(int col, int row) {
    int row_offsets[] = {0x00, 0x40};

    command(0x80 | (col + row_offsets[row])); // Set DDRAM Adddress : 00H-07H,40H-47H
}

void LCD_Putc(char c) {
    int ans;

    ans = I2C_Start(ST7032_ADRES, W_0); // �X�^�[�g�R���f�B�V�����𔭍s����
    if (ans == 0) {
        // command word �̑��M
        I2C_Send(0b11000000); // control byte �̑��M(�f�[�^���w��)
        I2C_Send(c); // data byte �̑��M
    }
    I2C_Stop(); // �X�g�b�v�R���f�B�V�����𔭍s����
    __delay_us(26);
}

void LCD_Puts(const char * s) {
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

void LCD_CreateChar(int p, char *dt) {
    int ans;

    ans = I2C_Start(ST7032_ADRES, W_0); // �X�^�[�g�R���f�B�V�����𔭍s����
    if (ans == 0) {
        //  LCD�ɃL�����ۑ���̃A�h���X���w������
        I2C_Send(0b10000000); // control byte �̑��M(�R�}���h���w��)
        I2C_Send(0x40 | (p << 3));
        __delay_us(26);
        //  LCD�ɃL�����f�[�^�𑗐M����
        I2C_Send(0b01000000); // control byte �̑��M(�f�[�^���w��)
        for (int i = 0; i < 7; i++) {
            I2C_Send(*dt++);
            __delay_us(26);
        }
    }
    I2C_Stop(); // �X�g�b�v�R���f�B�V�����𔭍s����
}

void LCD_Init(void) {
    __delay_ms(40); // �d���n�m��40ms�܂ő҂��Ă��珉����
    command(0x38); // function set           : �f�[�^����8�{�E�\���͂Q�s�E�t�H���g��5x8�h�b�g
    command(0x39); // function set           : �g���R�}���h�̐ݒ��L���ɂ���
    command(0x14); // Internal OSC frequency : �o�C�A�X�̑I���Ɠ���OSC���g���̒���
    command(0x70); // Contrast set           : �R���g���X�g�����f�[�^(����4�r�b�g)
    command(0x56); // Contrast set           : ������H�L���A�R���g���X�g�����f�[�^(���2�r�b�g)
    command(0x6C); // Follower control       : �t�H���A��H��ON�A�������̒������s��
    __delay_ms(200); // �d�͂����肷��܂ő҂�
    command(0x38); // function set           : �g���R�}���h��ݒ�𖳌��ɂ���
    command(0x0C); // display control        : ��ʕ\����ON�E�J�[�\���\����OFF�E�J�[�\���_�ł�OFF
    command(0x06); // entry mode set         : ������\���������ɃJ�[�\�����ړ�������w��
    LCD_Clear(); // Clear Display          : ��ʂ���������
}

void RTC_Write(unsigned char Reg, unsigned char c) {
    int ans;

    ans = I2C_Start(DS1307_ADRES, W_0); // �X�^�[�g�R���f�B�V�����𔭍s����
    if (ans == 0) {
        // command word �̑��M
        I2C_Send(Reg); // control byte �̑��M(�R�}���h���w��)
        I2C_Send(c); // data byte �̑��M
    }
    I2C_Stop(); // �X�g�b�v�R���f�B�V�����𔭍s����
    __delay_us(26);
}

unsigned char RTC_Read(unsigned char Reg) {
    int ans;
    unsigned char data;

    ans = I2C_Start(DS1307_ADRES, W_0); // �X�^�[�g�R���f�B�V�����𔭍s����
    if (ans == 0) {
        // command word �̑��M
        I2C_Send(Reg); // control byte �̑��M(�R�}���h���w��)
    }
    ans = I2C_rStart(DS1307_ADRES, R_1); // re�X�^�[�g�R���f�B�V�����𔭍s����
    if (ans == 0) {
        // command word �̑��M
        data = I2C_Receive(1); // control byte �̑��M(�R�}���h���w��)
    }
    I2C_Stop(); // �X�g�b�v�R���f�B�V�����𔭍s����
    __delay_us(26);
    return data;
}

void main(void) {
    OSCCON = 0b01110010; // �����N���b�N�͂W�l�g���Ƃ���
    OPTION_REG = 0b00000000; // �f�W�^��I/O�ɓ����v���A�b�v��R���g�p����
    ANSELA = 0b00000000; // AN0-AN4�͎g�p���Ȃ��S�ăf�W�^��I/O�Ƃ���
    ANSELB = 0b00000000; // AN5-AN11�͎g�p���Ȃ��S�ăf�W�^��I/O�Ƃ���
    TRISA = 0b00000011; // �s��(RA)�͑S�ďo�͂Ɋ����Ă�(RA5�͓��݂͂̂ƂȂ�)
    TRISB = 0b00010010; // �s��(RB)��RB4(SCL1)/RB1(SDA1)�̂ݓ���
    WPUB = 0b00010010; // RB1/4�͓����v���A�b�v��R���w�肷��
    PORTA = 0b00000000; // RA�o�̓s���̏�����(�S��LOW�ɂ���)
    PORTB = 0b00000000; // RB�o�̓s���̏�����(�S��LOW�ɂ���)

    T1CON = 0x21; //Fosc/4, ps:1/4, 7.62Hz, 131ms
    TMR1H = 0x00;
    TMR1L = 0x00;
    PIE1bits.TMR1IE = 1;

    InitI2C_Master();

    LCD_Init();

    RTC_Write(0x07, 0x00);
    RTC_Write(0x00, 0x56);
    RTC_Write(0x01, 0x34);
    RTC_Write(0x02, 0x12);
    RTC_Write(0x03, 0x02);
    RTC_Write(0x04, 0x06);
    RTC_Write(0x05, 0x01);
    RTC_Write(0x06, 0x15);

    while (1) {
        ss = RTC_Read(0x00);
        mm = RTC_Read(0x01);
        hh = RTC_Read(0x02);
        EE = RTC_Read(0x03);
        DD = RTC_Read(0x04);
        MM = RTC_Read(0x05);
        YY = RTC_Read(0x06);
        LCD_SetCursor(0, 0); // �\���ʒu��ݒ肷��
        LCD_Putc(YY / 16 + '0');
        LCD_Putc(YY % 16 + '0');
        LCD_Putc('/');
        LCD_Putc(MM / 16 + '0');
        LCD_Putc(MM % 16 + '0');
        LCD_Putc('/');
        LCD_Putc(DD / 16 + '0');
        LCD_Putc(DD % 16 + '0');
        LCD_SetCursor(0, 1); // �\���ʒu��ݒ肷��
        LCD_Putc(hh / 16 + '0');
        LCD_Putc(hh % 16 + '0');
        LCD_Putc(':');
        LCD_Putc(mm / 16 + '0');
        LCD_Putc(mm % 16 + '0');
        LCD_Putc('-');
        LCD_Putc(ss / 16 + '0');
        LCD_Putc(ss % 16 + '0');
        if (press_flag) {
            press_flag = 0;
            while (!press_flag) {
                if (cnt_t1 % 3 == 0) {
                    LCD_SetCursor(0, 0); // �\���ʒu��ݒ肷��
                    LCD_Puts("  ");
                } else {
                    YY = RTC_Read(0x06);
                    LCD_SetCursor(0, 0); // �\���ʒu��ݒ肷��
                    LCD_Putc(YY / 16 + '0');
                    LCD_Putc(YY % 16 + '0');
                }
                if (press_flagx) {
                    press_flagx = 0;
                    YY = RTC_Read(0x06);
                    if (YY % 16 == 9) YY -= 9;
                    else YY++;
                    RTC_Write(0x06, YY);
                }
                if (long_hold_flag1x) {
                    long_hold_flag1x = 0;
                    YY = RTC_Read(0x06);
                    if (YY / 16 == 9) YY -= 16 * 9;
                    else YY += 16;
                    RTC_Write(0x06, YY);
                }
            }
            YY = RTC_Read(0x06);
            LCD_SetCursor(0, 0); // �\���ʒu��ݒ肷��
            LCD_Putc(YY / 16 + '0');
            LCD_Putc(YY % 16 + '0');
            press_flag = 0;
            while (!press_flag) {
                if (cnt_t1 % 3 == 0) {
                    LCD_SetCursor(3, 0); // �\���ʒu��ݒ肷��
                    LCD_Puts("  ");
                } else {
                    MM = RTC_Read(0x05);
                    LCD_SetCursor(3, 0); // �\���ʒu��ݒ肷��
                    LCD_Putc(MM / 16 + '0');
                    LCD_Putc(MM % 16 + '0');
                }
                if (press_flagx) {
                    press_flagx = 0;
                    MM = RTC_Read(0x05);
                    if (MM % 16 == 9) MM -= 9;
                    else MM++;
                    RTC_Write(0x06, MM);
                }
                if (long_hold_flag1x) {
                    long_hold_flag1x = 0;
                    MM = RTC_Read(0x05);
                    if (MM / 16 == 9) MM -= 16 * 9;
                    else MM += 16;
                    RTC_Write(0x05, MM);
                }
            }
            press_flag = 0;
            MM = RTC_Read(0x05);
            LCD_SetCursor(3, 0); // �\���ʒu��ݒ肷��
            LCD_Putc(MM / 16 + '0');
            LCD_Putc(MM % 16 + '0');

            while (!press_flag) {
                if (cnt_t1 % 3 == 0) {
                    LCD_SetCursor(6, 0); // �\���ʒu��ݒ肷��
                    LCD_Puts("  ");
                } else {
                    DD = RTC_Read(0x04);
                    LCD_SetCursor(6, 0); // �\���ʒu��ݒ肷��
                    LCD_Putc(DD / 16 + '0');
                    LCD_Putc(DD % 16 + '0');
                }
                if (press_flagx) {
                    press_flagx = 0;
                    DD = RTC_Read(0x04);
                    if (DD % 16 == 9) DD -= 9;
                    else DD++;
                    RTC_Write(0x04, DD);
                }
                if (long_hold_flag1x) {
                    long_hold_flag1x = 0;
                    DD = RTC_Read(0x04);
                    if (DD / 16 == 9) DD -= 16 * 9;
                    else DD += 16;
                    RTC_Write(0x04, DD);
                }
            }
            press_flag = 0;
            DD = RTC_Read(0x04);
            LCD_SetCursor(6, 0); // �\���ʒu��ݒ肷��
            LCD_Putc(DD / 16 + '0');
            LCD_Putc(DD % 16 + '0');
        }
    }
}
