#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Keypad.h>
#include <avr/pgmspace.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_ADDRESS 0x3C 
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

#define BUZZER_PIN 10
#define LED_DEBUG 13

// --- CONFIGURAÇÃO DO TECLADO ---
const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS] = {
  {'1','2','3','A'}, 
  {'4','5','6','B'}, 
  {'7','8','9','C'}, 
  {'*','0','#','D'}  
};
byte rowPins[ROWS] = {9, 8, 7, 6}; 
byte colPins[COLS] = {5, 4, 3, 2}; 
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// --- ESTADOS DO SISTEMA ---
enum State { BOOT, WELCOME, SET_DATE, HOME, MENU, APP_NOTAS, APP_FLAPPY, APP_SETTINGS, APP_DINO, APP_SNAKE, APP_MUSIC, APP_MAKER, APP_GEOMETRY, APP_AMONGUS, APP_TETRIS, APP_PREESCRITA, GAMEOVER };
State currentState = BOOT;
State lastApp = MENU;

byte selectedApp = 0; 
const byte totalApps = 11; // Atualizado para 11 aplicativos
byte brilho = 150;

// --- CONFIGURAÇÃO DE DATA GLOBAL ---
byte dateDay = 01;
byte dateMonth = 1;
byte dateYear = 26;
byte dateSel = 0;

// --- MEMÓRIA ESTÁTICA DO BLOCO DE NOTAS ---
char inputNotas[32] = ""; 
byte notasLen = 0;

// --- VARIÁVEIS DE JOGO ---
int pY = 30;
int obsX = 120;
byte sX[15], sY[15]; 
byte sLen = 3, sDir = 6, fX = 40, fY = 40;
unsigned long lastSnakeMove = 0;
int score = 0; 

// --- ESTRUTURAS E VARIÁVEIS DO AMONG US ---
byte auState = 0; // 0: Menu, 1: Gameplay, 2: Vitoria, 3: Derrota
int auPlayerX = 64, auPlayerY = 32;
int auTasks = 0;
unsigned long auLastTaskTick = 0;

struct Crewmate {
  int x;
  int y;
  bool alive;
  unsigned long nextMove;
};
Crewmate auBots[5];

// --- VARIÁVEIS DO TETRIS ---
byte tetrisGrid[14]; 
int8_t tetrisX = 3, tetrisY = 0;
byte tetrisType = 0;
int8_t tpx[4], tpy[4];
unsigned long lastTetrisDrop = 0;
bool tetrisSpawned = false;

// --- VARIÁVEIS DO PRE-ESCRITA ---
byte challengeIdx = 0;
byte challengeStep = 0;

// --- CONTROLADORES T9 ---
unsigned long lastKeyTime = 0;
char lastKey = '\0';
byte tapCount = 0;

// --- MOTOR DE MÚSICA (PROGMEM) ---
bool musicPlaying = false;
byte selectedSong = 0;
byte currentNote = 0;
unsigned long nextNoteTime = 0;

const byte songSizes[] = {30, 24, 32, 20, 13, 22, 11}; 

const uint16_t doomMelody[] PROGMEM = {
  165, 165, 330, 165, 165, 294, 165, 165, 262, 165, 165, 247, 165, 165, 220, 165,
  165, 330, 165, 165, 294, 165, 165, 262, 165, 165, 247, 247, 247, 247
};
const uint16_t doomDur[] PROGMEM = {
  100, 100, 200, 100, 100, 200, 100, 100, 200, 100, 100, 200, 100, 100, 200, 100,
  100, 200, 100, 100, 200, 100, 100, 200, 100, 100, 100, 100, 100, 200
};

const uint16_t intheendMelody[] PROGMEM = {
  311, 466, 466, 370, 349, 311, 311, 311, 466, 466, 370, 349,
  311, 466, 466, 370, 349, 311, 311, 311, 466, 466, 370, 349
};
const uint16_t intheendDur[] PROGMEM = {
  250, 500, 250, 250, 250, 500, 250, 250, 500, 250, 250, 250,
  250, 500, 250, 250, 250, 500, 250, 250, 500, 250, 250, 250
};

const uint16_t nyanMelody[] PROGMEM = {
  370, 415, 311, 311, 247, 262, 277, 247, 247, 277, 311, 349, 370, 311, 370, 415,
  466, 554, 466, 415, 370, 311, 370, 415, 466, 370, 415, 349, 311, 277, 311, 349
};
const uint16_t nyanDur[] PROGMEM = {
  120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120,
  120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120
};

const uint16_t megalovaniaMelody[] PROGMEM = {
  294, 294, 587, 440, 0, 415, 392, 349, 294, 349, 392,
  262, 262, 587, 440, 0, 415, 392, 349, 294
};
const uint16_t megalovaniaDur[] PROGMEM = {
  100, 100, 200, 150, 50, 200, 200, 200, 100, 100, 100,
  100, 100, 200, 150, 50, 200, 200, 200, 100
};

const uint16_t nokiaMelody[] PROGMEM = {
  1319, 1175, 740, 831, 1109, 988, 494, 554, 988, 880, 440, 494, 880
};
const uint16_t nokiaDur[] PROGMEM = {
  100, 100, 200, 200, 100, 100, 200, 200, 100, 100, 200, 200, 400
};

const uint16_t tetrisMelody[] PROGMEM = {
  659, 494, 523, 587, 523, 494, 440, 440, 523, 659, 587, 523, 494, 523, 587, 659, 523, 440, 440, 0, 0, 0
};
const uint16_t tetrisDur[] PROGMEM = {
  300, 150, 150, 300, 150, 150, 300, 150, 150, 300, 150, 150, 450, 150, 300, 150, 150, 300, 150, 150, 300, 300
};

