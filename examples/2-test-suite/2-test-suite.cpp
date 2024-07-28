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

typedef struct {
    uint32_t color;
    const char *name;
} ColorPair;

ColorPair colorPairs[] = {
	{ StatusLed::COLOR_BLACK, "black" },
	{ StatusLed::COLOR_WHITE, "white" },
	{ StatusLed::COLOR_RED, "red" },
	{ StatusLed::COLOR_LIME, "lime" },
	{ StatusLed::COLOR_BLUE, "blue" },
	{ StatusLed::COLOR_YELLOW, "yellow" },
	{ StatusLed::COLOR_CYAN, "cyan" },
	{ StatusLed::COLOR_MAGENTA, "magenta" },
	{ StatusLed::COLOR_SILVER, "silver" },
	{ StatusLed::COLOR_GRAY, "gray" },
	{ StatusLed::COLOR_MAROON, "maroon" },
	{ StatusLed::COLOR_OLIVE, "olive" },
	{ StatusLed::COLOR_GREEN, "green" },
	{ StatusLed::COLOR_PURPLE, "purple" },
	{ StatusLed::COLOR_TEAL, "teal" },
	{ StatusLed::COLOR_NAVY, "navy" },
};
int colorPairsCount = sizeof(colorPairs) / sizeof(colorPairs[0]);

unsigned long testTime = 0;
unsigned long testDuration = 0;
bool testRunNext = true;
bool testRunAfterDelay = false;

typedef struct {
    const char *name;
    std::function<void()> handler;
} Test;

Test tests[] = {
    {
        "solid colors",
        []() {
            static int colorIndex = 0;

            Log.info("setColor %s", colorPairs[colorIndex].name);
            statusLed.setColor(0, colorPairs[colorIndex].color);
            if (++colorIndex >= colorPairsCount) {
                colorIndex = 0;
                testRunAfterDelay = true;        
            }
            testTime = millis();
            testDuration = 4000;
        },
    },
    {
        "test slow blinking red",
        []() {
            statusLed.setColorStyle(0, StatusLed::COLOR_RED, StatusLed::STYLE_BLINK_SLOW);
            testRunAfterDelay = true;        
            testTime = millis();
            testDuration = 6000;
        }
    },
    {
        "test fast blinking green",
        []() {
            statusLed.setColorStyle(0, StatusLed::COLOR_GREEN, StatusLed::STYLE_BLINK_FAST);
            testRunAfterDelay = true;        
            testTime = millis();
            testDuration = 6000;
        }
    },
    {
        "test green",
        []() {
            statusLed.setColorStyle(0, StatusLed::COLOR_GREEN, StatusLed::STYLE_ON);
            testRunAfterDelay = true;        
            testTime = millis();
            testDuration = 2000;
        }
    },
    {
        "blinking red override",
        []() {
            statusLed.setOverrideStyle(0, StatusLed::COLOR_RED, StatusLed::STYLE_BLINK_SLOW, 4000);
            testRunAfterDelay = true;        
            testTime = millis();
            testDuration = 8000;
        }
    },
    {
        "tests complete!",
        []() {
            if (testDuration == 0) {
                statusLed.setColorStyle(0, StatusLed::COLOR_BLACK, StatusLed::STYLE_ON);
                testDuration = 10000;
            }
            else {
                testRunNext = true;
            }
        }
    },
};
int testsCount = sizeof(tests) / sizeof(tests[0]);
int testsIndex = -1;

void setup() {
    statusLed.setup();
    
    // Particle.connect();
}

void loop() {
    statusLed.loop();

    if (testRunNext) {
        testRunNext = false;

        if (++testsIndex >= testsCount) {
            testsIndex = 0;
        }
        Log.trace("Running test %s (%d of %d)...", tests[testsIndex].name, (testsIndex + 1), testsCount);
        testTime = millis();
        testDuration = 0;
    }
    if (testDuration == 0 || millis() - testTime > testDuration) {
        if (testRunAfterDelay) {
            testRunAfterDelay = false;
            testRunNext = true;
        }
        else 
        if (testsIndex >= 0) {
            tests[testsIndex].handler();
        }
    }
}
