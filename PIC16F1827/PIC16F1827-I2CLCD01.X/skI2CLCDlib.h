// skI2CLCDlib.h
// �h�Q�b�ڑ��k�b�c�֐����C�u�����p�C���N���[�h�t�@�C��
#ifndef _SKI2CLCDLIB_H_
#define _SKI2CLCDLIB_H_

#ifndef _XTAL_FREQ
 // Unless already defined assume 8MHz system frequency
 // This definition is required to calibrate __delay_us() and __delay_ms()
 #define _XTAL_FREQ 8000000    // �g�p����PIC���ɂ�蓮����g���l��ݒ肷��
#endif
#define ST7032_ADRES 0x3E      // �H���d�q��I2C�ڑ����^LCD���W���[���̃A�h���X

void LCD_Clear(void) ;
void LCD_SetCursor(int col, int row) ;
void LCD_Putc(char c) ;
void LCD_Puts(const char * s) ;
void LCD_CreateChar(int p,char *dt) ;
void LCD_Init() ;

#endif