const uint16_t amongusMelody[] PROGMEM = {
  523, 622, 698, 740, 698, 622, 523, 0, 466, 587, 523
};
const uint16_t amongusDur[] PROGMEM = {
  180, 180, 180, 180, 180, 180, 350, 150, 120, 120, 350
};

char savedKey = '\0';

// --- CONFIGURAÇÃO INICIAL ---
void setup() {
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(LED_DEBUG, OUTPUT);
  Wire.begin();
  Wire.setClock(400000);
  
  if(!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDRESS)) {
    while(1) { 
      digitalWrite(LED_DEBUG, HIGH); delay(100);
      digitalWrite(LED_DEBUG, LOW);  delay(100);
    }
  }
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.display();
  runBootAnimation(); 
  randomSeed(analogRead(0));
}

// --- LOOP PRINCIPAL ---
void loop() {
  char key = keypad.getKey();
  if (key != '\0') {
    savedKey = key;
  }

  if (savedKey == 'D' && currentState != MENU && currentState != SET_DATE) { 
    noTone(BUZZER_PIN);
    musicPlaying = false;
    currentState = MENU; 
    resetGames(); 
    savedKey = '\0';
  }

  playSongNonBlocking();

  static unsigned long lastRenderTime = 0;
  if (millis() - lastRenderTime >= 40) {
    lastRenderTime = millis();
    
    char currentKey = savedKey;
    savedKey = '\0'; 

    switch (currentState) {
      case BOOT: 
        currentState = WELCOME; 
        break;
      case WELCOME: 
        drawSplash(F("Bem-vindo\n    a Gtduino OS"), 1500); 
        currentState = SET_DATE; 
        break;
      case SET_DATE:
        runSetDate(currentKey);
        break;
      case HOME: 
        drawHome(currentKey); 
        break;
      case MENU: 
        drawMenu(currentKey); 
        break;
      case APP_NOTAS: 
        runNotas(currentKey); 
        break;
      case APP_FLAPPY: 
        runFlappy(currentKey); 
        break;
      case APP_SETTINGS: 
        runSettings(currentKey); 
        break;
      case APP_DINO: 
        runDino(currentKey); 
        break;
      case APP_SNAKE: 
        runSnake(currentKey); 
        break;
      case APP_MUSIC: 
        runMusic(currentKey); 
        break;
      case APP_MAKER: 
        runMusicMaker(currentKey); 
        break;
      case APP_GEOMETRY: 
        runGeometryPocket(currentKey); 
        break;
      case APP_AMONGUS:
        runAmongUs(currentKey);
        break;
      case APP_TETRIS:
        runTetris(currentKey);
        break;
      case APP_PREESCRITA:
        runPreEscrita(currentKey);
        break;
      case GAMEOVER:
        display.clearDisplay();
        display.setCursor(35, 10); display.print(F("FIM DE JOGO"));
        display.setCursor(38, 28); display.print(F("Score: ")); display.print(score); 
        display.setCursor(22, 48); display.print(F("5 p/ Reiniciar"));
        display.display();
        if(currentKey == '5') { 
          resetGames(); 
          currentState = lastApp; 
        }
        break;
    }
  }
}

void runSetDate(char key) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(26, 2); display.print(F("Ajuste a Data"));
  display.drawFastHLine(0, 13, 128, WHITE);
  
  if(dateSel == 0) display.setTextColor(BLACK, WHITE);
  display.setCursor(22, 28); if(dateDay < 10) display.print('0'); display.print(dateDay);
  display.setTextColor(WHITE);
  display.print(F("/"));
  
  if(dateSel == 1) display.setTextColor(BLACK, WHITE);
  if(dateMonth < 10) display.print('0'); display.print(dateMonth);
  display.setTextColor(WHITE);
  display.print(F("/20"));
  
  if(dateSel == 2) display.setTextColor(BLACK, WHITE);
  if(dateYear < 10) display.print('0'); display.print(dateYear);
  display.setTextColor(WHITE);
  
  display.setCursor(4, 53); display.print(F("4/6:Nav  2/8:Alt  5:Ok"));
  display.display();
  
  if(key == '4') { if(dateSel == 0) dateSel = 2; else dateSel--; }
  if(key == '6') { dateSel++; if(dateSel > 2) dateSel = 0; }
  if(key == '2') {
    if(dateSel == 0) { dateDay++; if(dateDay > 31) dateDay = 1; }
    if(dateSel == 1) { dateMonth++; if(dateMonth > 12) dateMonth = 1; }
    if(dateSel == 2) { dateYear++; if(dateYear > 99) dateYear = 0; }
  }
  if(key == '8') {
    if(dateSel == 0) { if(dateDay == 1) dateDay = 31; else dateDay--; }
    if(dateSel == 1) { if(dateMonth == 1) dateMonth = 12; else dateMonth--; }
    if(dateSel == 2) { if(dateYear == 0) dateYear = 99; else dateYear--; }
  }
  if(key == '5') {
    currentState = HOME;
  }
}

void printAppName(byte idx) {
  switch(idx) {
    case 0: display.print(F("Bloco Notas")); break;
    case 1: display.print(F("Flappy Bird")); break;
    case 2: display.print(F("Config")); break;
    case 3: display.print(F("Dino Game")); break;
    case 4: display.print(F("Snake")); break;
    case 5: display.print(F("Musicas")); break;
    case 6: display.print(F("Music Maker")); break;
    case 7: display.print(F("Geometry Pocket")); break;
    case 8: display.print(F("Among Us")); break;
    case 9: display.print(F("Tetris")); break;
    case 10: display.print(F("Pre Escrita")); break;
  }
}

