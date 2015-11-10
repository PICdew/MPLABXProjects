/* 
 * File:   My_RTCC.h
 * Author: Ryotaro Onuki
 *
 * Created on 2015/03/19, 0:02
 * 
 * PIC18F28J53�p���A���^�C���N���b�N�v���O����
 * ���T�v
 * ���̃v���O�����ł́A�����������ϐ����R����B
 * 1.PIC��RTCC���W���[���̃��W�X�^���J�����_�[�\�L�A16�i�@�\�L�i�������ɂ�10�i�����֋X��16�i�Ƃ���B�j
 * 2.time_t�\���̓���epoch�iuint32_t�j��2000�N1��1��0��00������̑��b��
 * 3.time_t�\���̓���YY,MM,DD,EE,hh,mm,ss�����ꂼ��A�N�A���A���A�j���A���A���A�b��10�i�@�\�L�B
 * �ȏ�R������B�������ς���Ă�RTCC_loop()�֐������ׂĂ̒l����Ɉ�v�����Ă���̂ň��S���Ďg�p�ł���B
 * ���g����
 * 1.TIOSC�̃s����32.768kHz�̃N���X�^�����Ȃ��B�i12p�̃R���f���T���j
 * 2.CONFIG��ݒ肷��BRTCOSC = T1OSCREF
 * 3.���̎��ɁARTCC_init();�������B�i�����ݒ�֐��j
 * 4.main���[�v���ɁARTCC_loop();���������ƁB�i���Ԃ̓����֐��j
 * 5.�C�ӂŎ��v���킹������B�����v���킹��������ARTCC_from_caltime()��RTCC_from_epoch()���Ă�ŕύX��K�p����B
 * 6.���v���킹�ɂ́Aadjust_time_toggle()���Ă�ł���Aadjust_time_cursor()�ŁA�D���ȍ���I������
 * 7.adjust_time_inc()��adjust_time_dec()�Œ�������Ƃ悢�B
 * 8.���v���킹���I�������Aadjust_time_toggle()��������x�ĂԂƎ��v���킹�I���B
 */

#ifndef MY_RTCC_H
#define	MY_RTCC_H

#include <xc.h>
#include <stdint.h>
#include "My_button.h"

// epoch�^���`�B2000�N1��1��0��00������̑��b���B

typedef uint32_t epoch_t;

// �������킹�p�̃t���O�̍\���̂��`�B

typedef struct {

    union {
        uint8_t flags;

        struct {
            uint8_t ss : 1;
            uint8_t mm : 1;
            uint8_t hh : 1;
            uint8_t DD : 1;
            uint8_t MM : 1;
            uint8_t YY : 1;
        } flag;
    };
} edit_t;

// RTCC�֘A���ׂĂ��l�߂��\���́B���ۂɎg�p����B

typedef struct {
    epoch_t epoch;
    uint8_t ss;
    uint8_t mm;
    uint8_t hh;
    uint8_t EE;
    uint8_t DD;
    uint8_t MM;
    uint8_t YY;
    uint8_t colon;
    uint8_t halfsec;
    edit_t edit;
} time_t;

// �\���p�̗j��char��const�Œ�`�B
extern const char weekday_3char[7][4];

// ���ݎ���
extern time_t now;
// ���Ԃ��ς�����������t���O���o�B�\����̎����̍X�V�͂��̃t���O�̊Ď������čs���΂悢�B
extern uint8_t time_change_flag;
// main_init()�ɏ������ƁB
void RTCC_init(void);
/********************************** necessary functions **********************************/
// from decimal to hex
uint8_t d_to_x(uint8_t dec);
// from hex to decimal
uint8_t x_to_d(uint8_t hex);
// return month length
uint8_t month_length(uint8_t year, uint8_t month);
// quot��div�Ŋ����āA�]���Ԃ��B�悭�g���̂Ŋ֐��������B
epoch_t get_quot_rem(epoch_t *quot, uint8_t div);
/********************************** Transform time **********************************/
// RTCC���J�����_�^�C���ɕϊ��A�㏑���B
void RTCC_to_caltime(time_t *tm);
// �J�����_�^�C����RTCC�ɕϊ��A�㏑���B
void caltime_to_RTCC(time_t *tm);
// Epoch���J�����_�^�C���ɕϊ��A�㏑���B
void epoch_to_caltime(time_t *tm);
// �J�����_�^�C�����G�|�b�N�ɕϊ��A�㏑���B
void caltime_to_epoch(time_t *tm);
/********************************** Transform time User Functions **********************************/
// ������RTCC��ł��낦��
void RTCC_from_RTCC(time_t *tm);
// �������J�����_�[�^�C����ł��낦��
void RTCC_from_caltime(time_t *tm);
// �������G�|�b�N��ł��낦��
void RTCC_from_epoch(time_t *tm);
/********************************** LCD display **********************************/
// �e���ڕ\���p�֐��B���[�U�[�͎g��Ȃ��B
void display_dec(char *str, uint8_t dec, uint8_t edit);
// 0802�T�C�Y�t���p�A������쐬�֐�
void display_time_0802(time_t *tm, char *line0, char *line1);
// 1608�T�C�Y�t���p�A������쐬�֐�
void display_time_1602(time_t *tm, char *line0, char *line1);
/********************************** adjust the time **********************************/
// �����ҏW���[�h��؂�ւ���
void RTCC_adjust_time_toggle(time_t *tm);
// �ҏW���̎����j��؂�ւ���
void RTCC_adjust_time_cursor(time_t *tm);
// �ҏW���̎����j���P���₷
void RTCC_adjust_time_inc(time_t *tm);
// �ҏW���̎����j���P���炷
void RTCC_adjust_time_dec(time_t *tm);
// 3�̃{�^���Ŏ������킹
void RTCC_adjust_time_button(time_t *tm, button_t *mode, button_t *cnt_inc, button_t *cnt_dec);
// Time sync
void RTCC_loop(void);

#endif	/* MY_RTCC_H */
