#include <raylib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* RayLib GUI Experiments: Text, Fonts (ASCII plus codepoints U+00a1 to U+024f),
 * Images, Buttons, Labels, Panels, Input fields, Callbacks */

#define SCREEN_WIDTH 1350
#define SCREEN_HEIGHT 800
#define APP_TITLE "Raylib GUI Experiments"
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
static void paintRectangle(Vector2 pos,
                           Vector2 dim,
                           Color bgcolor,
                           bool hover,
                           bool sunken,
                           bool active);
static void paintTextButton(const char* text,
                            Vector2 pos,
                            Vector2 dim,
                            Font font,
                            Color textcolor,
                            bool hover);
static size_t u32_to_utf8(const uint32_t cp, char out[5]);
static void remove_last_utf8_char(char* str);
static size_t utf8_strlen(const char* s);
static void set_input_cursor(bool yes, char* text);
static void process_keys(void);
static void stoptyping(void);
static void click(size_t id);
static void print(size_t id);

typedef enum {
  NONE = 0,
  LABEL,
  BUTTON,
  IMAGE,
  CROSSBUTTON,
  PANEL,
  INPUT
} WidgetType;

typedef void (*callback_print_func)(size_t widget_index);
typedef void (*callback_event_func)(size_t widget_index);

typedef struct {
  WidgetType type;
  char filename[128];                  // IMAGE
  Texture2D texture;                   // IMAGE
  char text[256];                      // LABEL, BUTTON, INPUT
  size_t textmaxcount;                 // INPUT
  Vector2 position;                    // all
  Color backgroundcolor;               // LABEL, PANEL, BUTTON
  Color textcolor;                     // LABEL, BUTTON, CROSSBUTTON, INPUT
  Vector2 size;                        // BUTTON, PANEL
  size_t fontindex;                    // LABEL, BUTTON, CROSSBUTTON, TEXT
  bool sunken;                         // PANEL
  bool mouse_on_widget;                // all
  bool typing;                         // INPUT
  callback_print_func print_event_fn;  // LABEL, BUTTON
  callback_event_func click_event_fn;  // LABEL, BUTTON, IMAGE, CROSSBUTTON
} WidgetData;

WidgetData widgets[MAX_WIDGETS] = {
    {
        // 0
        .type = PANEL,
        .position = {1090, 40},
        .size = {148, 148},
        .backgroundcolor = WHITE,
        .sunken = true,
    },
    {
        // 1
        .type = PANEL,
        .position = {1090, 240},
        .size = {148, 148},
        .backgroundcolor = DARKGRAY,
        .sunken = false,
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
        .position = {1100, 200},
        .textcolor = APP_BACKGROUND_COLOR,
        .fontindex = 2,
        .print_event_fn = print,
    },
    {
        // 9
        .type = LABEL,
        .text = "A cat!",
        .backgroundcolor = APP_BACKGROUND_COLOR,
        .position = {1100, 400},
        .textcolor = APP_BACKGROUND_COLOR,
        .fontindex = 2,
        .print_event_fn = print,
    },
    {
        // 10
        .type = CROSSBUTTON,
        .position = {SCREEN_WIDTH - 32, 5},
        .textcolor = BLACK,
        .fontindex = 2,
        .click_event_fn = click,
    },
    {
        // 11
        .type = IMAGE,
        .filename = "cat.jpg",
        .position = {1100, 250},
        .click_event_fn = click,
    },
    {
        // 12
        .type = IMAGE,
        .filename = "icon.png",
        .position = {1100, 50},
        .click_event_fn = click,
    },
    {
        // 13
        .type = INPUT,
        .position = {600, 600},
        .text = "123.456",
        .textmaxcount = 10,
        .fontindex = 2,
        .textcolor = BLACK,
    },
    {
        // 14
        .type = INPUT,
        .position = {600, 650},
        .text = "ÄÖÜäöüß",
        .textmaxcount = 15,
        .fontindex = 2,
        .textcolor = RED,
    },
    {
        // 15
        .type = LABEL,
        .text = "Inputted data:\n...",
        .backgroundcolor = BLANK,
        .position = {600, 700},
        .textcolor = BLACK,
        .fontindex = 2,
        .print_event_fn = print,
    },
};
Font font[FONT_COUNT];

Vector2 mousepos;
bool mousepressed;
bool exitstate = false;
int typing_widget = -1;

int state1 = 0;
int state2 = 0;
int state3 = 0;
bool state4 = false;
bool state5 = false;
bool state6 = false;

