#ifndef __STATUSLED_RK_H
#define __STATUSLED_RK_H


#include "Particle.h"


/**
 * @brief Class for managing one or more status LEDs
 * 
 * This works with LEDs connected by various technologies by using different subclasses.
 * For example: LEDs connected to PWM pins and NeoPixels.
 */
class StatusLed {
public:
    /**
     * @brief Structure that keeps the state of a pixel.
     */
    typedef struct {
        uint32_t color; //!< Color of the pixel
        uint8_t style; //!< Style (on, blink slow, blink fast). See STYLE_ constants below.
        uint32_t lastTime; //!< Last state change millis(), used when blinking.
        bool blinkState; //!< Whether the blink is on (true) or off (false)
    } PixelState;

    /**
     * @brief Structure used to temporarily override the status LED color
     */
    typedef struct {
        PixelState state; //!< The state of the LED override (on, blinking, and color)
        unsigned long startMillis; //!< When the override started (millis value)
        unsigned long timeMs; //!< Duration of the override in milliseconds
        bool clearOnChange; //!< If true, if the color of the pixel is set while overridden, the override is removed.
    } LedOverride;


public:
    /**
     * @brief Constructor of the base class
     * 
     * @param numPixels 
     * 
     * You can't instantiate one of these directly as it's pure virtual. You must instantiate one 
     * of the concrete subclasses like StatusLedRGB instead.
     */
    StatusLed(size_t numPixels);
    virtual ~StatusLed();

    /**
     * @brief This must be called from application setup().
     */
    virtual void setup();

    /**
     * @brief This must be called from application loop().
     * 
     * The LEDs are only updated during loop(), so you should call this as frequently as possible.
     * It's efficient so it won't update the hardware when the pixels do not change.
     */
	virtual void loop();

    /**
     * @brief Update the hardware to match the current pixel state
     * 
     * This is only called when a pixel changes color (not continuously). This method
     * is pure virtual and must be overridden in a subclass.
     */
    virtual void show() = 0;

    /**
     * @brief Subclasses can override this to do setup
     * 
     * Because setup() does a number of things that are required by subclasses, the
     * subclasses should override setup2() instead of setup() to avoid disrupting the
     * required calls in setup().
     * 
     * For example, StatusLedRGB uses setup2() to set the pinMode of the RGB pins.
     */
    virtual void setup2() {};

    /**
     * @brief Get the color of the specified pixel, taking into account overrides.
     * 
     * @param n Pixel number (0 is the first pixel)
     * @return uint32_t RGB color. See constants like COLOR_RED, below.
     */
	uint32_t getColorWithOverride(uint16_t n);

    /**
     * @brief Set the color of a pixel (solid color, not blinking)
     * 
     * @param n Pixel number (0 is the first pixel)
     * @param color RGB color (uint32_t). See constants like COLOR_RED, below.
     * @param showNow true to show the pixel immediately, or false to wait
     * 
     * If you are using a NeoPixel string, it may make sense to pass false to showNow and
     * call show() later if you are updating multiple pixels at the same time, for example.
     */
	void setColor(uint16_t n, uint32_t color, bool showNow = true);

    /**
     * @brief Set the color and style of a pixel. This can be used for solid or blinking colors.
     * 
     * @param n Pixel number (0 is the first pixel)
     * @param color RGB color (uint32_t). See constants like COLOR_RED, below.
     * @param style One of STYLE_ON, STYLE_BLINK_SLOW, STYLE_BLINK_FAST.
     * @param showNow true to show the pixel immediately, or false to wait
     * 
     * If you are using a NeoPixel string, it may make sense to pass false to showNow and
     * call show() later if you are updating multiple pixels at the same time, for example.
     */
	void setColorStyle(uint16_t n, uint32_t color, uint8_t style, bool showNow = true);

    /**
     * @brief Temporarily override the color and style of a pixel
     * 
     * @param n Pixel number (0 is the first pixel)
     * @param color RGB color (uint32_t). See constants like COLOR_RED, below.
     * @param style One of STYLE_ON, STYLE_BLINK_SLOW, STYLE_BLINK_FAST.
     * @param howLong How long to override the color in milliseconds.
     * @param clearOnChange If true and the color is set using setColor or setColorStyle, the override is removed.
     */
	void setOverrideStyle(uint16_t n, uint32_t color, uint8_t style, unsigned long howLong, bool clearOnChange = true);

