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
#define CONFIG_DELAY 500
#define RESET_DELAY 1000

// Data structure for config options
struct Config {
    bool skunk;
    int score;
};

// Create the main display instance.
Adafruit_SSD1306 display = Adafruit_SSD1306();

// set up scores
int black = 0;
int white = 0;

// Config options.
bool doConfig = false;
Config configOptions = {false, 21};

// Timing for events.
unsigned long previousTime = 0;

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
    display.println("Black: ");
    display.println("White: ");
    display.display();
}

void displayScores() {
    display.setCursor(0, 0);
    display.print("Black: ");
    display.print(black);
    display.println("   ");
    display.print("White: ");
    display.print(white);
    display.println("   ");
    display.display();
}

void displayConfig() {
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.println("***CONFIG***");

    display.print("Skunk:   ");
    if (configOptions.skunk) {
        display.println("ON  ");
    } else {
        display.println("OFF ");
    }

    display.print("Play to: ");
    display.println(configOptions.score);

    display.display();
}

void resetDisplay() {
    display.clearDisplay();
    display.setCursor(0, 0);
    display.setTextSize(2);
    display.display();
}

bool hasSkunk() {
    if (!configOptions.skunk) {
        return false;
    }

    if (white == 0 && black >= 11) {
        return true;
    } else if (black == 0 && white >= 11) {
        return true;
    }

    return false;
}

void displaySkunk() {
    display.println("SKUNK!");
    displayWin();
}

bool hasWin() {
    if (white >= configOptions.score && white > (black + 1)) {
        return true;
    } else if (black >= configOptions.score && black > (white + 1)) {
        return true;
    }

    return false;
}

void displayWin() {
    if (white < black) {
        display.print("Black ");
    } else {
        display.print("White ");
    }
    display.println("Wins!");
    display.display();
}

void configLoop() {
    bool doDelay = false;

    // Check the button states
    if (LOW == digitalRead(BUTTON_C)) {
        delay(BUTTON_DELAY);
        if (LOW == digitalRead(BUTTON_C)) {
            doConfig = false;
            resetDisplay();
        }
    } else {
        if (LOW == digitalRead(BUTTON_A)) {
            if (configOptions.skunk) {
                configOptions.skunk = false;
            } else {
                configOptions.skunk = true;
            }
        } else if (LOW == digitalRead(BUTTON_B)) {
            // Values go 21 -> 30 -> 50
            if (21 == configOptions.score) {
                configOptions.score = 30;
            } else if (30 == configOptions.score) {
                configOptions.score = 50;
            } else {
                configOptions.score = 21;
            }
        }

        doDelay = true;
    }

    displayConfig();

    if (doDelay) {
      delay(BUTTON_DELAY);
    }
}


function resetScore() {
  white = 0;
  black = 0;
}

void scoreLoop() {
    bool doDelay = false;

    // Button C is a reset.
    if (LOW == digitalRead(BUTTON_C)) {
        resetScore();
        display.setTextSize(2);
    } else {
        // Read the state of the buttons.
        if (LOW == digitalRead(BUTTON_A)) {
            black++;
            doDelay = true;
        } else if (LOW == digitalRead(BUTTON_B)) {
            white++;
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

void handleButtonC() {
    unsigned long currentTime = millis();
    unsigned long difference = 0;

    // Check for button C being held down.
    if (LOW != digitalRead(BUTTON_C)) {
        previousTime = currentTime;
    }
}

void loop() {
    handleButtonC();

    if (doConfig) {
        configLoop();
    } else {
        scoreLoop();
    }
}
