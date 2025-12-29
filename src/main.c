#include <raylib.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* RayLib GUI Experiments: Text, Fonts (ASCII plus codepoints U+00a1 to U+024f),
 * Images, Buttons, Labels, Callbacks */

#define SCREEN_WIDTH 1350
#define SCREEN_HEIGHT 800
#define APP_TITLE "RayLib Test 3"
#define APP_ICON "icon.png"
#define APP_BACKGROUND_COLOR \
  (Color) {                  \
    230, 230, 230, 255       \
  }
#define TARGET_FPS 60
#define FONT_COUNT 3
// #define FONT_PATH "/usr/share/fonts/truetype/noto/NotoSans-Regular.ttf"
#define FONT_PATH "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf"
// #define FONT_PATH "/usr/share/fonts/truetype/msttcorefonts/Comic_Sans_MS.ttf"
// #define FONT_PATH "/home/hp/.fonts/calibri.ttf"
#define FONT_SIZES \
  { 48, 36, 24 }
#define MAX_WIDGETS 20

int main(void);
static void Startup(void);
static void Update(void);
static void Render(void);
static void Shutdown(void);
static void AddCodepointRange(Font* font,
                              const char* fontPath,
                              int start,
                              int stop);
static void doButton(const char* text,
                     Vector2 buttpos,
                     Vector2 buttdim,
                     Font font,
                     Color textcolor,
                     bool mouse_on_widget);
static void click(size_t id);
static void print(size_t id);

typedef enum { NONE = 0, LABEL, BUTTON, IMAGE } WidgetType;

typedef void (*callback_print_func)(size_t widget_index);
typedef void (*callback_event_func)(size_t widget_index);

typedef struct {
  WidgetType type;
  char filename[128];     // IMAGE
  Texture2D texture;      // IMAGE
  char text[256];         // LABEL, BUTTON
  Vector2 position;       // all
  Color backgroundcolor;  // LABEL
  Color textcolor;        // LABEL, BUTTON
  Vector2 size;           // BUTTON
  int fontindex;          // LABEL, BUTTON
  bool mouse_on_widget;
  callback_print_func print_event_fn;  // LABEL, BUTTON
  callback_event_func click_event_fn;  // LABEL, BUTTON, IMAGE
} WidgetData;

WidgetData widgets[MAX_WIDGETS] = {
    {
        // 0
        .type = IMAGE,
        .filename = "icon.png",
        .position = {1000, 100},
        .click_event_fn = click,
    },
    {
        // 1
        .type = IMAGE,
        .filename = "cat.jpg",
        .position = {1000, 300},
        .click_event_fn = click,
    },
    {
        // 2
        .type = LABEL,
        .text =
            "SIZE="
            "48\nɏqwertzuiopüasdfghjklöäyxcvbnm\nQWERTZUIOPÜASDFGHJKLÖÄYXCVB"
            "NM\nß^"
            "°1234567890!\"§$%&/\n()=?`'*+'#_-:.;,><|\n~\\}][{³²@€~µáàâéèê",
        .backgroundcolor = WHITE,
        .position = {100, 50},
        .textcolor = BLACK,
        .fontindex = 0,
    },
    {
        // 3
        .type = LABEL,
        .text =
            "SIZE="
            "36 CLICK "
            "ME!\nɏqwertzuiopüasdfghjklöäyxcvbnm\nQWERTZUIOPÜASDFGHJKLÖÄYXCVB"
            "NM\nß^"
            "°1234567890!\"§$%&/\n()=?`'*+'#_-:.;,><|\n~\\}][{³²@€~µáàâéèê",
        .backgroundcolor = YELLOW,
        .position = {100, 360},
        .textcolor = BLACK,
        .fontindex = 1,
        .click_event_fn = click,
        .print_event_fn = print,
    },
    {
        // 4
        .type = LABEL,
        .text =
            "SIZE="
            "24\nɏqwertzuiopüasdfghjklöäyxcvbnm\nQWERTZUIOPÜASDFGHJKLÖÄYXCVB"
            "NM\nß^"
            "°1234567890!\"§$%&/\n()=?`'*+'#_-:.;,><|\n~\\}][{³²@€~µáàâéèê",
        .backgroundcolor = LIGHTGRAY,
        .position = {100, 600},
        .textcolor = BLACK,
        .fontindex = 2,
    },
    {
        // 5
        .type = BUTTON,
        .position = {1000, 500},
        .size = {250, 70},
        .textcolor = BLACK,
        .fontindex = 1,
        .print_event_fn = print,
        .click_event_fn = click,
    },
    {
        // 6
        .type = BUTTON,
        .position = {1000, 600},
        .size = {250, 70},
        .textcolor = BLUE,
        .fontindex = 1,
        .print_event_fn = print,
        .click_event_fn = click,
    },
    {
        // 7
        .type = BUTTON,
        .position = {1000, 700},
        .size = {250, 70},
        .textcolor = RED,
        .fontindex = 2,
        .print_event_fn = print,
        .click_event_fn = click,
    },
    {
        // 8
        .type = LABEL,
        .text = "A lightning!",
        .backgroundcolor = APP_BACKGROUND_COLOR,
        .position = {1200, 100},
        .textcolor = APP_BACKGROUND_COLOR,
        .fontindex = 2,
        .print_event_fn = print,
    },
    {
        // 9
        .type = LABEL,
        .text = "A cat!",
        .backgroundcolor = APP_BACKGROUND_COLOR,
        .position = {1200, 300},
        .textcolor = APP_BACKGROUND_COLOR,
        .fontindex = 2,
        .print_event_fn = print,
    },
};
Font font[FONT_COUNT];