void printSongName(byte idx) {
  switch(idx) {
    case 0: display.print(F("DOOM E1M1")); break;
    case 1: display.print(F("In The End")); break;
    case 2: display.print(F("Nyan Cat")); break;
    case 3: display.print(F("Megalovania")); break;
    case 4: display.print(F("Nokia Ringtone")); break;
    case 5: display.print(F("Tetris")); break;
    case 6: display.print(F("Among Us Theme")); break;
  }
}

void runBootAnimation() {
  for (byte i = 0; i <= 100; i += 8) { 
    display.clearDisplay();
    display.drawRect(0, 0, 128, 64, WHITE); 
    display.setTextSize(1);
    display.setCursor(32, 14); display.print(F("Gtduino OS"));
    display.setCursor(31, 24); display.print(F("Version 2.0"));
    display.setCursor(22, 32); display.print(F("Carregando..."));
    display.drawRect(14, 44, 100, 8, WHITE);
    display.fillRect(16, 46, map(i, 0, 100, 0, 96), 4, WHITE);
    display.display();
    delay(20); 
  }
  tone(BUZZER_PIN, 1800, 80); delay(100);
  tone(BUZZER_PIN, 2400, 120); delay(150);
}

void drawSplash(const __FlashStringHelper* t, int d) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(20, 24); display.print(t);
  display.display(); 
  delay(d);
}

void drawHome(char key) {
  display.clearDisplay();
  display.setTextSize(1); 
  display.setCursor(32, 5); 
  if(dateDay < 10) display.print('0'); display.print(dateDay); display.print(F("/"));
  if(dateMonth < 10) display.print('0'); display.print(dateMonth); display.print(F("/20"));
  if(dateYear < 10) display.print('0'); display.print(dateYear);
  display.setCursor(25, 55); display.print(F("5: Menu de app"));
  display.display();
  if (key == '5') currentState = MENU;
}

void drawMenu(char key) {
  if (key == 'D') { 
    currentState = HOME;
    return;
  }

  display.clearDisplay();
  display.setCursor(0,0); display.print(F("--- MENU ---"));
  int startApp = 0;
  if (selectedApp >= 5) {
    startApp = selectedApp - 4;
  }
  for(int i=0; i<5 && (startApp + i)<totalApps; i++) {
    int currentIdx = startApp + i;
    int yPos = 15 + (i * 9);
    if(currentIdx == selectedApp) { 
      display.setCursor(0, yPos); display.print(F(">")); 
    }
    display.setCursor(10, yPos); printAppName(currentIdx);
  }
  display.display();
  
  if (key == '2') { selectedApp--; if(selectedApp < 0) selectedApp = totalApps-1; }
  if (key == '8') { selectedApp++; if(selectedApp >= totalApps) selectedApp = 0; }
  if (key == '5') { currentState = (State)(APP_NOTAS + selectedApp); resetGames(); }
}

void playSongNonBlocking() {
  if (!musicPlaying) return;
  if (millis() >= nextNoteTime) {
    byte size = songSizes[selectedSong];
    if (currentNote < size) {
      uint16_t note = 0;
      uint16_t duration = 0;
      switch(selectedSong) {
        case 0: note = pgm_read_word(&doomMelody[currentNote]); duration = pgm_read_word(&doomDur[currentNote]); break;
        case 1: note = pgm_read_word(&intheendMelody[currentNote]); duration = pgm_read_word(&intheendDur[currentNote]); break;
        case 2: note = pgm_read_word(&nyanMelody[currentNote]); duration = pgm_read_word(&nyanDur[currentNote]); break;
        case 3: note = pgm_read_word(&megalovaniaMelody[currentNote]); duration = pgm_read_word(&megalovaniaDur[currentNote]); break;
        case 4: note = pgm_read_word(&nokiaMelody[currentNote]); duration = pgm_read_word(&nokiaDur[currentNote]); break;
        case 5: note = pgm_read_word(&tetrisMelody[currentNote]); duration = pgm_read_word(&tetrisDur[currentNote]); break;
        case 6: note = pgm_read_word(&amongusMelody[currentNote]); duration = pgm_read_word(&amongusDur[currentNote]); break;
      }
      if (note == 0) noTone(BUZZER_PIN);
      else tone(BUZZER_PIN, note);
      nextNoteTime = millis() + duration;
      currentNote++;
    } else {
      currentNote = 0; 
    }
  }
}

char getT9Char(int btn, int tap) {
  switch(btn) {
    case 0: return ' ';
    case 1: return ".,!"[tap % 3];
    case 2: return "ABC"[tap % 3];
    case 3: return "DEF"[tap % 3];
    case 4: return "GHI"[tap % 3];
    case 5: return "JKL"[tap % 3];
    case 6: return "MNO"[tap % 3];
    case 7: return "PQRS"[tap % 4];
    case 8: return "TUV"[tap % 3];
    case 9: return "WXYZ"[tap % 4];
  }
  return '\0';
}

