#include "TB62709.h"

TB62709 disp(2, 3, 4);

void setup() {
    disp.displayOn();

    // chars (use decoder)
    const uint8_t chars[] = 
        { CHAR_BLANK, CHAR_BLANK, CHAR_BLANK, CHAR_H, CHAR_E, CHAR_L, CHAR_L, CHAR_0, CHAR_BLANK, CHAR_BLANK, CHAR_BLANK };

    for (uint8_t i = 0; i <= sizeof(chars) - 4; i++) {
        disp.displayChars(chars + i, 4);
        delay(200);
    }

    // (Off)
    disp.displayOff();
    disp.standby();
    delay(1000);
    disp.displayBlank();
    disp.displayOn();
}

void loop() {
    // integar
    for (uint8_t i = 1; i < 10; i++) {
        disp.display(i);
        delay(1000);
    }

    // hex
    for (uint8_t i = 0; i <= 5; i++) {
        disp.displayHex(0xFFFA + i);
        delay(1000);
    }
    // > change brightness
    for (uint8_t i = 0; i < 32; i ++) {
        disp.setBrightness(i & 0xF);
        delay(200);
    }

    // (Off)
    disp.displayOff();
    disp.standby();
    delay(1000);
    disp.displayBlank();
    disp.displayOn();

    // float auto
    disp.display(-0.001f);
    delay(1000);
    disp.display(-0.012f);
    delay(1000);
    disp.display(-0.123f);
    delay(1000);
    disp.display(-1.234f);
    delay(1000);
    disp.display(-12.34f);
    delay(1000);
    disp.display(-123.4f);
    delay(1000);
    disp.display(-1234.f);
    delay(1000);

    // float fix point
    disp.display(0.001f, 2);
    delay(1000);
    disp.display(0.012f, 2);
    delay(1000);
    disp.display(0.123f, 2);
    delay(1000);
    disp.display(1.234f, 2);
    delay(1000);
    disp.display(12.34f, 2);
    delay(1000);
    disp.display(123.4f, 2);
    delay(1000);
    disp.display(1234.f, 2);
    delay(1000);
    disp.display(12345.f, 2);
    delay(1000);

    // (Off)
    disp.displayOff();
    disp.standby();
    delay(1000);
    disp.displayBlank();
    disp.displayOn();

    // raw mode (= no decode)
    disp.config(false);
    uint8_t raw = 1;
    for (uint8_t i = 0; i < 8; i++) {
        disp.displayRawAt(0, raw);
        disp.displayRawAt(1, raw);
        disp.displayRawAt(2, raw);
        disp.displayRawAt(3, raw);
        delay(250);
        raw <<= 1;
    }

    // (off)
    disp.displayOff();
    disp.displayBlank();
    disp.config(true);
    disp.standby();
    delay(2000);
    disp.displayOn();
}