Vector2 mousepos;
bool mousepressed;

int state1 = 0;
int state2 = 0;
int state3 = 0;
bool state4 = false;
bool state5 = false;
bool state6 = false;

int main(void) {
  Startup();
  while (!WindowShouldClose()) {
    Update();
    BeginDrawing();
    Render();
    EndDrawing();
  }
  Shutdown();
  return 0;
}

static void Startup(void) {
  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, APP_TITLE);
  // Not resizable
  ClearWindowState(FLAG_WINDOW_RESIZABLE);
  SetTargetFPS(TARGET_FPS);
  // Icon
  Image img = LoadImage(APP_ICON);
  SetWindowIcon(img);
  // Center Window
  int monitor = GetCurrentMonitor();
  int monitorWidth = GetMonitorWidth(monitor);
  int monitorHeight = GetMonitorHeight(monitor);
  SetWindowPosition(monitorWidth / 2 - SCREEN_WIDTH / 2,
                    monitorHeight / 2 - SCREEN_HEIGHT / 2);
  // Fonts
  int fontsizes[FONT_COUNT] = FONT_SIZES;
  for (size_t i = 0; i < FONT_COUNT; i++) {
    font[i] = LoadFontEx(FONT_PATH, fontsizes[i], NULL, 0);
    AddCodepointRange(&font[i], FONT_PATH, 0xa1, 0x24f);
    SetTextureFilter(font[i].texture, TEXTURE_FILTER_BILINEAR);
  }
  SetTextLineSpacing(0);
  // Widgets
  for (size_t i = 0; i < MAX_WIDGETS; i++) {
    switch (widgets[i].type) {
      case IMAGE:
        widgets[i].texture = LoadTexture(widgets[i].filename);
        widgets[i].size =
            (Vector2){widgets[i].texture.width, widgets[i].texture.height};
        break;
      case LABEL:
        widgets[i].size =
            MeasureTextEx(font[widgets[i].fontindex], widgets[i].text,
                          font[widgets[i].fontindex].baseSize,
                          font[widgets[i].fontindex].baseSize / 24);
        break;
      default:
        break;
    }
  }
}

static void Update(void) {
  mousepos = GetMousePosition();
  bool mousereleased = IsMouseButtonReleased(MOUSE_BUTTON_LEFT);
  mousepressed = IsMouseButtonDown(MOUSE_BUTTON_LEFT);
  for (size_t i = 0; i < MAX_WIDGETS; i++)
    widgets[i].mouse_on_widget = false;
  bool notfound = true;
  for (size_t i = 0; i < MAX_WIDGETS && notfound; i++) {
    switch (widgets[i].type) {
      case LABEL:
      case IMAGE:
      case BUTTON:
        if (CheckCollisionPointRec(
                mousepos,
                (Rectangle){widgets[i].position.x, widgets[i].position.y,
                            widgets[i].size.x, widgets[i].size.y})) {
          if (mousereleased && widgets[i].click_event_fn)
            widgets[i].click_event_fn(i);
          widgets[i].mouse_on_widget = true;
          notfound = false;
        }
        break;
      default:
        break;
    }
  }
}

