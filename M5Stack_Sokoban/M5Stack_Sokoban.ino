//#define WIFI_UDP_CONTROLLER_USE    // if you don't use WiFi UDP Controller, delete this line. In the case, you can use serial port controller.
#include <M5Stack.h>
#ifdef WIFI_UDP_CONTROLLER_USE
  #include <WiFi.h>
  #include <WiFiUdp.h>
#else
  #include "WaveAnimation.h"
#endif
#include "Map.h"
#include "BlockImage.h"
#include "WalkAnimation.h"
#include "PushAnimation.h"
#include "StageChangeAnimation.h"
#include "StageLogo.h"
#include "ThankYouLogo.h"

#ifdef WIFI_UDP_CONTROLLER_USE
WiFiUDP udp;
#endif
const uint16_t backgroundColor = 0xC7EF;
uint16_t buffer100x100[100][100];
uint16_t stageLogoImage[41][119];
uint16_t thankYouLogoImage[58][320];
const int h = 30, w = 30;
uint16_t blockImage[h][w];
uint16_t walkingHeight[2 * h][w];
uint16_t walkingWidth[h][2 * w];
uint16_t pushingHeight[3 * h][w];
uint16_t pushingWidth[h][3 * w];
const int transparent = 0x18C3;
int staticMap[mapH][mapW];
int activeMap[mapH][mapW];
enum Dir { UP, RIGHT, DOWN, LEFT };

void fillImage(void *image, int x, int y, int w, int h) {
  uint16_t* p = (uint16_t*)image;
  M5.Lcd.startWrite(); // Requires start/end transaction now
  M5.Lcd.setAddrWindow(x, y, x+w-1, y+h-1);
  for ( int i = 0; i < h; i++) {
    for (int j = 0; j < w; j++) {
        M5.Lcd.writePixel(p[i*w+j]);
    }
  }
  M5.Lcd.endWrite(); // End last TFT transaction  
}

void DrawBlock(int x, int y) {
  for (int i = 0; i < h; ++i) for (int j = 0; j < w; ++j) blockImage[i][j] = blockImages[activeMap[y][x]][i][j];
  fillImage(blockImage, x * w , y * h , w, h);
}

void Walk(Dir dir, int x, int y) {
  const int numAnimation = 30;
  for (int t = 0; t < numAnimation + 1; ++t) {
    if (dir == UP) {
      for (int i = 0; i < h; ++i) for (int j = 0; j < w; ++j) walkingHeight[i][j] = blockImages[staticMap[y - 1][x]][i][j];
      for (int i = 0; i < h; ++i) for (int j = 0; j < w; ++j) walkingHeight[h + i][j] = blockImages[staticMap[y][x]][i][j];
      for (int i = 0; i < h; ++i) {
        for (int j = 0; j < w; ++j) {
          uint16_t pixel = walkAnimation[t][i][j];
          if (pixel != transparent) walkingHeight[(numAnimation - t) * h / numAnimation + i][j] = pixel;
        }
      }
      fillImage(walkingHeight, x * w , (y - 1) * h , w, 2 * h);
    }
    else if (dir == DOWN) {
      for (int i = 0; i < h; ++i) for (int j = 0; j < w; ++j) walkingHeight[i][j] = blockImages[staticMap[y][x]][i][j];
      for (int i = 0; i < h; ++i) for (int j = 0; j < w; ++j) walkingHeight[h + i][j] = blockImages[staticMap[y + 1][x]][i][j];
      for (int i = 0; i < h; ++i) {
        for (int j = 0; j < w; ++j) {
          uint16_t pixel = walkAnimation[t][h - 1 - i][w - 1 - j];
          if (pixel != transparent) walkingHeight[t * h / numAnimation + i][j] = pixel;
        }
      }
      fillImage(walkingHeight, x * w , y * h , w, 2 * h);
    }
    else if (dir == RIGHT) {
      for (int i = 0; i < h; ++i) for (int j = 0; j < w; ++j) walkingWidth[i][j] = blockImages[staticMap[y][x]][i][j];
      for (int i = 0; i < h; ++i) for (int j = 0; j < w; ++j) walkingWidth[i][w + j] = blockImages[staticMap[y][x + 1]][i][j];
      for (int i = 0; i < h; ++i) {
        for (int j = 0; j < w; ++j) {
          uint16_t pixel = walkAnimation[t][w - 1 - j][i];
          if (pixel != transparent) walkingWidth[i][t * w / numAnimation + j] = pixel;
        }
      }
      fillImage(walkingWidth, x * w , y * h , 2 * w, h);
    }
    else if (dir == LEFT) {
      for (int i = 0; i < h; ++i) for (int j = 0; j < w; ++j) walkingWidth[i][j] = blockImages[staticMap[y][x - 1]][i][j];
      for (int i = 0; i < h; ++i) for (int j = 0; j < w; ++j) walkingWidth[i][w + j] = blockImages[staticMap[y][x]][i][j];
      for (int i = 0; i < h; ++i) {
        for (int j = 0; j < w; ++j) {
          uint16_t pixel = walkAnimation[t][j][h - 1 - i];
          if (pixel != transparent) walkingWidth[i][(numAnimation - t) * w / numAnimation + j] = pixel;
        }
      }
      fillImage(walkingWidth, (x - 1) * w , y * h , 2 * w, h);
    }
    delay(10);
  }
}

