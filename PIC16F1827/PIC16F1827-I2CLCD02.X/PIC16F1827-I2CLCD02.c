#include <xc.h>
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

void interrupt InterFunction(void);
void I2C_IdleCheck(char mask);
void InterI2C(void);
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
int AckCheck;
char heart[7] = {
    0b01010,
    0b11011,
    0b11111,
    0b11111,
    0b01110,
    0b00100,
    0b00000,
};

/*******************************************************************************
 *  InterFunction()   ���荞�݂̏���                                            *
 *******************************************************************************/
void interrupt InterFunction(void) {
    // �h�Q�b�֘A�̊��荞�ݏ���
    InterI2C();
}
/*******************************************************************************
 *  skI2Clib - �h�Q�b�֐����C�u����(PIC12F/16F18xx�V���[�Y�p)                   *
 *             ���̃��C�u������I2C�f�o�C�X(RTC/EEPROM��)�Ɛڑ����s���ׂ̊֐��W  *
 *                                                                              *
 *    InterI2C       - �h�Q�b�֘A�̊��荞�ݏ���                                 *
 *    InitI2C_Master - �h�Q�b�ʐM�̃}�X�^�[���[�h�ŏ��������s������             *
 *    I2C_Start      - �X���[�u�ɃX�^�[�g�R���f�B�V�����𔭍s���鏈��           *
 *    I2C_rStart     - �X���[�u�Ƀ��s�[�g�E�X�^�[�g�R���f�B�V�����𔭍s���鏈�� *
 *    I2C_Stop       - �X���[�u�ɃX�g�b�v�R���f�B�V�����𔭍s���鏈��           *
 *    I2C_Send       - �X���[�u�Ƀf�[�^���P�o�C�g���M���鏈��                   *
 *    I2C_Receive    - �X���[�u����f�[�^���P�o�C�g��M���鏈��                 *
 *                                                                              *
 *    �����FSDA/SCL�s���͕K���u�f�W�^�����̓s���v�ɐݒ���s���ĉ������B         *
 *          �ʐM�N���b�N��100KHz(CPU8MHz)�ł̏������ł��B                       *
 * ============================================================================ *
 *  VERSION DATE        BY                    CHANGE/COMMENT                    *
 * ---------------------------------------------------------------------------- *
 *  1.00    2012-01-20  ���ޒ��H�[(���ނ���)  Create                            *
 *  1.01    2013-02-16  ���ޒ��H�[(���ނ���)  XC8 C Compiler �Ή��ɕύX         *
 * ============================================================================ *
 *  PIC 12F1822 16F18xx (16F1827��SSP2���p�s��)(16F19xx�͗��p�s��)              *
 *  MPLAB IDE(V8.84)                                                            *
 *  MPLAB(R) XC8 C Compiler Version 1.00                                        *
 *******************************************************************************/
// �A�C�h����Ԃ̃`�F�b�N
// ACKEN RCEN PEN RSEN SEN R/W BF ���S�ĂO�Ȃ�n�j

void I2C_IdleCheck(char mask) {
    while ((SSP1CON2 & 0x1F) | (SSP1STAT & mask));
}

/*******************************************************************************
 *  InterI2C( void )                                                            *
 *    �h�Q�b�֘A�̊��荞�ݏ���                                                  *
 *     ���̊֐��̓��C���v���O�����̊����݊֐��ŌĂт܂�                         *
 *******************************************************************************/
void InterI2C(void) {
    if (SSP1IF == 1) { // SSP(I2C)���荞�ݔ������H
        if (AckCheck == 1) AckCheck = 0;
        SSP1IF = 0; // �t���O�N���A
    }
    if (BCL1IF == 1) { // MSSP(I2C)�o�X�Փˊ��荞�ݔ������H
        BCL1IF = 0; // ����̓t���O�̂݃N���A����(������)
    }
}

/*******************************************************************************
 *  InitI2C_Master()                                                            *
 *    �h�Q�b�ʐM�̃}�X�^�[���[�h�ŏ��������s������                              *
 *                                                                              *
 *    ��)�N���b�N8MHz�ł̐ݒ�ł��A���̃N���b�N��SSP1ADD��ύX����K�v���L��܂�*
 *******************************************************************************/
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

/*******************************************************************************
 *  ans = I2C_Start(adrs,rw)                                                    *
 *    �X���[�u�ɃX�^�[�g�R���f�B�V�����𔭍s���鏈��                            *
 *                                                                              *
 *    adrs : �X���[�u�̃A�h���X���w�肵�܂�                                     *
 *    rw   : �X���[�u�ɑ΂��铮��̎w������܂�                                 *
 *           0=�X���[�u�ɏ����݂Ȃ����v���@1=�X���[�u�ɑ��M���Ȃ����v��         *
 *    ans  : 0=����@1=�ُ�(���肩��ACK���Ԃ��Ă��Ȃ�)                          *
 *******************************************************************************/
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