void runNotas(char key) {
  if (key != '\0') {
    if (key == 'A') { 
      if (notasLen > 0) { 
        notasLen--; 
        inputNotas[notasLen] = '\0'; 
      }
      lastKey = '\0'; tapCount = 0;
    } 
    else if (key == '*') { 
      if (notasLen < 31) {
        inputNotas[notasLen] = ' ';
        notasLen++;
        inputNotas[notasLen] = '\0';
      }
      lastKey = '\0'; tapCount = 0;
    } 
    else if (key >= '0' && key <= '9') {
      int btn = key - '0';
      if (key == lastKey && (millis() - lastKeyTime < 800)) {
        if (notasLen > 0) {
          tapCount++;
          inputNotas[notasLen - 1] = getT9Char(btn, tapCount);
        }
      } else { 
        if (notasLen < 31) {
          tapCount = 0;
          inputNotas[notasLen] = getT9Char(btn, tapCount);
          notasLen++;
          inputNotas[notasLen] = '\0';
        }
      }
      lastKey = key; lastKeyTime = millis();
    } else { lastKey = '\0'; tapCount = 0; }
  }
  
  display.clearDisplay();
  display.setCursor(0,0); display.print(F("NOTAS (A:Del *:Esp)"));
  display.drawFastHLine(0, 10, 128, WHITE);
  display.setCursor(0,20); display.print(inputNotas);
  display.display();
}

void resetGames() {
  pY = 30; obsX = 120;
  sLen = 3; sX[0] = 60; sY[0] = 30; sDir = 6;
  lastKey = '\0'; tapCount = 0;
  score = 0; 
  auState = 0;
  tetrisSpawned = false;
  challengeIdx = 0;
  challengeStep = 0;
}

void runFlappy(char key) {
  lastApp = APP_FLAPPY;
  display.clearDisplay();
  if(key == '5') pY -= 6; else pY += 1;
  
  obsX -= 2; 
  if(obsX < -12) { 
    obsX = 128;
    score++; 
  }
  
  display.fillRect(20, pY, 4, 4, WHITE);
  display.fillRect(obsX, 0, 12, 20, WHITE);
  display.fillRect(obsX, 40, 12, 24, WHITE);
  
  display.setCursor(0, 0); display.print(F("PTS:")); display.print(score);
  display.display();
  if(pY > 60 || pY < 0 || (obsX < 24 && obsX > 8 && (pY < 20 || pY > 40))) currentState = GAMEOVER;
}

void runDino(char key) {
  lastApp = APP_DINO;
  display.clearDisplay();
  static int8_t dY = 52, jump = 0;
  if(key == '5' && dY >= 52) jump = 7;
  if(jump > 0) { dY -= 3; jump--; } else if(dY < 52) dY += 2;
  
  obsX -= 4; 
  if(obsX < -6) { 
    obsX = 128;
    score++; 
  }
  
  display.fillRect(15, dY, 6, 6, WHITE);
  display.fillRect(obsX, 52, 5, 8, WHITE);
  display.drawLine(0, 60, 128, 60, WHITE);
  
  display.setCursor(0, 0); display.print(F("PTS:")); display.print(score);
  display.display();
  if(obsX < 21 && obsX > 10 && dY > 46) currentState = GAMEOVER;
}

void runSnake(char key) {
  lastApp = APP_SNAKE;
  display.clearDisplay();
  if(key == '2') sDir = 2; if(key == '8') sDir = 8;
  if(key == '4' && sDir != 6) sDir = 4; if(key == '6' && sDir != 4) sDir = 6;

  if (millis() - lastSnakeMove >= 180) { 
    lastSnakeMove = millis();
    for(int i=sLen; i>0; i--) { sX[i] = sX[i-1]; sY[i] = sY[i-1]; }
    if(sDir == 6) sX[0]+=4; if(sDir == 4) sX[0]-=4;
    if(sDir == 2) sY[0]-=4; if(sDir == 8) sY[0]+=4;
    
    if(abs(sX[0]-fX)<4 && abs(sY[0]-fY)<4) { 
      sLen = min(sLen + 1, 14); 
      fX = random(5, 120); 
      fY = random(15, 55); 
      score++; 
    }
    if(sX[0]<0 || sX[0]>124 || sY[0]<0 || sY[0]>60) currentState = GAMEOVER;
  }
  for(int i=0; i<sLen; i++) display.fillRect(sX[i], sY[i], 3, 3, WHITE);
  display.fillRect(fX, fY, 3, 3, WHITE);
  
  display.setCursor(0, 0); display.print(F("PTS:")); display.print(score);
  display.display();
}

void initAmongUsGame() {
  auPlayerX = 60; 
  auPlayerY = 20; 
  auTasks = 0;
  auLastTaskTick = millis();
  for(byte i = 0; i < 5; i++) {
    auBots[i].x = random(15, 110);
    auBots[i].y = random(20, 48);
    auBots[i].alive = true;
    auBots[i].nextMove = millis() + random(100, 600);
  }
  selectedSong = 6; 
  currentNote = 0;
  nextNoteTime = millis();
  musicPlaying = true;
}