void Push(Dir dir, int x, int y) {
  const int numAnimation = 30;
  for (int t = 0; t < numAnimation + 1; ++t) {
    if (dir == UP) {
      for (int i = 0; i < h; ++i) for (int j = 0; j < w; ++j) pushingHeight[i][j] = blockImages[staticMap[y - 2][x]][i][j];
      for (int i = 0; i < h; ++i) for (int j = 0; j < w; ++j) pushingHeight[h + i][j] = blockImages[staticMap[y - 1][x]][i][j];
      for (int i = 0; i < h; ++i) for (int j = 0; j < w; ++j) pushingHeight[2 * h + i][j] = blockImages[staticMap[y][x]][i][j];
      for (int i = 0; i < h; ++i) for (int j = 0; j < w; ++j) pushingHeight[(numAnimation - t) * h / numAnimation + i][j] = blockImages[2][i][j];
      for (int i = 0; i < h; ++i) {
        for (int j = 0; j < w; ++j) {
          uint16_t pixel = pushAnimation[t][i][j];
          if (pixel != transparent) pushingHeight[(numAnimation - t) * h / numAnimation + h + i][j] = pixel;
        }
      }
      fillImage(pushingHeight, x * w , (y - 2) * h , w, 3 * h);
    }
    else if (dir == DOWN) {
      for (int i = 0; i < h; ++i) for (int j = 0; j < w; ++j) pushingHeight[i][j] = blockImages[staticMap[y][x]][i][j];
      for (int i = 0; i < h; ++i) for (int j = 0; j < w; ++j) pushingHeight[h + i][j] = blockImages[staticMap[y + 1][x]][i][j];
      for (int i = 0; i < h; ++i) for (int j = 0; j < w; ++j) pushingHeight[2 * h + i][j] = blockImages[staticMap[y + 2][x]][i][j];
      for (int i = 0; i < h; ++i) for (int j = 0; j < w; ++j) pushingHeight[t * h / numAnimation + h + i][j] = blockImages[2][i][j];
      for (int i = 0; i < h; ++i) {
        for (int j = 0; j < w; ++j) {
          uint16_t pixel = pushAnimation[t][h - 1 - i][w - 1 - j];
          if (pixel != transparent) pushingHeight[t * h / numAnimation + i][j] = pixel;
        }
      }
      fillImage(pushingHeight, x * w , y * h , w, 3 * h);
    }
    else if (dir == RIGHT) {
      for (int i = 0; i < h; ++i) for (int j = 0; j < w; ++j) pushingWidth[i][j] = blockImages[staticMap[y][x]][i][j];
      for (int i = 0; i < h; ++i) for (int j = 0; j < w; ++j) pushingWidth[i][w + j] = blockImages[staticMap[y][x + 1]][i][j];
      for (int i = 0; i < h; ++i) for (int j = 0; j < w; ++j) pushingWidth[i][2 * w + j] = blockImages[staticMap[y][x + 2]][i][j];
      for (int i = 0; i < h; ++i) for (int j = 0; j < w; ++j) pushingWidth[i][t * w / numAnimation + w + j] = blockImages[2][i][j];
      for (int i = 0; i < h; ++i) {
        for (int j = 0; j < w; ++j) {
          uint16_t pixel = pushAnimation[t][w - 1 - j][i];
          if (pixel != transparent) pushingWidth[i][t * w / numAnimation + j] = pixel;
        }
      }
      fillImage(pushingWidth, x * w , y * h , 3 * w, h);
    }
    else if (dir == LEFT) {
      for (int i = 0; i < h; ++i) for (int j = 0; j < w; ++j) pushingWidth[i][j] = blockImages[staticMap[y][x - 2]][i][j];
      for (int i = 0; i < h; ++i) for (int j = 0; j < w; ++j) pushingWidth[i][w + j] = blockImages[staticMap[y][x - 1]][i][j];
      for (int i = 0; i < h; ++i) for (int j = 0; j < w; ++j) pushingWidth[i][2 * w + j] = blockImages[staticMap[y][x]][i][j];
      for (int i = 0; i < h; ++i) for (int j = 0; j < w; ++j) pushingWidth[i][(numAnimation - t) * w / numAnimation + j] = blockImages[2][i][j];
      for (int i = 0; i < h; ++i) {
        for (int j = 0; j < w; ++j) {
          uint16_t pixel = pushAnimation[t][j][h - 1 - i];
          if (pixel != transparent) pushingWidth[i][(numAnimation - t) * w / numAnimation + w + j] = pixel;
        }
      }
      fillImage(pushingWidth, (x - 2) * w , y * h , 3 * w, h);
    }
    delay(10);
  }
  for (int i = 0; i < h; ++i) for (int j = 0; j < w; ++j) blockImage[i][j] = blockImages[3][i][j];
  if (dir == UP && staticMap[y - 2][x] == 4) {
    fillImage(blockImage, x * w , (y - 2) * h , w, h);
    delay(300);
  }
  if (dir == DOWN && staticMap[y + 2][x] == 4) {
    fillImage(blockImage, x * w , (y + 2) * h , w, h);
    delay(300);
  }
  if (dir == RIGHT && staticMap[y][x + 2] == 4) {
    fillImage(blockImage, (x + 2) * w , y * h , w, h);
    delay(300);
  }
  if (dir == LEFT && staticMap[y][x - 2] == 4) {
    fillImage(blockImage, (x - 2) * w , y * h , w, h);
    delay(300);
  }
}