/*******************************************************************************
 *  ans = I2C_rStart(adrs,rw)                                                   *
 *    �X���[�u�Ƀ��s�[�g�E�X�^�[�g�R���f�B�V�����𔭍s���鏈��                  *
 *                                                                              *
 *    adrs : �X���[�u�̃A�h���X���w�肵�܂�                                     *
 *    rw   : �X���[�u�ɑ΂��铮��̎w������܂�                                 *
 *           0=�X���[�u�ɏ����݂Ȃ����v���@1=�X���[�u�ɑ��M���Ȃ����v��         *
 *    ans  : 0=����@1:�ُ�(���肩��ACK���Ԃ��Ă��Ȃ�)                          *
 *******************************************************************************/
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

/*******************************************************************************
 *  I2C_Stop()                                                                  *
 *    �X���[�u�ɃX�g�b�v�R���f�B�V�����𔭍s���鏈��                            *
 *******************************************************************************/
void I2C_Stop(void) {
    // �X�g�b�v(STOP CONDITION)
    I2C_IdleCheck(0x5);
    SSP1CON2bits.PEN = 1;
}

/*******************************************************************************
 *  ans = I2C_Send(dt)                                                          *
 *    �X���[�u�Ƀf�[�^���P�o�C�g���M���鏈��                                    *
 *                                                                              *
 *    dt  : ���M����f�[�^���w�肵�܂�                                          *
 *    ans  : 0=����@1=�ُ�(���肩��ACK���Ԃ��Ă��Ȃ�����NOACK��Ԃ���)         *
 *******************************************************************************/
int I2C_Send(char dt) {
    I2C_IdleCheck(0x5);
    AckCheck = 1;
    SSP1BUF = dt; // �f�[�^�𑗐M
    while (AckCheck); // ���肩���ACK�ԓ���҂�
    return SSP1CON2bits.ACKSTAT;
}

/*******************************************************************************
 *  ans = I2C_Receive(ack)                                                      *
 *    �X���[�u����f�[�^���P�o�C�g��M���鏈��                                  *
 *                                                                              *
 *    ack  : �X���[�u�ւ̕ԓ��f�[�^���w�肵�܂�                                 *
 *           0:ACK��Ԃ��@1:NOACK��Ԃ�(��M�f�[�^���Ō�Ȃ�1)                  *
 *    ans  : ��M�����f�[�^��Ԃ�                                               *
 *******************************************************************************/
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
/*******************************************************************************
 *  skI2CLCDlib - �h�Q�b�ڑ��k�b�c�֐����C�u����                                *
 *             ���̃��C�u�����́A�H���d�q�h�Q�b�ڑ����^�k�b�c���W���[���p�ł��B *
 *                                                                              *
 *    LCD_Init      - �k�b�c�̏��������s������                                  *
 *    LCD_Clear     - �k�b�c���W���[���̉�ʂ���������                          *
 *    LCD_SetCursor - �k�b�c���W���[����ʓ��̃J�[�\���ʒu���ړ����鏈��        *
 *    LCD_Putc      - �k�b�c�Ƀf�[�^���P�o�C�g�o�͂��鏈��                      *
 *    LCD_Puts      - �k�b�c�ɕ�����f�[�^���o�͂��鏈��                        *
 *    LCD_CreateChar - �I���W�i���̃L�����N�^��o�^���܂�                       *
 *                                                                              *
 *    �����F__delay_us() and __delay_ms() ���g�p���Ă���̂� "skI2CLCDlib.h" �� *
 *         "#define _XTAL_FREQ 8000000"���L�q����Ă��܂��A                     *
 *         8MHz�ȊO��CPU�N���b�N�ɂ���l�͏��������܂��傤�B                    *
 * ============================================================================ *
 *  VERSION DATE        BY                    CHANGE/COMMENT                    *
 * ---------------------------------------------------------------------------- *
 *  1.00    2013-07-25  ���ޒ��H�[(���ނ���)  Create                            *
 * ============================================================================ *
 *  PIC 12F1822 16F1827 (���̃��C�u�������̂͑��̂o�h�b�ł��n�j�Ǝv���܂�)      *
 *  MPLAB IDE(V8.84)                                                            *
 *  MPLAB(R) XC8 C Compiler Version 1.00                                        *
 *******************************************************************************/
// �k�b�c�ɃR�}���h�𔭍s���鏈��

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

/*******************************************************************************
 *  LCD_Clear( )                                                                *
 *    �k�b�c���W���[���̉�ʂ���������                                          *
 *******************************************************************************/
void LCD_Clear(void) {
    command(0x01); // Clear Display : ��ʑS�̂�20H�̽�߰��ŕ\���A���ق�col=0,row=0�Ɉړ�
    __delay_us(1100); // LCD������(1.08ms)����̂�҂��܂�
}