void runAmongUs(char key) {
  if(auState == 0) {
    display.clearDisplay();
    display.setTextSize(2);
    display.setCursor(16, 12); display.print(F("Among Us"));
    display.setTextSize(1);
    display.drawRect(39, 40, 50, 16, WHITE);
    display.setCursor(49, 44); display.print(F("JOGAR"));
    display.display();
    if(key == '5') {
      initAmongUsGame();
      auState = 1; 
    }
    return;
  }
  
  if(auState == 1) {
    if(key == '2' && auPlayerY > 15) auPlayerY -= 3;
    if(key == '8' && auPlayerY < 53) auPlayerY += 3;
    if(key == '4' && auPlayerX > 2)  auPlayerX -= 3;
    if(key == '6' && auPlayerX < 119) auPlayerX += 3;
    
    if(millis() - auLastTaskTick >= 1500) {
      auLastTaskTick = millis();
      byte activeBots = 0;
      for(byte i=0; i<5; i++) { if(auBots[i].alive) activeBots++; }
      auTasks += activeBots;
      if(auTasks >= 100) { 
        auTasks = 100; 
        auState = 3; 
        musicPlaying = false; noTone(BUZZER_PIN);
      }
    }
    
    byte aliveCount = 0;
    for(byte i = 0; i < 5; i++) {
      if(auBots[i].alive) {
        aliveCount++;
        if(millis() >= auBots[i].nextMove) {
          auBots[i].x += random(-4, 5);
          auBots[i].y += random(-4, 5);
          if(auBots[i].x < 2) auBots[i].x = 2;
          if(auBots[i].x > 119) auBots[i].x = 119;
          if(auBots[i].y < 15) auBots[i].y = 15;
          if(auBots[i].y > 53) auBots[i].y = 53;
          auBots[i].nextMove = millis() + random(400, 900);
        }
        
        if(key == '5') {
          if(abs(auPlayerX - auBots[i].x) < 12 && abs(auPlayerY - auBots[i].y) < 12) {
            auBots[i].alive = false;
            tone(BUZZER_PIN, 130, 180); 
          }
        }
      }
    }
    
    if(aliveCount == 0) { 
      auState = 2; 
      musicPlaying = false; noTone(BUZZER_PIN); 
    }
    
    display.clearDisplay();
    display.drawRect(0, 14, 128, 50, WHITE); 
    
    display.drawFastHLine(0, 46, 128, WHITE);
    display.drawFastVLine(38, 46, 18, WHITE); 
    display.drawFastVLine(88, 46, 18, WHITE); 
    
    display.drawFastHLine(0, 32, 38, WHITE);  
    display.drawFastHLine(88, 32, 40, WHITE);  
    display.drawFastVLine(38, 14, 32, WHITE);  
    display.drawFastVLine(88, 14, 32, WHITE);  
    
    display.setTextSize(1);
    display.setCursor(54, 16);  display.print(F("CAF"));  
    display.setCursor(4, 16);   display.print(F("MED"));  
    display.setCursor(94, 16);  display.print(F("ARM"));  
    display.setCursor(4, 35);   display.print(F("REA"));  
    display.setCursor(94, 35);  display.print(F("NAV"));  
    display.setCursor(4, 50);   display.print(F("ELEC")); 
    display.setCursor(48, 50);  display.print(F("ALM"));  
    display.setCursor(94, 50);  display.print(F("ESC"));  
    
    display.setCursor(2, 2); display.print(F("TSK:"));
    display.drawRect(26, 2, 98, 8, WHITE);
    display.fillRect(28, 4, map(auTasks, 0, 100, 0, 94), 4, WHITE);
    
    for(byte i = 0; i < 5; i++) {
      if(auBots[i].alive) {
        display.drawRect(auBots[i].x + 2, auBots[i].y, 6, 10, WHITE); 
        display.drawRect(auBots[i].x, auBots[i].y + 2, 2, 6, WHITE);  
        display.drawRect(auBots[i].x + 4, auBots[i].y + 2, 3, 3, WHITE); 
      } else {
        display.drawLine(auBots[i].x, auBots[i].y, auBots[i].x + 7, auBots[i].y + 9, WHITE);
        display.drawLine(auBots[i].x + 7, auBots[i].y, auBots[i].x, auBots[i].y + 9, WHITE);
      }
    }
    
    display.fillRect(auPlayerX + 2, auPlayerY, 6, 10, WHITE); 
    display.fillRect(auPlayerX, auPlayerY + 2, 2, 6, WHITE);  
    display.fillRect(auPlayerX + 4, auPlayerY + 2, 3, 3, BLACK); 
    
    display.display();
    return;
  }
  
  display.clearDisplay();
  display.setTextSize(2);
  if(auState == 2) {
    display.setCursor(16, 15); display.print(F("VITORIA"));
  } else {
    display.setCursor(16, 15); display.print(F("DERROTA"));
  }
  display.setTextSize(1);
  display.setCursor(14, 46); display.print(F("5: Voltar ao Menu"));
  display.display();
  
  if(key == '5') {
    auState = 0;
    currentState = MENU;
  }
}

// --- NOVO APLICATIVO: TETRIS ---
void spawnTetrisPiece() {
  tetrisX = 3; tetrisY = 0;
  tetrisType = random(0, 4);
  if(tetrisType == 0) { // Bloco Quadrado (O-Piece)
    tpx[0]=0; tpy[0]=0; tpx[1]=1; tpy[1]=0; tpx[2]=0; tpy[2]=1; tpx[3]=1; tpy[3]=1;
  } else if(tetrisType == 1) { // Bloco Linha (I-Piece)
    tpx[0]=0; tpy[0]=0; tpx[1]=0; tpy[1]=1; tpx[2]=0; tpy[2]=2; tpx[3]=0; tpy[3]=3;
  } else if(tetrisType == 2) { // Bloco L (L-Piece)
    tpx[0]=0; tpy[0]=0; tpx[1]=0; tpy[1]=1; tpx[2]=0; tpy[2]=2; tpx[3]=1; tpy[3]=2;
  } else { // Bloco T (T-Piece)
    tpx[0]=0; tpy[0]=0; tpx[1]=1; tpy[1]=0; tpx[2]=2; tpy[2]=0; tpx[3]=1; tpy[3]=1;
  }
}

bool checkTetrisCollision(int8_t dx, int8_t dy, bool rotate) {
  for(byte i=0; i<4; i++) {
    int8_t rx = tpx[i];
    int8_t ry = tpy[i];
    if(rotate) {
      int8_t tx = rx;
      rx = -ry;
      ry = tx;
    }
    int8_t wx = tetrisX + dx + rx;
    int8_t wy = tetrisY + dy + ry;
    if(wx < 0 || wx >= 8 || wy >= 14) return true;
    if(wy >= 0 && (tetrisGrid[wy] & (1 << wx))) return true;
  }
  return false;
}