void StageChange(int stageNumber) {
  for (int i = 0; i < 80; ++i) for (int j = 0; j < 233; ++j) stageLogoImage[i][j] = stageLogoImages[stageNumber][i][j];
  fillImage(stageLogoImage, 100 , 20 , 119, 41);
  for (int l = 0; l < 5; ++l) {
    for (int k = 0; k < 12; ++k) {
      for (int i = 0; i < 100; ++i) for (int j = 0; j < 100; ++j) buffer100x100[i][j] = stageChangeAnimation[k][i][j];
      fillImage(buffer100x100, 110 , 100 , 100, 100);
      delay(20);
    }
  }
}

void ShowThankYou() {
  M5.Lcd.fillScreen(backgroundColor);
  for (int i = 0; i < 58; ++i) for (int j = 0; j < 320; ++j) thankYouLogoImage[i][j] = thankYouLogo[i][j];
  fillImage(thankYouLogoImage, 0 , 20 , 320, 58);

#ifndef WIFI_UDP_CONTROLLER_USE
  for (int k = 0; k < 18; ++k) {
    for (int i = 0; i < 100; ++i) for (int j = 0; j < 100; ++j) buffer100x100[i][j] = waveAnimation[k][i][j];
    fillImage(buffer100x100, 110 , 100 , 100, 100);
    delay(20);
  }
#endif
}

