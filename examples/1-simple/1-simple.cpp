#include "Particle.h"

#include "StatusLedRK.h"

SYSTEM_THREAD(ENABLED);
SYSTEM_MODE(SEMI_AUTOMATIC);

SerialLogHandler logHandler(LOG_LEVEL_TRACE);

StatusLedRGB::LedPins ledPins[1] = {
    { WKP, D3, D2 }, // red, green, blue
};
size_t ledPinsCount = sizeof(ledPins) / sizeof(ledPins[0]);

StatusLedRGB statusLed(ledPinsCount, ledPins, true /* isCommonAnode */);


void setup() {
    statusLed.setup();
    statusLed.setColor(0, StatusLed::COLOR_GREEN);
    
    Particle.connect();
}

void loop() {
    statusLed.loop();
}