int main(void) {
  Startup();
  while (!WindowShouldClose() && !exitstate) {
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
      case CROSSBUTTON:
        widgets[i].size =
            MeasureTextEx(font[widgets[i].fontindex], " x ",
                          font[widgets[i].fontindex].baseSize,
                          font[widgets[i].fontindex].baseSize / 24);
        widgets[i].size =
            (Vector2){widgets[i].size.x + 1, widgets[i].size.y + 1};
        break;
      case INPUT:
        widgets[i].size =
            MeasureTextEx(font[widgets[i].fontindex], "M",
                          font[widgets[i].fontindex].baseSize,
                          font[widgets[i].fontindex].baseSize / 24);
        widgets[i].size =
            (Vector2){widgets[i].size.x * widgets[i].textmaxcount + 5,
                      widgets[i].size.y + 5};
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
  if (mousereleased)
    stoptyping();
  if (typing_widget >= 0)
    process_keys();
  for (size_t i = 0; i < MAX_WIDGETS; i++)
    widgets[i].mouse_on_widget = false;
  bool notfound = true;
  for (size_t i = 0; i < MAX_WIDGETS && notfound; i++) {
    switch (widgets[i].type) {
      case LABEL:
      case CROSSBUTTON:
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
      case INPUT:
        if (CheckCollisionPointRec(
                mousepos,
                (Rectangle){widgets[i].position.x, widgets[i].position.y,
                            widgets[i].size.x, widgets[i].size.y})) {
          if (mousereleased) {
            typing_widget = i;
            widgets[i].typing = true;
            set_input_cursor(true, widgets[i].text);
          }
        }
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
      case PANEL:
        paintRectangle(widgets[i].position, widgets[i].size,
                       widgets[i].backgroundcolor, false, widgets[i].sunken,
                       false);
        break;
      case IMAGE:
        DrawTexture(widgets[i].texture, widgets[i].position.x,
                    widgets[i].position.y, WHITE);
        break;
      case BUTTON:
        if (widgets[i].print_event_fn)
          widgets[i].print_event_fn(i);
        paintTextButton(widgets[i].text, widgets[i].position, widgets[i].size,
                        font[widgets[i].fontindex], widgets[i].textcolor,
                        widgets[i].mouse_on_widget);
        break;
      case CROSSBUTTON:
        paintTextButton(" x ", widgets[i].position, widgets[i].size,
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
      case INPUT:
        paintRectangle(widgets[i].position, widgets[i].size, BLANK, false, true,
                       widgets[i].typing);
        DrawTextEx(
            font[widgets[i].fontindex], widgets[i].text,
            (Vector2){widgets[i].position.x + 3, widgets[i].position.y + 3},
            font[widgets[i].fontindex].baseSize,
            font[widgets[i].fontindex].baseSize / 24, widgets[i].textcolor);
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

static void paintRectangle(Vector2 pos,
                           Vector2 dim,
                           Color bgcolor,
                           bool hover,
                           bool sunken,
                           bool active) {
  Color color_shadow = (Color){0, 0, 0, 255};
  Color color_light = (Color){255, 255, 255, 255};
  Color color_shadow_active = (Color){20, 20, 20, 255};
  Color color_light_active = (Color){253, 249, 0, 255};
  Color color_normal = (Color){200, 200, 200, 255};
  Color color_hover = (Color){190, 190, 190, 255};
  Color color_sunken = (Color){175, 175, 175, 255};
  Color c1, c2, c3, csave;
  if (bgcolor.r == 0 && bgcolor.g == 0 && bgcolor.b == 0 && bgcolor.a == 0) {
    c1 = color_normal;
    if (hover)
      c1 = color_hover;
    if (sunken)
      c1 = color_sunken;
  } else
    c1 = bgcolor;
  c2 = color_light;
  c3 = color_shadow;
  if (active) {
    c2 = color_light_active;
    c3 = color_shadow_active;
  }
  if (sunken) {
    csave = c2;
    c2 = c3;
    c3 = csave;
  }
  DrawRectangleV(pos, dim, c1);
  DrawLineEx(pos, (Vector2){pos.x + dim.x + 1, pos.y}, 2, c2);
  DrawLineEx(pos, (Vector2){pos.x, pos.y + dim.y + 1}, 2, c2);
  DrawLineEx((Vector2){pos.x + 1, pos.y + dim.y},
             (Vector2){pos.x + dim.x + 1, pos.y + dim.y}, 2, c3);
  DrawLineEx((Vector2){pos.x + dim.x, pos.y + 1},
             (Vector2){pos.x + dim.x, pos.y + dim.y + 1}, 2, c3);
}

static void paintTextButton(const char* text,
                            Vector2 pos,
                            Vector2 dim,
                            Font font,
                            Color textcolor,
                            bool hover) {
  float delta = 0.0f;
  Vector2 textdim =
      MeasureTextEx(font, text, font.baseSize, font.baseSize / 24);
  if (hover && mousepressed)
    delta = -2.0f;
  paintRectangle(pos, dim, BLANK, hover, hover && mousepressed, false);
  DrawTextEx(font, text,
             (Vector2){pos.x + dim.x / 2 - textdim.x / 2 + delta,
                       pos.y + dim.y / 2 - textdim.y / 2 + delta},
             font.baseSize, font.baseSize / 24, textcolor);
}

static size_t u32_to_utf8(const uint32_t cp, char out[5]) {
  for (int i = 0; i < 5; i++)
    out[i] = '\0';
  if (cp <= 0x7F) {
    out[0] = (unsigned char)cp;
    return 1;
  }
  if (cp <= 0x7FF) {
    out[0] = 0xC0 | (cp >> 6);
    out[1] = 0x80 | (cp & 0x3F);
    return 2;
  }
  if (cp >= 0xD800 && cp <= 0xDFFF) {
    return 0;  // invalid (surrogate)
  }
  if (cp <= 0xFFFF) {
    out[0] = 0xE0 | (cp >> 12);
    out[1] = 0x80 | ((cp >> 6) & 0x3F);
    out[2] = 0x80 | (cp & 0x3F);
    return 3;
  }
  if (cp <= 0x10FFFF) {
    out[0] = 0xF0 | (cp >> 18);
    out[1] = 0x80 | ((cp >> 12) & 0x3F);
    out[2] = 0x80 | ((cp >> 6) & 0x3F);
    out[3] = 0x80 | (cp & 0x3F);
    return 4;
  }
  return 0;  // invalid (out of Unicode range)
}

static void remove_last_utf8_char(char* str) {
  if (!str || *str == '\0')
    return;
  char* p = str + strlen(str) - 1;
  while (p > str && ((*p & 0xC0) == 0x80)) {
    p--;
  }
  *p = '\0';
}

static size_t utf8_strlen(const char* s) {
  size_t len = 0;
  while (*s) {
    if ((*s & 0xC0) != 0x80) {
      len++;
    }
    s++;
  }
  return len;
}

static void set_input_cursor(bool yes, char* text) {
  int len = strlen(text);
  if (yes)
    text[len] = '|';
  else
    len = len - 2;
  text[len + 1] = '\0';
}

static void process_keys(void) {
  int key;
  char buf[5];
  while ((key = GetKeyPressed()) != 0) {
    if (KEY_BACKSPACE == key) {
      // printf("BACKSPACE %s\n",widgets[typing_widget].text);
      set_input_cursor(false, widgets[typing_widget].text);
      remove_last_utf8_char(widgets[typing_widget].text);
      set_input_cursor(true, widgets[typing_widget].text);
    }
  }
  while ((key = GetCharPressed()) != 0) {
    int l = u32_to_utf8(key, buf);
    if (l > 0) {
      set_input_cursor(false, widgets[typing_widget].text);
      if (utf8_strlen(widgets[typing_widget].text) <
          widgets[typing_widget].textmaxcount)
        strcat(widgets[typing_widget].text, buf);
      set_input_cursor(true, widgets[typing_widget].text);
    }
  }
}

static void stoptyping(void) {
  if (typing_widget >= 0) {
    set_input_cursor(false, widgets[typing_widget].text);
  }
  typing_widget = -1;
  for (size_t j = 0; j < MAX_WIDGETS; j++)
    widgets[j].typing = false;
}

static void click(size_t id) {
  if (id == 5)
    state1++;
  if (id == 6)
    state2++;
  if (id == 7)
    state3++;
  if (id == 12)
    state4 = !state4;
  if (id == 11)
    state5 = !state5;
  if (id == 3)
    state6 = !state6;
  if (id == 10)
    exitstate = true;
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
  if (id == 15) {
    widgets[id].text[0] = '\0';
    strcat(widgets[id].text, "Inputted data:\n");
    if (typing_widget != 13)
      strncat(widgets[id].text, widgets[13].text, 35);
    strcat(widgets[id].text, "\n");
    if (typing_widget != 14)
      strncat(widgets[id].text, widgets[14].text, 35);
  }
}