void setup() {
  // initialize the M5Stack object
  M5.begin();
  M5.Lcd.setBrightness(100);
  M5.Lcd.fillScreen(BLACK);
  Serial.begin(115200);
#ifdef WIFI_UDP_CONTROLLER_USE
  WiFi.softAP("ESP32", "esp32pass");
  udp.begin(10000);
#endif
}

struct Point {
  int x;
  int y;
};

void ReviseMap(Dir dir) {
  Point pos[3];
  for (int i = 0; i < mapH; ++i) {
    for (int j = 0; j < mapW; ++j) {
      if (activeMap[i][j] > 5) {  // current position
        pos[0].x = j;
        pos[0].y = i;
      }
    }
  }
  if (dir == UP) {
    pos[1].x = pos[0].x;
    pos[1].y = pos[0].y - 1;
  }
  else if (dir == RIGHT) {
    pos[1].x = pos[0].x + 1;
    pos[1].y = pos[0].y;
  }
  if (dir == DOWN) {
    pos[1].x = pos[0].x;
    pos[1].y = pos[0].y + 1;
  }
  else if (dir == LEFT) {
    pos[1].x = pos[0].x - 1;
    pos[1].y = pos[0].y;
  }
  pos[2].x = pos[1].x + (pos[1].x - pos[0].x);
  pos[2].y = pos[1].y + (pos[1].y - pos[0].y);
  if (pos[2].x < 0 || pos[2].x >= mapW || pos[2].y < 0 || pos[2].y >= mapH) pos[2].x = -1;  // invalid position
  int pic[3];
  pic[0] = activeMap[pos[0].y][pos[0].x];
  for (int i = 1; i < 3; ++i) if (pos[i].x != -1) pic[i] = activeMap[pos[i].y][pos[i].x];
  if (pic[1] == 1) return;                    // next position is wall.
  else if (pic[1] == 4 || pic[1] == 5) {      // next position is floor
    pic[0] = (pic[0] == 7) ? 4 : 5;
    pic[1] = (pic[1] == 4) ? 7 : 6;
    Walk(dir, pos[0].x, pos[0].y);
  }
  else if (pic[1] == 2 || pic[1] == 3) {      // next position is box
    if (pic[2] == 1 || pic[2] == 2 || pic[2] == 3) return;  // next next position is wall or box
    else if (pic[2] == 4 || pic[2] == 5) {    // next next position is floor
      pic[0] = (pic[0] == 7) ? 4 : 5;
      pic[1] = (pic[1] == 3) ? 7 : 6;
      pic[2] = (pic[2] == 4) ? 3 : 2;
      Push(dir, pos[0].x, pos[0].y);
    }
  }
  for (int i = 0; i < 3; ++i) activeMap[pos[i].y][pos[i].x] = pic[i];
  //for (int i = 0; i < mapH; ++i) for (int j = 0; j < mapW; ++j) DrawBlock(j, i);  // for test
}

