#include "Particle.h"

#include "StatusLedRK.h"

SYSTEM_THREAD(ENABLED);
SYSTEM_MODE(SEMI_AUTOMATIC);

SerialLogHandler logHandler(LOG_LEVEL_TRACE);

StatusLedRK_RGB::LedPins ledPins[1] = {
    { WKP, D3, D2 }, // red, green, blue
};
size_t ledPinsCount = sizeof(ledPins) / sizeof(ledPins[0]);

StatusLedRK_RGB statusLed(ledPinsCount, ledPins, true /* isCommonAnode */);

typedef struct {
    uint32_t color;
    const char *name;
} ColorPair;

ColorPair colorPairs[] = {
	{ StatusLedRK::COLOR_BLACK, "black" },
	{ StatusLedRK::COLOR_WHITE, "white" },
	{ StatusLedRK::COLOR_RED, "red" },
	{ StatusLedRK::COLOR_LIME, "lime" },
	{ StatusLedRK::COLOR_BLUE, "blue" },
	{ StatusLedRK::COLOR_YELLOW, "yellow" },
	{ StatusLedRK::COLOR_CYAN, "cyan" },
	{ StatusLedRK::COLOR_MAGENTA, "magenta" },
	{ StatusLedRK::COLOR_SILVER, "silver" },
	{ StatusLedRK::COLOR_GRAY, "gray" },
	{ StatusLedRK::COLOR_MAROON, "maroon" },
	{ StatusLedRK::COLOR_OLIVE, "olive" },
	{ StatusLedRK::COLOR_GREEN, "green" },
	{ StatusLedRK::COLOR_PURPLE, "purple" },
	{ StatusLedRK::COLOR_TEAL, "teal" },
	{ StatusLedRK::COLOR_NAVY, "navy" },
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

            Log.trace("setColor %s", colorPairs[colorIndex].name);
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
            statusLed.setColorStyle(0, StatusLedRK::COLOR_RED, StatusLedRK::STYLE_BLINK_SLOW);
            testRunAfterDelay = true;        
            testTime = millis();
            testDuration = 6000;
        }
    },
    {
        "test fast blinking green",
        []() {
            statusLed.setColorStyle(0, StatusLedRK::COLOR_GREEN, StatusLedRK::STYLE_BLINK_FAST);
            testRunAfterDelay = true;        
            testTime = millis();
            testDuration = 6000;
        }
    },
    {
        "test green",
        []() {
            statusLed.setColorStyle(0, StatusLedRK::COLOR_GREEN, StatusLedRK::STYLE_ON);
            testRunAfterDelay = true;        
            testTime = millis();
            testDuration = 4000;
        }
    },
    {
        "blinking red override",
        []() {
            statusLed.setOverrideStyle(0, StatusLedRK::COLOR_RED, StatusLedRK::STYLE_BLINK_SLOW, 6000);
            testRunAfterDelay = true;        
            testTime = millis();
            testDuration = 6000;
        }
    },
    {
        "should revert to green",
        []() {
            testRunAfterDelay = true;        
            testTime = millis();
            testDuration = 4000;
        }
    },
    {
        "tests complete!",
        []() {
            if (testDuration == 0) {
                statusLed.setColorStyle(0, StatusLedRK::COLOR_BLACK, StatusLedRK::STYLE_ON);
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