	static const uint32_t COLOR_BLACK = 0x000000;	// (0,0,0)
	static const uint32_t COLOR_WHITE = 0xFFFFFF;	// (255,255,255)
	static const uint32_t COLOR_RED = 0xFF0000;	// (255,0,0)
	static const uint32_t COLOR_LIME = 0x00FF00;	// (0,255,0)
	static const uint32_t COLOR_BLUE = 0x0000FF;	// (0,0,255)
	static const uint32_t COLOR_YELLOW = 0xFFFF00;	// (255,255,0)
	static const uint32_t COLOR_CYAN  = 0x00FFFF;	// (0,255,255)
	static const uint32_t COLOR_MAGENTA  = 0xFF00FF;	// (255,0,255)
	static const uint32_t COLOR_SILVER = 0xC0C0C0;	// (192,192,192)
	static const uint32_t COLOR_GRAY = 0x808080;	// (128,128,128)
	static const uint32_t COLOR_MAROON = 0x800000;	// (128,0,0)
	static const uint32_t COLOR_OLIVE = 0x808000;	// (128,128,0)
	static const uint32_t COLOR_GREEN = 0x008000;	// (0,128,0)
	static const uint32_t COLOR_PURPLE = 0x800080;	// (128,0,128)
	static const uint32_t COLOR_TEAL = 0x008080;	// (0,128,128)
	static const uint32_t COLOR_NAVY = 0x000080;	// (0,0,128)

	static const uint8_t STYLE_ON = 0;
	static const uint8_t STYLE_BLINK_SLOW = 1;
	static const uint8_t STYLE_BLINK_FAST = 2;

    static const unsigned long FAST_BLINK_MS = 250;
    static const unsigned long SLOW_BLINK_MS = 1000;


protected:
    /**
     * @brief Used internally to see if there is an override change and the pixels need to be updated
     * 
     * @return true 
     * @return false 
     */
	bool checkForOverrideChange();

    /**
     * @brief Used internally to update the loopCheckEnabled flag.
     * 
     */
	void updateLoopCheckEnabled();

    /**
     * This class cannot be copied
     */
    StatusLed(const StatusLed&) = delete;

    /**
     * This class cannot be copied
     */
    StatusLed& operator=(const StatusLed&) = delete;

protected:
    size_t numPixels = 0; //!< Number of pixels, set during construction.
	PixelState *state = 0; //!< Array of PixelState structures, one per pixel. Allocated during setup().
	LedOverride *overrides = 0; //!< Array of LedOverride structures, one per pixels. Allocated during setup().
	bool loopCheckEnabled = false; //!< Internal flag used to determine whether the pixels should be checked on calls to loop.
};

/**
 * @brief Class for status LED connected to PWM pins
 */
class StatusLedRGB : public StatusLed {
public:
    /**
     * @brief Structure that defines the pins used for red, green, and blue of an RGB LED
     */
    typedef struct {
        pin_t rPin; //!< Red pin
        pin_t gPin; //!< Green pin
        pin_t bPin; //!< Blue pin
    } LedPins;


    /**
     * @brief Construct a new Status Led object
     * 
     * @param numPixels Number of entries in pinsArray
     * @param pinsArray The pinsArray is typically a const global structure. This is not copied! 
     * @param isCommonAnode true for common anode (common pin connected to VCC) or false for common cathode (common pin connected to GND)
     */
	StatusLedRGB(size_t numPixels, const LedPins *pinsArray, bool isCommonAnode = true);

    /**
     * @brief Destructor
     */
	virtual ~StatusLedRGB();

    /**
     * @brief Sets the pinMode of the LED pins. Called from setup().
     */
	virtual void setup2();

    /**
     * @brief Update the RGB LED to match the state in the StatusLed object
     */
	virtual void show();


protected:
    const LedPins *pinsArray; //!< Definition of the GPIO used for the RGB LED PWM pins
    bool isCommonAnode = true;  //!< true for common anode (common pin connected to VCC) or false for common cathode (common pin connected to GND)
};


#endif // __STATUSLED_RK_H