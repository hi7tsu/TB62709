#include "TB62709.h"

TB62709::TB62709(uint8_t pinDIN, uint8_t pinCLK, uint8_t pinLOAD)
    : _str_lo{ CHAR_BLANK, CHAR_BLANK, CHAR_L, CHAR_LOWER_SQUARE },
      _str_hi{ CHAR_BLANK, CHAR_BLANK, CHAR_H, CHAR_1 }
{
    _pDIN = pinDIN;
    _pCLK = pinCLK;
    _pLOAD = pinLOAD;
    digitalWrite(_pDIN, LOW);
    digitalWrite(_pCLK, LOW);
    digitalWrite(_pLOAD, LOW);
    pinMode(_pDIN, OUTPUT);
    pinMode(_pCLK, OUTPUT);
    pinMode(_pLOAD, OUTPUT);
}

void TB62709::send(uint16_t value)
{
    // CLK  Max16MHz (H:30ns / L:30ns)
    // LOAD H(Min):100ns
    // DATA is loaded at CLK rising edge
    // DATA is MSB first.

    // Even if pin is set to OUTPUT and LOW, the internal pin value will 
    // be HIGH once INPUT-PULLUP is set. So you must write LOW each time
    // pin mode is switched to OUTPUT.
    
    for (uint8_t i = 0; i < 16; i++)
    {
        pinMode(_pCLK, OUTPUT);
        digitalWrite(_pCLK, LOW);
        if (value & 0x8000){
            pinMode(_pDIN, INPUT_PULLUP);
        }
        else{
            pinMode(_pDIN, OUTPUT);
            digitalWrite(_pDIN, LOW);
        }
        CLK_DELAY
        pinMode(_pCLK, INPUT_PULLUP);
        CLK_DELAY
        value <<= 1;
    }
    pinMode(_pCLK, OUTPUT);
    digitalWrite(_pCLK, LOW);
    pinMode(_pLOAD, INPUT_PULLUP);
    CLK_DELAY
    CLK_DELAY
    CLK_DELAY
    pinMode(_pLOAD, OUTPUT);
    digitalWrite(_pLOAD, LOW);
}

void TB62709::setBrightness(uint8_t brightness)
{
    // 0 to 15
    // 0 is duty 0% (turn off)
    send(LOAD_DUTY_REG | (brightness & 0xF));
}

void TB62709::displayOn()
{
    send(MODE_NORMAL);
}

void TB62709::displayOff()
{
    send(MODE_BLANK);
}

void TB62709::standby(bool clearData)
{
    send(MODE_STANDBY | (clearData ? STANDBY_CLEAR_DATA : STANDBY_KEEP_DATA));
}

void TB62709::config(bool enableDecoder, uint8_t activateSegmentCount)
{
    uint16_t value = LOAD_DEC_DIG_REG;
    value |= enableDecoder ? ENABLE_DECODE : DISABLE_DECODE;
    switch (activateSegmentCount)
    {
    case 1:
        value |= ACTIVATE_DIG_0;
        break;
        
    case 2:
        value |= ACTIVATE_DIG_0_1;
        break;
        
    case 3:
        value |= ACTIVATE_DIG_0_2;
        break;

    default:
    case 4:
        value |= ACTIVATE_DIG_0_3;
        break;
    }
    send(value);
}

void TB62709::displayRawAt(uint8_t digit, uint8_t value)
{
    switch (digit)
    {
    case 0:
        send(LOAD_DATA_D0_REG | value);
        return;
        
    case 1:
        send(LOAD_DATA_D1_REG | value);
        return;

    case 2:
        send(LOAD_DATA_D2_REG | value);
        return;

    case 3:
        send(LOAD_DATA_D3_REG | value);
        return;
    
    default:
        return;
    }
}


void TB62709::displayAt(uint8_t digit, uint8_t value, bool dp)
{
    uint8_t data = value & 0x3F;
    if (dp) data |= DP_ON;
    displayRawAt(digit, data);
}

void TB62709::displayBlank()
{
    displayAt(0, CHAR_BLANK, false);
    displayAt(1, CHAR_BLANK, false);
    displayAt(2, CHAR_BLANK, false);
    displayAt(3, CHAR_BLANK, false);
}