void runTetris(char key) {
  lastApp = APP_TETRIS;
  if(!tetrisSpawned) {
    memset(tetrisGrid, 0, sizeof(tetrisGrid));
    spawnTetrisPiece();
    tetrisSpawned = true;
    lastTetrisDrop = millis();
  }
  
  if(key == '4' && !checkTetrisCollision(-1, 0, false)) tetrisX--;
  if(key == '6' && !checkTetrisCollision(1, 0, false)) tetrisX++;
  if(key == '2' && !checkTetrisCollision(0, 0, true)) {
    for(byte i=0; i<4; i++) {
      int8_t tx = tpx[i];
      tpx[i] = -tpy[i];
      tpy[i] = tx;
    }
  }
  if(key == '8' && !checkTetrisCollision(0, 1, false)) tetrisY++;

  if(millis() - lastTetrisDrop >= 600) {
    lastTetrisDrop = millis();
    if(!checkTetrisCollision(0, 1, false)) {
      tetrisY++;
    } else {
      for(byte i=0; i<4; i++) {
        int8_t wx = tetrisX + tpx[i];
        int8_t wy = tetrisY + tpy[i];
        if(wy >= 0) tetrisGrid[wy] |= (1 << wx);
      }
      for(int y=0; y<14; y++) {
        if(tetrisGrid[y] == 0xFF) { // Linha cheia (8 bits ativados)
          score += 10;
          for(int ty=y; ty>0; ty--) tetrisGrid[ty] = tetrisGrid[ty-1];
          tetrisGrid[0] = 0;
        }
      }
      spawnTetrisPiece();
      if(checkTetrisCollision(0, 0, false)) {
        tetrisSpawned = false;
        currentState = GAMEOVER;
      }
    }
  }

  display.clearDisplay();
  display.drawRect(44, 4, 36, 58, WHITE);
  
  for(byte y=0; y<14; y++) {
    for(byte x=0; x<8; x++) {
      if(tetrisGrid[y] & (1 << x)) display.fillRect(46 + x*4, 6 + y*4, 3, 3, WHITE);
    }
  }
  for(byte i=0; i<4; i++) {
    int8_t wx = tetrisX + tpx[i];
    int8_t wy = tetrisY + tpy[i];
    if(wy >= 0) display.fillRect(46 + wx*4, 6 + wy*4, 3, 3, WHITE);
  }

  display.setCursor(0, 0); display.print(F("PTS"));
  display.setCursor(0, 10); display.print(score);
  display.setCursor(0, 32); display.print(F("4/6:Mv"));
  display.setCursor(0, 42); display.print(F("2:Rot"));
  display.setCursor(0, 52); display.print(F("8:Cair"));
  display.display();
}

// --- NOVO APLICATIVO: PRE ESCRITA (20 DESAFIOS) ---
void runPreEscrita(char key) {
  lastApp = APP_PREESCRITA;
  display.clearDisplay();
  display.setCursor(0, 0); display.print(F("--- PRE-ESCRITA ---"));
  display.setCursor(0, 12); display.print(F("Desafio ")); display.print(challengeIdx + 1); display.print(F("/20:"));
  
  display.setCursor(0, 26);
  const char* targetSeq = "";
  
  switch(challengeIdx) {
    case 0:  display.print(F("Aperte '5' 3 vezes")); targetSeq = "555"; break;
    case 1:  display.print(F("Digite: 123")); targetSeq = "123"; break;
    case 2:  display.print(F("Digite: 456")); targetSeq = "456"; break;
    case 3:  display.print(F("Digite: 789")); targetSeq = "789"; break;
    case 4:  display.print(F("Digite: *#*")); targetSeq = "*#*"; break;
    case 5:  display.print(F("Aperte 'A' e dps 'B'")); targetSeq = "AB"; break;
    case 6:  display.print(F("Digite o ano: 2026")); targetSeq = "2026"; break;
    case 7:  display.print(F("Sequencia: 159")); targetSeq = "159"; break;
    case 8:  display.print(F("Sequencia: 357")); targetSeq = "357"; break;
    case 9:  display.print(F("Aperte '2' 4 vezes")); targetSeq = "2222"; break;
    case 10: display.print(F("Aperte '8' 4 vezes")); targetSeq = "8888"; break;
    case 11: display.print(F("Código: A1B2")); targetSeq = "A1B2"; break;
    case 12: display.print(F("Coluna 1: 741")); targetSeq = "741"; break;
    case 13: display.print(F("Coluna 3: 963")); targetSeq = "963"; break;
    case 14: display.print(F("Aperte '*' 4 vezes")); targetSeq = "****"; break;
    case 15: display.print(F("Aperte '#' 4 vezes")); targetSeq = "####"; break;
    case 16: display.print(F("Cruzado: 2468")); targetSeq = "2468"; break;
    case 17: display.print(F("Bordas: 1379")); targetSeq = "1379"; break;
    case 18: display.print(F("Confirme: C6")); targetSeq = "C6"; break;
    case 19: display.print(F("Desafio Final: 5555")); targetSeq = "5555"; break;
  }
  
  display.setCursor(0, 52); display.print(F("Progresso: "));
  for(byte i=0; i<challengeStep; i++) display.print(F("*"));
  display.display();
  
  if (key != '\0') {
    if (key == targetSeq[challengeStep]) {
      challengeStep++;
      tone(BUZZER_PIN, 2000, 40);
      if (targetSeq[challengeStep] == '\0') { // Desafio concluído!
        challengeIdx++;
        challengeStep = 0;
        score += 15;
        tone(BUZZER_PIN, 2500, 200);
        if (challengeIdx >= 20) {
          currentState = GAMEOVER; // Concluiu todos!
        }
      }
    } else {
      challengeStep = 0; // Se errar, reinicia o progresso do desafio atual
      tone(BUZZER_PIN, 350, 150);
    }
  }
}

