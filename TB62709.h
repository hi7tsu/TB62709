#pragma once

#include "Arduino.h"

// to be below 16MHz
#ifdef F_CPU
#if F_CPU <= 32000000L
#define CLK_DELAY   asm volatile("nop\n\t");
#elif F_CPU <= 64000000L
#define CLK_DELAY   asm volatile("nop\n\tnop\n\t");
#elif F_CPU <= 96000000L
#define CLK_DELAY   asm volatile("nop\n\tnop\n\tnop\n\t");
#elif F_CPU <= 128000000L
#define CLK_DELAY   asm volatile(".rept 4\n\tnop\n\t.endr");
#elif F_CPU <= 160000000L
#define CLK_DELAY   asm volatile(".rept 5\n\tnop\n\t.endr");
#elif F_CPU <= 192000000L
#define CLK_DELAY   asm volatile(".rept 6\n\tnop\n\t.endr");
#elif F_CPU <= 256000000L
#define CLK_DELAY   asm volatile(".rept 8\n\tnop\n\t.endr");
#elif F_CPU <= 512000000L
#define CLK_DELAY   asm volatile(".rept 16\n\tnop\n\t.endr");
#elif F_CPU <= 1024000000L
#define CLK_DELAY   asm volatile(".rept 32\n\tnop\n\t.endr");
#else
#define CLK_DELAY   delayMicroseconds(1);
#endif
#else
#define CLK_DELAY   delayMicroseconds(1);
#endif 

#define MODE_BLANK          0x0000
#define MODE_NORMAL         0x1000
//#define MODE_LOADREG        0x2000
#define MODE_ALLON          0x3000
#define MODE_STANDBY        0x4000

#define LOAD_DUTY_REG       0x2000
#define LOAD_DEC_DIG_REG    0x2100
#define LOAD_DATA_D0_REG    0x2200
#define LOAD_DATA_D1_REG    0x2300
#define LOAD_DATA_D2_REG    0x2400
#define LOAD_DATA_D3_REG    0x2500

#define ACTIVATE_DIG_0      0
#define ACTIVATE_DIG_0_1    1
#define ACTIVATE_DIG_0_2    2
#define ACTIVATE_DIG_0_3    3

#define DISABLE_DECODE      0
#define ENABLE_DECODE       16

#define STANDBY_KEEP_DATA   0
#define STANDBY_CLEAR_DATA  1

#define CHAR_0              0
#define CHAR_1              1
#define CHAR_2              2
#define CHAR_3              3
#define CHAR_4              4
#define CHAR_5              5
#define CHAR_6              6
#define CHAR_7              7
#define CHAR_8              8
#define CHAR_9              9
#define CHAR_A              10
#define CHAR_B              11
#define CHAR_C              12
#define CHAR_D              13
#define CHAR_E              14
#define CHAR_F              15
#define CHAR_BLANK          16
#define CHAR_H              17
#define CHAR_J              18
#define CHAR_L              19
#define CHAR_P              20
#define CHAR_Q              21
#define CHAR_R              22
#define CHAR_S              23
#define CHAR_U              24
#define CHAR_Y              25
#define CHAR_MINUS          26
#define CHAR_UPPER_SQUARE   27
#define CHAR_LOWER_SQUARE   28
#define CHAR_CLOSING_BRACKET 29
#define CHAR_HORIZONTAL_T   30
#define DP_OFF              0
#define DP_ON               64


class TB62709
{
public:
    TB62709(uint8_t pinDIN, uint8_t pinCLK, uint8_t pinLOAD);
    void send(uint16_t value);
    void setBrightness(uint8_t brightness);
    void displayOn();
    void displayOff();
    void standby(bool clearData = false);
    void config(bool enableDecoder, uint8_t activateSegmentCount = 4);
    void displayRawAt(uint8_t digit, uint8_t value);
    void displayAt(uint8_t digit, uint8_t value, bool dp = false);
    void displayBlank();
    void display(int16_t value);
    void displayWithZeroPadding(int16_t value);
    void display(float value, uint8_t dpPos = 0xFF);
    void displayWithDp(int16_t value, uint8_t dpPos);
    void displayHex(uint16_t value, bool zeroPadding = false);
    void displayChars(const uint8_t* chars, uint8_t len);

private:
    uint8_t _pDIN;
    uint8_t _pCLK;
    uint8_t _pLOAD;
    const uint8_t _str_lo[4];
    const uint8_t _str_hi[4];
    void _display(int16_t value, uint8_t zeroFillPos, uint8_t dpPos);
};