void TB62709::_display(int16_t value, uint8_t zeroFillPos, uint8_t dpPos)
{
    if (value < -999)
    {
        displayChars(_str_lo, 4);
        return;
    }
    else if (value > 9999)
    {
        displayChars(_str_hi, 4);
        return;
    }
    
    uint8_t maxDigit = 4;
    if (value < 0) 
    {
        value = -value;
        if (zeroFillPos > 3) zeroFillPos = 3;
        maxDigit = 3;
        displayAt(3, CHAR_MINUS, dpPos == 3);
    }
    
    for (uint8_t i = 0; i < maxDigit; i++)
    {
        bool dp = dpPos == i;
        if (value == 0)
        {
            if (i <= zeroFillPos)
            {
                displayAt(i, 0, dp);
            }
            else
            {
                displayAt(i, CHAR_BLANK, dp);
            }
        }
        else
        {
            uint8_t num = value % 10;
            displayAt(i, num, dp);
            value /= 10;
        }
    }
}

void TB62709::display(int16_t value)
{
    _display(value, 0, 0xFF);
}

void TB62709::displayWithZeroPadding(int16_t value)
{
    _display(value, 3, 0xFF);
}

void TB62709::display(float value, uint8_t dpPos)
{
    int16_t ival;
    uint8_t zpos = dpPos;
    bool autodp = true;
    
    if (value < -999) value = -1000;
    else if (value > 9999) value = 10000;
    
    if (dpPos == 0)
    {
        ival = (int16_t)value;
        autodp = false;
    }
    else if (dpPos == 1)
    {
        if (-100 < value && value < 1000)
        {
            ival = (int16_t)(value * 10);
            autodp = false;
        }
    }
    else if (dpPos == 2)
    {
        if (-10 < value && value < 100)
        {
            ival = (int16_t)(value * 100);
            autodp = false;
        }
    }
    else if (dpPos == 3)
    {
        if (-1 < value && value < 10)
        {
            ival = (int16_t)(value * 1000);
            autodp = false;
        }
    }
    
    if (autodp)
    {
        // auto
        zpos = 0;
        if (value <= -100 || 1000 <= value)
        {
            // 1234
            // -123
            ival = (int16_t)value;
            dpPos = 0xFF; 
        }
        else if (value <= -10 || 100 <= value)
        {
            // 123.4
            // -12.3
            ival = (int16_t)(value * 10);
            dpPos = 1; 
        }
        else if (value <= -1 || 10 <= value)
        {
            // 12.34
            // -1.23
            ival = (int16_t)(value * 100);
            dpPos = 2; 
        }
        else if (0 <= value && value < 10)
        {
            // 1.234
            // 0.123
            // 0.000
            ival = (int16_t)(value * 1000);
            dpPos = 3;
            zpos = 3;
        }
        else if (-0.010f < value && value < 0)
        {
            // -.009
            ival = (int16_t)(value * 1000);
            dpPos = 3;
            zpos = 2;
        }
        else
        {
            // -1 < value <= -0.01
            // -0.12
            ival = (int16_t)(value * 100);
            dpPos = 2;
            zpos = 2;
        }
    }
    _display(ival, zpos, dpPos);
}

void TB62709::displayWithDp(int16_t value, uint8_t dpPos)
{
    _display(value, dpPos, dpPos);
}

void TB62709::displayHex(uint16_t value, bool zeroPadding)
{
    uint8_t num;
    for (uint8_t i = 0; i < 4; i++)
    {
        if (value == 0)
        {
            if (i == 0 || zeroPadding)
            {
                displayAt(i, 0);
            }
            else
            {
                displayAt(i, CHAR_BLANK);
            }
        }
        else
        {
            num = value & 0xF;
            displayAt(i, num);
            value >>= 4;
        }
    }
}

void TB62709::displayChars(const uint8_t* chars, uint8_t len)
{
    uint8_t* cptr = (uint8_t *)chars + len - 1;
    for (uint8_t i = 0; i < 4; i++)
    {
        if (cptr < chars) 
        {
            displayAt(i, CHAR_BLANK);
        }
        else 
        {
            uint8_t c = *cptr & 0x3F;
            displayAt(i, c);
        }
        cptr--;
    }
}