void runSettings(char key) {
  display.clearDisplay();
  display.setCursor(0,0); display.print(F("Brilho: 2(+) 8(-)"));
  display.drawRect(10, 30, 100, 10, WHITE);
  display.fillRect(10, 30, map(brilho, 0, 255, 0, 100), 10, WHITE);
  display.display();
  if(key == '2') brilho = min(brilho+30, 255);
  if(key == '8') brilho = max(brilho-30, 0);
  display.ssd1306_command(SSD1306_SETCONTRAST);
  display.ssd1306_command(brilho);
}

void runMusic(char key) {
  if (key == '2') { selectedSong--; if (selectedSong < 0) selectedSong = 6; if (musicPlaying) { musicPlaying = false; noTone(BUZZER_PIN); } }
  if (key == '8') { selectedSong++; if (selectedSong > 6) selectedSong = 0; if (musicPlaying) { musicPlaying = false; noTone(BUZZER_PIN); } }
  if (key == '5') { if (musicPlaying) { musicPlaying = false; noTone(BUZZER_PIN); } else { currentNote = 0; nextNoteTime = millis(); musicPlaying = true; } }

  display.clearDisplay();
  display.setCursor(0, 0); display.print(F("--- PLAYLIST ---"));
  int startSong = 0;
  if (selectedSong >= 5) startSong = selectedSong - 4;
  for (int i = 0; i < 5; i++) {
    int currentIdx = startSong + i;
    int yPos = 15 + (i * 9);
    if (currentIdx == selectedSong) display.setCursor(0, yPos); display.print(musicPlaying ? F(">") : F("-"));
    display.setCursor(10, yPos); printSongName(currentIdx);
  }
  display.display();
}

void runMusicMaker(char key) {
  if (key >= '1' && key <= '6') {
    static const uint16_t frequencies[] PROGMEM = {262, 294, 330, 349, 392, 440}; 
    tone(BUZZER_PIN, pgm_read_word(&frequencies[key - '1']), 120);
  }
  display.clearDisplay();
  display.setCursor(0, 0); display.print(F("--- MUSIC MAKER ---"));
  display.setCursor(0, 20); display.print(F("Clique nos botoes"));
  display.setCursor(0, 32); display.print(F("de 1 a 6 para"));
  display.setCursor(0, 44); display.print(F("fazer uma melodia!"));
  display.display();
}

