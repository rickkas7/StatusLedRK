#include "StatusLedRK.h"


StatusLed::StatusLed(size_t numPixels) : numPixels(numPixels) {
}

StatusLed::~StatusLed() {
    if (state) {
        delete[] state;
    }
    if (overrides) {
        delete[] overrides;
    }
}

void StatusLed::setup() {
    state = new PixelState[numPixels];
    overrides = new LedOverride[numPixels];

    for(size_t ii = 0; ii < numPixels; ii++) {
        state[ii].color = COLOR_BLACK;
        state[ii].style = STYLE_ON;
        state[ii].lastTime = 0;
        state[ii].blinkState = false;

        overrides[ii].timeMs = 0; // inactive
    }

    setup2();
}


void StatusLed::loop() {
    bool doShow = false;

    if (loopCheckEnabled) {
        // We have either blinking or overrides so we may need to update the NeoPixels

        if (checkForOverrideChange()) {
            // An override expired so we need to restore the original color
            doShow = true;
        }

        for(size_t ii = 0; ii < numPixels; ii++) {
            PixelState *pixelState;

            if (overrides[ii].timeMs > 0) {
                pixelState = &overrides[ii].state;
            }
            else {
                pixelState = &state[ii];
            }
            
            if (pixelState->style == STYLE_BLINK_SLOW) {
                if (millis() - pixelState->lastTime >= SLOW_BLINK_MS) {
                    pixelState->lastTime = millis();
                    pixelState->blinkState = !pixelState->blinkState;
                    doShow = true;
                }
            }
            else
            if (pixelState->style == STYLE_BLINK_FAST) {
                if (millis() - pixelState->lastTime >= FAST_BLINK_MS) {
                    pixelState->lastTime = millis();
                    pixelState->blinkState = !pixelState->blinkState;
                    doShow = true;
                }
            }
        }
    }

    if (doShow) {
        show();
    }
}


uint32_t StatusLed::getColorWithOverride(uint16_t n)  {

    PixelState *curState = &state[n];

    if (overrides[n].timeMs != 0) {
        // An override is in effect, use that state instead
        curState = &overrides[n].state;
    }

    if (curState->style == STYLE_ON) {
        // Just on, use the specified color
        return curState->color;
    }
    else
    if (curState->blinkState) {
        // Blinking and in the on part of blinking, use the specified color
        return curState->color;
    }
    else {
        // Blinking and in the off part of blinking, use off (black)
        return COLOR_BLACK;
    }
}

void StatusLed::setColor(uint16_t n, uint32_t color, bool showNow) {
    setColorStyle(n, color, STYLE_ON, showNow);
}

void StatusLed::setColorStyle(uint16_t n, uint32_t color, uint8_t style, bool showNow) {
    state[n].color = color;
    state[n].style = style;

    if (overrides[n].timeMs > 0) {
        // There is an override
        if (overrides[n].clearOnChange) {
            // clearOnChange is set, and we just changed the underlying color, so clear the override
            overrides[n].timeMs = 0;
        }
    }

    if (showNow) {
        show();
    }

    updateLoopCheckEnabled();
}


void StatusLed::setOverrideStyle(uint16_t n, uint32_t color, uint8_t style, unsigned long howLong, bool clearOnChange) {
    overrides[n].startMillis = millis();
    overrides[n].timeMs = howLong;
    overrides[n].state.color = color;
    overrides[n].state.style = style;
    overrides[n].state.lastTime = 0;
    overrides[n].state.blinkState = false;
    overrides[n].clearOnChange = clearOnChange;

    updateLoopCheckEnabled();

    show();

}

bool StatusLed::checkForOverrideChange() {
    bool overrideChange = false;

    for(size_t ii = 0; ii < numPixels; ii++) {
        if (overrides[ii].timeMs > 0) {
            // Check to see if the override has expired
            if (millis() - overrides[ii].startMillis >= overrides[ii].timeMs) {
                // Yes, expired
                overrides[ii].timeMs = 0;
                overrideChange = true;
            }
        }
    }

    return overrideChange;
}

void StatusLed::updateLoopCheckEnabled() {
    loopCheckEnabled = false;

    for(size_t ii = 0; ii < numPixels; ii++) {
        if (overrides[ii].timeMs > 0) {
            // There is an override, loop check is required
            loopCheckEnabled = true;
        }
        else
        if (state[ii].style != STYLE_ON) {
            // Blinking is enabled, so a loop check is required
            loopCheckEnabled = true;
        }
    }
}


StatusLedRGB::StatusLedRGB(size_t numPixels, const LedPins *pinsArray, bool isCommonAnode) : StatusLed(numPixels), pinsArray(pinsArray), isCommonAnode(isCommonAnode) {
}

StatusLedRGB::~StatusLedRGB() {
}

void StatusLedRGB::setup2() {

    for(size_t ii = 0; ii < numPixels; ii++) {
        pinMode(pinsArray[ii].rPin, OUTPUT);
        pinMode(pinsArray[ii].gPin, OUTPUT);
        pinMode(pinsArray[ii].bPin, OUTPUT);
    }

}


void StatusLedRGB::show()  {
    for(uint16_t ii = 0; ii < numPixels; ii++) {
        uint32_t color = getColorWithOverride(ii);
        uint8_t red = (uint8_t) (color >> 16);
        uint8_t green = (uint8_t) (color >> 8);
        uint8_t blue = (uint8_t) color;

        if (isCommonAnode) {
            red = 255 - red;
            green = 255 - green;
            blue = 255 - blue;
        }

        analogWrite(pinsArray[ii].rPin, red);
        analogWrite(pinsArray[ii].gPin, green);
        analogWrite(pinsArray[ii].bPin, blue);
    }
}