bool SerialAction(Dir* dir, bool* restart) {
  if(M5.BtnA.wasPressed()) {
    delay(300);
    M5.update();
    if(M5.BtnC.wasPressed()) {
      *dir = DOWN;
    } else {
      *dir = LEFT;
    }
    return true;
  } else if(M5.BtnB.wasPressed()) {
    delay(300);
    M5.update();
    if(M5.BtnC.wasPressed()) {
      *restart = true;
      return false;
    } else {
      *dir = UP;
    }
    return true;
  } else if(M5.BtnC.wasPressed()) {
    delay(300);
    M5.update();
    if(M5.BtnA.wasPressed()) {
      *dir = DOWN;
    } else if(M5.BtnB.wasPressed()) {
      *restart = true;
      return false;
    } else {
       *dir = RIGHT;
    }
    return true;
  }  
  if (!Serial.available()) return false;
  char r;
  r = Serial.read();
  Serial.println(r);
  if (r == '8') *dir = UP;
  else if (r == '6') *dir = RIGHT;
  else if (r == '2') *dir = DOWN;
  else if (r == '4') *dir = LEFT;
  else if (r == 'r') *restart = true;
  if (r == '8' || r == '6' || r == '2' || r == '4') return true;
  else return false;
}

#ifdef WIFI_UDP_CONTROLLER_USE
bool UdpAction(Dir* dir, bool* restart) {
  if (!udp.parsePacket()) return false;
  char r;
  r = udp.read();  // key is from 0 to 6 : "↑", "←", "→", "↓", "B", "A", "Release A or B"
  if (r == 0) *dir = UP;
  else if (r == 2) *dir = RIGHT;
  else if (r == 3) *dir = DOWN;
  else if (r == 1) *dir = LEFT;
  else if (r == 4) *restart = true;
  if (r == 0 || r == 1 || r == 2 || r == 3) return true;
  else return false;
}
#endif

void loop() {
  static bool restart = false;
  static int stage = 0;
  static int prev_stage = -1;
  M5.update();
  if (stage != prev_stage || restart) {
    M5.Lcd.fillScreen(backgroundColor);
    StageChange(stage);
    for (int i = 0; i < mapH; ++i) for (int j = 0; j < mapW; ++j) activeMap[i][j] = maps[stage][i][j];
    for (int i = 0; i < mapH; ++i) {
      for (int j = 0; j < mapW; ++j) {
        int cell = activeMap[i][j];
        if (cell == 2) cell = 5;
        else if (cell == 3) cell = 4;
        else if (cell == 6) cell = 5;
        else if (cell == 7) cell = 4;
        staticMap[i][j] = cell;
      }
    }
    for (int i = 0; i < mapH; ++i) for (int j = 0; j < mapW; ++j) DrawBlock(j, i);
    delay(1000);
    prev_stage = stage;
    restart = false;
  }
  Dir dir;
  if (SerialAction(&dir, &restart)) ReviseMap(dir);
#ifdef WIFI_UDP_CONTROLLER_USE
  else if (UdpAction(&dir, &restart)) ReviseMap(dir);
#endif

  // test
//  Dir dir[] = {LEFT, RIGHT, RIGHT, LEFT, LEFT, UP, DOWN, RIGHT, RIGHT, UP, LEFT, UP, LEFT, RIGHT, DOWN, DOWN, LEFT, DOWN, LEFT, DOWN, RIGHT, RIGHT, RIGHT, LEFT, LEFT, UP, UP, RIGHT, UP, UP, LEFT, DOWN, LEFT, DOWN, DOWN, RIGHT, DOWN, UP, LEFT, UP, UP, RIGHT, RIGHT, DOWN, LEFT, DOWN, DOWN, UP, UP, UP, RIGHT, RIGHT, RIGHT, UP, LEFT, DOWN, LEFT, LEFT, DOWN, DOWN, DOWN, RIGHT, RIGHT, UP, RIGHT, UP, UP, LEFT, LEFT, DOWN, RIGHT, LEFT, DOWN};
//  static int n = 0;
//  if (stage == 0) {
//    ReviseMap(dir[n]);
//    ++n;
//  }

  bool stageClear = true;
  for (int i = 0; i < mapH; ++i) for (int j = 0; j < mapW; ++j) if (activeMap[i][j] == 2) stageClear = false;
  if (stageClear) stage++;
  if (stage > 9) {
    ShowThankYou();
    while (1);
  }
}
