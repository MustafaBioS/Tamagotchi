#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

const int SW1 = 2;
const int SW2 = 3;
const int SW3 = 4;

const int SDA_PIN = 6;
const int SCL_PIN = 7;

const int BUZZER_PIN = 10;

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define OLED_RESET -1

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

struct Pet {
  int hunger;
  int happiness;
  int energy;
  int anger;
  unsigned long age;
};

Pet pet;

enum Screen {
  SCREEN_MAIN,
  SCREEN_FEED,
  SCREEN_PLAY,
  SCREEN_SLEEP
};

Screen currentScreen = SCREEN_MAIN;

unsigned long lastUpdate = 0;
unsigned long lastButtonPress = 0;

void setup() {
  // put your setup code here, to run once:
  pinMode(SW1, INPUT_PULLUP);
  pinMode(SW2, INPUT_PULLUP);
  pinMode(SW3, INPUT_PULLUP);
  pinMode(BUZZER_PIN, OUTPUT);

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("Tamagotchi Initialize");
  display.display();
  delay(1000);

  pet.hunger = 80;
  pet.happiness = 80;
  pet.energy = 80;
  pet.anger = 0;
  pet.sadness = 0;
  pet.age = 0;

}

void loop() {
  // put your main code here, to run repeatedly:
  checkButtons();
  updatePet();
  handleScreenLogic();
  render();
  delay(100);
}

const unsigned char PROGMEM petHappy[] = {
  0b00000000, 0b00000000,
  0b00001111, 0b11110000,
  0b00010000, 0b00001000,
  0b00100000, 0b00000100,
  0b00100110, 0b01100100,
  0b00100110, 0b01100100,
  0b00100110, 0b01100100,
  0b00100110, 0b01100100,
  0b00100000, 0b00000100,
  0b00100111, 0b11100100,
  0b00100111, 0b11100100,
  0b00100011, 0b11000100,
  0b00100000, 0b00000100,
  0b00010000, 0b00001000,
  0b00001111, 0b11110000,
  0b00000000, 0b00000000
};

const unsigned char PROGMEM petSad[] = {
  0b00000000, 0b00000000,
  0b00001111, 0b11110000,
  0b00010000, 0b00001000,
  0b00100000, 0b00000100,
  0b00100110, 0b01100100,
  0b00100110, 0b01100100,
  0b00100110, 0b01100100,
  0b00100110, 0b01100100,
  0b00100000, 0b00000100,
  0b00100011, 0b11000100,
  0b00100110, 0b01100100,
  0b00100100, 0b00100100,
  0b00100000, 0b00000100,
  0b00010000, 0b00001000,
  0b00001111, 0b11110000,
  0b00000000, 0b00000000
};

const unsigned char PROGMEM petNeutral[] = {
  0b00000000, 0b00000000,
  0b00001111, 0b11110000,
  0b00010000, 0b00001000,
  0b00100000, 0b00000100,
  0b00100110, 0b01100100,
  0b00100110, 0b01100100,
  0b00100110, 0b01100100,
  0b00100110, 0b01100100,
  0b00100000, 0b00000100,
  0b00100000, 0b00000100,
  0b00100111, 0b11100100,
  0b00100111, 0b11100100,
  0b00100000, 0b00000100,
  0b00010000, 0b00001000,
  0b00001111, 0b11110000,
  0b00000000, 0b00000000
};

const unsigned char PROGMEM petSleep[] = {
  0b00000000, 0b00000000,
  0b00001111, 0b11110000,
  0b00010000, 0b00001000,
  0b00100000, 0b00000100,
  0b00100000, 0b00000100,
  0b00101110, 0b01110100,
  0b00101110, 0b01110100,
  0b00100000, 0b00000100,
  0b00100000, 0b00000100,
  0b00100000, 0b00000100,
  0b00100111, 0b11100100,
  0b00100111, 0b11100100,
  0b00100000, 0b00000100,
  0b00010000, 0b00001000,
  0b00001111, 0b11110000,
  0b00000000, 0b00000000
};