void runGeometryPocket(char key) {
  static byte selectedCmd = 0;
  static bool showingAnswer = false;
  static byte scroll_y = 0;
  const byte totalCmds = 27; 

  if (!showingAnswer) {
    if (key == '2') { if (selectedCmd == 0) selectedCmd = totalCmds - 1; else selectedCmd--; } 
    if (key == '8') { selectedCmd++; if (selectedCmd >= totalCmds) selectedCmd = 0; }
    if (key == '5') { showingAnswer = true; scroll_y = 0; } 

    display.clearDisplay();
    display.setCursor(0, 0); display.print(F("Geometry Pocket"));
    display.drawFastHLine(0, 10, 128, WHITE);
    display.setCursor(0, 15); display.print(F("O que precisas?"));
    display.drawRect(0, 30, 128, 20, WHITE);
    display.setCursor(4, 36);
        
    switch(selectedCmd) {
      case 0:  display.print(F("O que voce faz?")); break;
      case 1:  display.print(F("Quem e seu criador?")); break;
      case 2:  display.print(F("Clima de Cachoeiro?")); break;
      case 3:  display.print(F("O que e o Arduino?")); break;
      case 4:  display.print(F("Materiais usados?")); break;
      case 5:  display.print(F("Colonizacao do BR?")); break;
      case 6:  display.print(F("O que sao num bin?")); break;
      case 7:  display.print(F("E facil programar?")); break;
      case 8:  display.print(F("O que e a Apollo 11?")); break;
      case 9:  display.print(F("Qual a ideia dessa criacao?")); break;
      case 10: display.print(F("Esse aparelho e simples?")); break;
      case 11: display.print(F("Executar teste Ping")); break;
      case 12: display.print(F("Qual a velocidade da luz?")); break;
      case 13: display.print(F("O que e Gtduino OS?")); break;
      case 14: display.print(F("Me conte uma piada!")); break;
      case 15: display.print(F("Segredo do sucesso?")); break;
      case 16: display.print(F("11/09/2001")); break;
      case 17: display.print(F("O que e a Internet?")); break;
      case 18: display.print(F("Quem foi Einstein?")); break;
      case 19: display.print(F("O que e um Bit?")); break;
      case 20: display.print(F("Qual formula da agua?")); break;
      case 21: display.print(F("O que e Hardware?")); break;
      case 22: display.print(F("O que e Software?")); break;
      case 23: display.print(F("Maior planeta?")); break;
      case 24: display.print(F("O que e open-source?")); break;
      case 25: display.print(F("O que e um Bug?")); break;
      case 26: display.print(F("Dica de estudo?")); break;
    }
    display.setCursor(0, 55); display.print(F("2/8:Nav   5:Perguntar"));
    display.display();
  } 
  else {
    if (key == '5') { showingAnswer = false; }
    if (key == 'A') { scroll_y = max(0, scroll_y - 8); }      
    if (key == 'B') { scroll_y = min(64, scroll_y + 8); }      

    display.clearDisplay();
    display.setCursor(0, 15 - scroll_y);
    
    switch(selectedCmd) {
      case 0: display.print(F("Ola! Eu sou o Geometry\nPocket, sua IA local.\nPosso responder sobre\ntecnologia, historia,\nciencia e dar detalhes\ndeste aparelho.\nUse A/B para rolar!")); break;
      case 1: display.print(F("O meu criador e o\ntalentoso Gabriel mo-\nta dos Santos, conhecido\ncomo Guester_DEV.\nEle programou este OS\ndo completo zero!")); break;
      case 2: display.print(F("Em Cachoeiro de\nItapemirim, o clima e\ntropical e quente.\nVeras veroes intensos\ne umidos, e invernos\nmais secos e amenos.")); break;
      case 3: display.print(F("O Arduino e uma\nplataforma aberta de\nprototipagem. Permite\ncriar projetos inter-\nativos de eletronica,\nsejam eles complexos\nou bem simples.")); break;
      case 4: display.print(F("Neste dispositivo foi\nusado: 1x Arduino UNO,\n1x Tela OLED 0.96,\n1x Protoboard para as\nconexoes e 1x Buzzer\nde caixa de som.")); break;
      case 5: display.print(F("A colonização do\nBrasil comecou de\nfato em 1530, com a\nchegada de expedicoes\nportuguesas, 30 anos\napos o descobrimento\nem 1500.")); break;
      case 6: display.print(F("Numeros binarios sao\num sistema de base 2\n(0 ou 1). Eles formam\na linguagem nativa e\nfundamental de todos\nos processadores e\nmaquinas do mundo.")); break;
      case 7: display.print(F("Nao e dificil,\nmas exige persistenca.\nComece entendendo a\nlogica de programacao\naprimore a sua habilidade\ntodos os dias.")); break;
      case 8: display.print(F("A missao Apollo 11\nfoi responsavel pelo\nprimeiro pouso humano\nna Lua, em 20 de julho\nde 1969, comandada por\nNeil Armstrong.")); break;
      case 9: display.print(F("A ideia principal e\ncriar um ecossistema\nde simulação operacional\nretro embarcado,\nmostrando o poder de\notimizacao do Arduino.")); break;
      case 10: display.print(F("Por fora sim, mas\no gerenciamento de\nmemoria interna e\nestados do software\nenvolve conceitos de\nengenharia complexos.")); break;
      case 11: display.print(F("Enviando pacotes ICMP\npara o nucleo local...\nResposta de 127.0.0.1\nTempo: 1ms\nStatus: Online e OK!")); break;
      case 12: display.print(F("A velocidade da luz\nno vacuo e de aproxi-\nmadamente 299.792.458\nmetros por segundo\n(ou ~300.000 km/s).")); break;
      case 13: display.print(F("O Gtduino OS e um\nsistema operacional\nficticio e customizado\ncriado em C/C++ para\nrodar em hardwares\nlimitados de 8-bits.")); break;
      case 14: display.print(F("Por que o computador\nfoi ao medico?\nPorque ele estava com\num virus de sistema!\nHaha... ou nao.")); break;
      case 15: display.print(F("O segredo e nunca\nparar de aprender,\nter resiliencia diante\dos erros e sempre\ntestar novos codigos.")); break;
      case 16: display.print(F("Data marcada pelos\nataques terroristas\naos Estados Unidos,\nonde as torres gemeas\ndo World Trade Center\nforam destruidas.")); break;
      case 17: display.print(F("A internet e uma\nrede global de com-\nputadores interconec-\ntados que utilizam o\nconjunto de protocolos\nTCP/IP para trocar dados.")); break;
      case 18: display.print(F("Albert Einstein foi\num fisico teorico\nalemao que desenvolveu\na teoria da relativi-\ndade, um dos pilares\nda fisica moderna.")); break;
      case 19: display.print(F("Um Bit (Binary Digit)\ne a menor unidade de\ninformacao em um\ncomputador, podendo\nassumir os valores\nde 0 ou 1 apenas.")); break;
      case 20: display.print(F("A formula quimica\nda agua e H2O. Signi-\nfica que cada molecula\npossui dois atomos de\nhidrogenio e um de\noxigenio.")); break;
      case 21: display.print(F("Hardware e a parte\nfisica de um compu-\ntador ou dispositivo,\niu seja, as peças,\nplacas, fios e todos\nos componentes mecânicos.")); break;
      case 22: display.print(F("Software e a parte\nlogica, constituida\ pelos programas,\ncodigos e instrucoes\nque dizem ao hardware\no que ele deve fazer.")); break;
      case 23: display.print(F("O maior planeta do\nnosso sistema solar e\nJupiter. Ele e um\ngigante gasoso com\nmais de duas vezes a\nmassa de todos os outros.")); break;
      case 24: display.print(F("Open-source significa\ncodigo aberto. Qualquer\npessoa pode ver, modi-\nficar e distribuir o\ncodigo da forma que\nachar melhor.")); break;
      case 25: display.print(F("Um Bug e um erro ou\nfalha inesperada em\num software que faz com\nque ele funcione de\nmaneira incorreta ou\ntrave completamente.")); break;
      case 26: display.print(F("Foque na pratica!\nEscreva codigos por\nconta propria, tente\nmodificar projetos e\nnão tenha medo de\nerrar nos testes.")); break;
    }
    display.display();
  }
}