static void Render(void) {
  ClearBackground(APP_BACKGROUND_COLOR);
  // DrawFPS(1, 1);
  for (size_t i = 0; i < MAX_WIDGETS; i++) {
    switch (widgets[i].type) {
      case IMAGE:
        DrawTexture(widgets[i].texture, widgets[i].position.x,
                    widgets[i].position.y, WHITE);
        break;
      case BUTTON:
        if (widgets[i].print_event_fn)
          widgets[i].print_event_fn(i);
        doButton(widgets[i].text, widgets[i].position, widgets[i].size,
                 font[widgets[i].fontindex], widgets[i].textcolor,
                 widgets[i].mouse_on_widget);
        break;
      case LABEL:
        if (widgets[i].print_event_fn)
          widgets[i].print_event_fn(i);
        DrawRectangleV(widgets[i].position, widgets[i].size,
                       widgets[i].backgroundcolor);
        DrawTextEx(font[widgets[i].fontindex], widgets[i].text,
                   widgets[i].position, font[widgets[i].fontindex].baseSize,
                   font[widgets[i].fontindex].baseSize / 24,
                   widgets[i].textcolor);
        break;
      default:
        break;
    }
  }
}

static void Shutdown(void) {
  for (size_t i = 0; i < MAX_WIDGETS; i++) {
    if (widgets[i].type == IMAGE) {
      UnloadTexture(widgets[i].texture);
    }
  }
  for (size_t i = 0; i < FONT_COUNT; i++)
    UnloadFont(font[i]);
  CloseWindow();
}

static void AddCodepointRange(Font* font,
                              const char* fontPath,
                              int start,
                              int stop) {
  int rangeSize = stop - start + 1;
  int currentRangeSize = font->glyphCount;
  int updatedCodepointCount = currentRangeSize + rangeSize;
  int* updatedCodepoints = (int*)calloc(updatedCodepointCount, sizeof(int));
  for (int i = 0; i < currentRangeSize; i++)
    updatedCodepoints[i] = font->glyphs[i].value;
  for (int i = currentRangeSize; i < updatedCodepointCount; i++)
    updatedCodepoints[i] = start + (i - currentRangeSize);
  UnloadFont(*font);
  *font = LoadFontEx(fontPath, font->baseSize, updatedCodepoints,
                     updatedCodepointCount);
  free(updatedCodepoints);
}

static void doButton(const char* text,
                     Vector2 buttpos,
                     Vector2 buttdim,
                     Font font,
                     Color textcolor,
                     bool mouse_on_widget) {
  Color c1, c2, c3;
  float delta = 0.0f;
  Vector2 textdim =
      MeasureTextEx(font, text, font.baseSize, font.baseSize / 24);
  c2 = WHITE;
  c3 = BLACK;
  c1 = LIGHTGRAY;
  if (mouse_on_widget) {
    c1 = (Color){190, 190, 190, 255};
    if (mousepressed) {
      c2 = BLACK;
      c3 = WHITE;
      delta = -2.0f;
      c1 = (Color){175, 175, 175, 255};
    }
  }
  DrawRectangleV(buttpos, buttdim, c1);
  DrawLineEx((Vector2){buttpos.x, buttpos.y},
             (Vector2){buttpos.x + buttdim.x + 1, buttpos.y}, 2, c2);
  DrawLineEx((Vector2){buttpos.x, buttpos.y},
             (Vector2){buttpos.x, buttpos.y + buttdim.y + 1}, 2, c2);
  DrawLineEx((Vector2){buttpos.x + 1, buttpos.y + buttdim.y},
             (Vector2){buttpos.x + buttdim.x + 1, buttpos.y + buttdim.y}, 2,
             c3);
  DrawLineEx((Vector2){buttpos.x + buttdim.x, buttpos.y + 1},
             (Vector2){buttpos.x + buttdim.x, buttpos.y + buttdim.y + 1}, 2,
             c3);
  DrawTextEx(font, text,
             (Vector2){buttpos.x + buttdim.x / 2 - textdim.x / 2 + delta,
                       buttpos.y + buttdim.y / 2 - textdim.y / 2 + delta},
             font.baseSize, font.baseSize / 24, textcolor);
}

static void click(size_t id) {
  if (id == 5)
    state1++;
  if (id == 6)
    state2++;
  if (id == 7)
    state3++;
  if (id == 0)
    state4 = !state4;
  if (id == 1)
    state5 = !state5;
  if (id == 3)
    state6 = !state6;
}

static void print(size_t id) {
  if (id > 4 && id < 8) {
    int state = 0;
    if (id == 5)
      state = state1;
    if (id == 6)
      state = state2;
    if (id == 7)
      state = state3;
    sprintf(widgets[id].text, "Click me! (%d)", state);
  }
  if (id == 8)
    widgets[id].textcolor = state4 ? BLACK : APP_BACKGROUND_COLOR;
  if (id == 9)
    widgets[id].textcolor = state5 ? BLACK : APP_BACKGROUND_COLOR;
  if (id == 3) {
    widgets[id].backgroundcolor = state6 ? LIME : YELLOW;
    widgets[id].textcolor = state6 ? WHITE : BLACK;
  }
}