/*******************************************************************************
 *  LCD_SetCursor(col,row)                                                      *
 *    �k�b�c���W���[����ʓ��̃J�[�\���ʒu���ړ����鏈��                        *
 *                                                                              *
 *    col : ��(��)�����̃J�[�\���ʒu(0-7)                                       *
 *    row : �c(�s)�����̃J�[�\���ʒu(0-1)                                       *
 *******************************************************************************/
void LCD_SetCursor(int col, int row) {
    int row_offsets[] = {0x00, 0x40};

    command(0x80 | (col + row_offsets[row])); // Set DDRAM Adddress : 00H-07H,40H-47H
}

/*******************************************************************************
 *  LCD_Putc(c)                                                                 *
 *    ������́ANULL(0x00)�܂ŌJ�Ԃ��o�͂��܂��B                                *
 *                                                                              *
 *    c :  �o�͂��镶���f�[�^���w��                                             *
 *******************************************************************************/
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

/*******************************************************************************
 *  LCD_Puts(*s)                                                                *
 *    �k�b�c�ɕ�����f�[�^���o�͂��鏈��                                        *
 *    ������́ANULL(0x00)�܂ŌJ�Ԃ��o�͂��܂��B                                *
 *                                                                              *
 *    *s :  �o�͂��镶����̃f�[�^���i�[�����ꏊ�̃A�h���X���w��                *
 *******************************************************************************/
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

/*******************************************************************************
 *  LCD_CreateChar(p,*dt)                                                       *
 *    �I���W�i���̃L�����N�^��o�^���܂�                                        *
 *                                                                              *
 *    p   : �o�^����ꏊ�̎w��(�O�`�T�̂U�����̂�)                              *
 *    *dt : �o�^�������L�����N�^�̃f�[�^���i�[�����o�b�t�@���w��                *
 ********************************************************************************/
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

/*******************************************************************************
 *  LCD_Init( )                                                                 *
 *    �k�b�c�̏��������s������                                                  *
 *******************************************************************************/
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

void RTC_write(unsigned char Reg, unsigned char c) {
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

unsigned char RTC_read(unsigned char Reg, unsigned char c) {
    int ans;
    unsigned char data;

    ans = I2C_Start(DS1307_ADRES, W_0); // �X�^�[�g�R���f�B�V�����𔭍s����
    if (ans == 0) {
        // command word �̑��M
        I2C_Send(Reg); // control byte �̑��M(�R�}���h���w��)
    }
    ans = I2C_Start(DS1307_ADRES, R_1); // �X�^�[�g�R���f�B�V�����𔭍s����
    if (ans == 0) {
        // command word �̑��M
        data = I2C_Receive(1); // control byte �̑��M(�R�}���h���w��)
    }
    I2C_Stop(); // �X�g�b�v�R���f�B�V�����𔭍s����
    __delay_us(26);
    return data;
}

/*******************************************************************************
 *  ���C���̏���                                                                *
 *******************************************************************************/
void main(void) {
    OSCCON = 0b01110010; // �����N���b�N�͂W�l�g���Ƃ���
    OPTION_REG = 0b00000000; // �f�W�^��I/O�ɓ����v���A�b�v��R���g�p����
    ANSELA = 0b00000000; // AN0-AN4�͎g�p���Ȃ��S�ăf�W�^��I/O�Ƃ���
    ANSELB = 0b00000000; // AN5-AN11�͎g�p���Ȃ��S�ăf�W�^��I/O�Ƃ���
    TRISA = 0b00000000; // �s��(RA)�͑S�ďo�͂Ɋ����Ă�(RA5�͓��݂͂̂ƂȂ�)
    TRISB = 0b00010010; // �s��(RB)��RB4(SCL1)/RB1(SDA1)�̂ݓ���
    WPUB = 0b00010010; // RB1/4�͓����v���A�b�v��R���w�肷��
    PORTA = 0b00000000; // RA�o�̓s���̏�����(�S��LOW�ɂ���)
    PORTB = 0b00000000; // RB�o�̓s���̏�����(�S��LOW�ɂ���)

    InitI2C_Master(); // �h�Q�b�̏���������

    // �k�b�c���W���[���̏���������
    LCD_Init();

    LCD_CreateChar(0x00, heart); // 1�ԂɃI���W�i���L�����N�^��o�^����

    LCD_SetCursor(0, 0); // �\���ʒu��ݒ肷��
    LCD_Puts("Hello");
    //LCD_Putc(0x00) ;            // 1�Ԃɓo�^�����L������\������
    LCD_SetCursor(2, 1); // �\���ʒu��ݒ肷��
    LCD_Puts("World!");

    while (1) {
    }

}
