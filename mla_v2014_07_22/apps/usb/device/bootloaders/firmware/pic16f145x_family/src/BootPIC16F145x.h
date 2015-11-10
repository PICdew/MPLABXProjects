/*********************************************************************
 * FileName:        BootPIC16F145x.h
 * Dependencies:    See INCLUDES section below
 * Processor:       PIC16F145x devices
 * Compiler:        XC8 v1.21+
 * Company:         Microchip Technology, Inc.
 *
 * Software License Agreement
 *
 * The software supplied herewith by Microchip Technology Incorporated
 * (the "Company") for its PIC(R) Microcontroller is intended and
 * supplied to you, the Company's customer, for use solely and
 * exclusively on Microchip PIC Microcontroller products. The
 * software is owned by the Company and/or its supplier, and is
 * protected under applicable copyright laws. All rights are reserved.
 * Any use in violation of the foregoing restrictions may subject the
 * user to criminal sanctions under applicable laws, as well as to
 * civil liability for the breach of the terms and conditions of this
 * license.
 *
 * THIS SOFTWARE IS PROVIDED IN AN "AS IS" CONDITION. NO WARRANTIES,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED
 * TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE COMPANY SHALL NOT,
 * IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR
 * CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 *
 ********************************************************************/
#ifndef BOOTPIC16F145x_H
#define BOOTPIC16F145x_H

/** P U B L I C  P R O T O T Y P E S *****************************************/
void UserInit(void);
void ProcessIO(void);
void ClearWatchdog(void);
void DisableUSBandExecuteLongDelay(void);


//Constants

/*
 * There is a likelihood that Flash Signature Word and/or App_Version Word may conflict with application interrupt code (code generated by XC8 compiler). The best
 * solution to avoid this conflict is to relocate Flash Signature Word and App_Version word to some other location (preferably where there will no likelihood of any
 * interrupt code being present). Therefore the Flash Signature Word and App_Version Word are now relocated to address 0x900 and 0x902, respectively and application
 * reset address is moved further down to 0x904. This will ensure that the application interrupts begin at address 0x90C and will not conflict with Signature or Version
 * words.
 */

/*APP_SPACE_RESET_VECTOR represents address where application reset vector resides.
 * APP_SPACE_START_ADDRESS is the first program memory address occupied by the application firmware. This is also the address where Flash Signature Word is placed.
 * APP_VERSION_ADDRESS contains application image firmware version number (Major(8-bit).Minor(8-bit).
 */

#define APP_SPACE_RESET_VECTOR           0x904  //This is the first program memory address occupied by the application firmware (and represents the border between the application space and bootloader space)
#define APP_SPACE_START_ADDRESS          0x900  //This is the first program memory address occupied by the application firmware (and represents the border between the application space and bootloader space)
#define APP_SIGNATURE_ADDRESS            0x900 // This is the program memory address that contains the "signature" word (14-bit instruction word), indicating successful erase/program/verify operation.  Note: 14-bit WORD at 0x906 is 0x34[APP_SIGNATURE_VALUE].  The 0x34 is for the opcode for RETLW (needed since a ROM* dereference of prog mem on XC8 will insert a RETLW instruction).
#define APP_SIGNATURE_VALUE              0x6D   //0x6D = "GOOD", implying that the erase/program was a success and the bootloader intentionally programmed the APP_SIGNATURE_ADDRESS with this value
#define APP_VERSION_ADDRESS              0x902 //0x902  //0x902 + 0x903 should contain the application image firmware version number [Major(8bit).Minor(8-bit)]

//Derived constants
#define APP_SPACE_START_HI_BYTE                 (APP_SPACE_RESET_VECTOR >> 8)
#define APP_SPACE_START_LOWER_11BITS            (APP_SPACE_RESET_VECTOR & 0x7FF)
#define APP_SPACE_REMAP_INT_VECTOR_LO_11BITS    (APP_SPACE_START_LOWER_11BITS + 0x04)

//Other constants
#define RETLW_OPCODE_MSB         0x34
#define BLANK_FLASH_WORD_VALUE   0x3FFF

#endif //BOOTPIC16F145x_H