const unsigned char PROGMEM petAngry[] = {
  0b00000000, 0b00000000,
  0b00001111, 0b11110000,
  0b00010000, 0b00001000,
  0b00100000, 0b00000100,
  0b00101100, 0b00110100,
  0b00100110, 0b01100100,
  0b00100110, 0b01100100,
  0b00100000, 0b00000100,
  0b00100000, 0b00000100,
  0b00100000, 0b00000100,
  0b00100011, 0b11000100,
  0b00100111, 0b11100100,
  0b00100000, 0b00000100,
  0b00010000, 0b00001000,
  0b00001111, 0b11110000,
  0b00000000, 0b00000000
};


void checkButtons() {
  if (millis() - lastButtonPress < 200) return;

  if (digitalRead(SW1) == LOW) {
    currentScreen = SCREEN_FEED;
    tone(BUZZER_PIN, 1000, 50);
    lastButtonPress = millis();
  } 
  else if (digitalRead(SW2) == LOW) {
    currentScreen = SCREEN_PLAY;
    tone(BUZZER_PIN, 1200, 50);
    lastButtonPress = millis();
  }
  else if (digitalRead(SW3) == LOW) {
    currentScreen = SCREEN_SLEEP;
    tone(BUZZER_PIN, 800, 50);
    lastButtonPress = millis();
  }
};


void updatePet() {
  if (millis() - lastUpdate > 7500 {
    pet.hunger--;
    pet.happiness--;
    pet.energy--;
    
    if (pet.hunger < 50) {
      pet.anger++;
      if (pet.anger > 100) pet.anger = 100;
    }

    if (pet.hunger < 0) pet.hunger = 0;
    if (pet.happiness < 0) pet.happiness = 0;
    if (pet.energy < 0) pet.energy = 0;

    pet.age += 7.5;
    lastUpdate = millis();
  })
};


void handleScreenLogic() {
  switch(currentScreen) {
    case SCREEN_FEED:
      pet.hunger += 10;
      pet.anger -= 10;
      if (pet.hunger > 100) pet.hunger = 100;
      if (pet.anger < 0) pet.anger = 0;
      currentScreen = SCREEN_MAIN;
      break

    case SCREEN_PLAY:
      pet.happiness += 10;
      pet.energy -= 5;
      if (pet.happiness > 100) pet.happiness = 100;
      if (pet.energy < 0) pet.energy = 0;
      currentScreen = SCREEN_MAIN;
      break;
    
    case SCREEN_SLEEP:
      pet.energy += 15;
      if (pet.energy > 100) pet.energy = 100;
      currentScreen = SCREEN_MAIN;
      break;
    
    case SCREEN_MAIN:
      break;
  }
};


void render() {
  display.clearDisplay(); 

  if (pet.happiness < 30) {
    sprite = petSad;
  } else if (pet.hunger < 30) {
    sprite = petAngry;
  } else if (pet.energy < 30) {
    sprite = petSleep;
  } else if (pet.hunger > 50 && pet.happiness > 50 && pet.energy > 50) {
    sprite = petHappy;
  } else {
    sprite = petNeutral;
  }

  display.drawBitMap(56, 2, sprite, 16, 16, SSD1306_WHITE);

  display.setTextSize(1);

  display.setCursor(0, 24);
  display.print("HUN: ");
  drawBar(24, 24, pet.hunger);


  display.setCursor(0, 34);
  display.print("HAP: ");
  drawBar(24, 34, pet.happiness);

  display.setCursor(0, 44);
  display.print("ENG: ");
  drawBar(24, 44, pet.energy);

  display.setCursor(0, 56);
  display.println("[Feed] [Play] [Sleep]")

  display.display()
}


void drawBar() {
  int barWidth = 100;
  int barHeight = 6;
  int fillWidth = map(value, 0, 100, 0, barWidth);

  display.drawRect(x, y, barWidth, barHeight, SSD1306_WHITE);
  display.fillRect(x, y, barWidth, barHeight, SSD1306_WHITE);
}
