#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Set up the default buttons based on the feather being used.
#if defined(ESP8266)
#define BUTTON_A 0
#define BUTTON_B 16
#define BUTTON_C 2
#define LED      0
#elif defined(ESP32)
#define BUTTON_A 15
#define BUTTON_B 32
#define BUTTON_C 14
#define LED      13
#elif defined(ARDUINO_STM32F2_FEATHER)
#define BUTTON_A PA15
#define BUTTON_B PC7
#define BUTTON_C PC5
#define LED      PB5
#elif defined(TEENSYDUINO)
#define BUTTON_A 4
#define BUTTON_B 3
#define BUTTON_C 8
#define LED      13
#elif defined(ARDUINO_FEATHER52)
#define BUTTON_A 31
#define BUTTON_B 30
#define BUTTON_C 27
#define LED      17
#else // 32u4, M0, and 328p
#define BUTTON_A 9
#define BUTTON_B 6
#define BUTTON_C 5
#define LED      13
#endif

// The delay to wait between button presses.
#define BUTTON_DELAY 100

// Create the main display instance.
Adafruit_SSD1306 display = Adafruit_SSD1306();

// set up scores
int blue = 0;
int red = 0;

void setup() {
    // Give the display a second to start up.
    delay(1000);

    // Show image buffer on the display hardware (the Adafruit splash screen)
    display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
    display.setTextWrap(false);
    display.display();
    delay(2500);
    display.clearDisplay();

    // Set up pins on the display.
    pinMode(BUTTON_A, INPUT_PULLUP);
    pinMode(BUTTON_B, INPUT_PULLUP);
    pinMode(BUTTON_C, INPUT_PULLUP);

    // Initial text display.
    display.setTextSize(2);
    display.setTextColor(WHITE, BLACK);
    display.setCursor(0, 0);
    display.println("A  Red: ");
    display.println("B Blue: ");
    display.display();
}

void displayScores() {
    display.setCursor(0, 0);
    display.print("A  Red: ");
    display.print(red);
    display.println("   ");
    display.print("B Blue: ");
    display.print(blue);
    display.println("   ");
    display.display();
}

void resetDisplay() {
    display.clearDisplay();
    display.setCursor(0, 0);
    display.setTextSize(2);
    display.display();
}

bool hasSkunk() {
    if (red == 0 && blue >= 11) {
        return true;
    } else if (blue == 0 && red >= 11) {
        return true;
    }

    return false;
}

void displaySkunk() {
    display.println("SKUNK!");
    displayWin();
}

bool hasWin() {
    if (red >= 21 && red > (blue + 1)) {
        return true;
    } else if (blue >= 21 && blue > (red + 1)) {
        return true;
    }

    return false;
}

void displayWin() {
    if (red < blue) {
        display.print("Blue ");
    } else {
        display.print("Red ");
    }
    display.println("Wins!");
    display.display();
}

void loop() {
    bool doDelay = false;

    // Button C is a reset.
    if (LOW == digitalRead(BUTTON_C)) {
        red = 0;
        blue = 0;
        display.setTextSize(2);
    } else {
        // Read the state of the buttons.
        if (LOW == digitalRead(BUTTON_A)) {
            red++;
            doDelay = true;
        } else if (LOW == digitalRead(BUTTON_B)) {
            blue++;
            doDelay = true;
        }
    }

    if (hasSkunk()) {
        resetDisplay();
        displaySkunk();
    } else if (hasWin()) {
        resetDisplay();
        displayWin();
    } else {
        displayScores();
    }

    if (doDelay) {
        delay(BUTTON_DELAY);
    }
}
