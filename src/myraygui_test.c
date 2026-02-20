#include <raylib.h>
#include <stdio.h>

/* RayLib GUI Experiments: Text, Fonts (ASCII plus codepoints U+00a1
   to U+024f), Images, Buttons, Labels, Panels, Input fields,
   Scrollable Text, Callbacks.

   Version 2026-02-18 */

/* ----------------------------------------------------------------*/

// Definitions

// Convert a macro to a string (stringification)
#define STR(x) #x
#define XSTR(x) STR(x)

// Definitions

#define SCREEN_WIDTH 1370
#define SCREEN_HEIGHT 800
#define APP_TITLE "Raylib GUI Experiments"
#define APP_ICON "icon.png"
#define APP_BACKGROUND_COLOR \
  (Color) {                  \
    230, 230, 230, 255       \
  }
#define TARGET_FPS 30

#define FONT_COUNT 3
#define FONT_PATHS                                              \
  {                                                             \
    "LiberationSans-Regular.ttf", "LiberationSans-Regular.ttf", \
        "LiberationSans-Regular.ttf"                            \
  }
#define FONT_SIZES \
  { 46, 34, 22 }
#define FONT_SPACINGS \
  { 0.f, 0.f, 0.f }
#define FONT_ADD_CP_RANGES_COUNT 1
#define FONT_ADD_CP_RANGES \
  {                        \
    { 0xa1, 0x24f }        \
  }
#define TEXT_LINE_SPACING 0
#define INPUT_CURSOR '_'

#define MAX_WIDGETS 19

/* ----------------------------------------------------------------*/

#include "myraygui.h"

/* ----------------------------------------------------------------*/

int main(void);
static void click(int id);
static void print(int id);
static void init(int id);

/* ----------------------------------------------------------------*/

// Widget Definitions

static WidgetData widgets[MAX_WIDGETS] = {
    {
        // 0
        .type = PANEL,
        .position = {1190, 40},
        .size = {148, 148},
        .backgroundcolor = WHITE,
        .sunken = true,
    },
    {
        // 1
        .type = PANEL,
        .position = {1190, 240},
        .size = {148, 148},
        .backgroundcolor = DARKGRAY,
        .sunken = false,
    },
    {
        // 2
        .type = LABEL,
        .text =
            "SIZE="
            "46\nɏqwertzuiopüasdfghjklöäyxcvbnm\nQWERTZUIOPÜASDFGHJKLÖÄYXCVB"
            "NM\nß^"
            "°1234567890!\"§$%&/\n()=?`'*+'#_-:.;,><|\n~\\}][{³²@€~µáàâéèê",
        .backgroundcolor = WHITE,
        .position = {10, 50},
        .textcolor = BLACK,
        .fontindex = 0,
    },
    {
        // 3
        .type = LABEL,
        .text =
            "SIZE="
            "34 - CLICK "
            "ME!\nɏqwertzuiopüasdfghjklöäyxcvbnm\nQWERTZUIOPÜASDFGHJKLÖÄYXCVB"
            "NM\nß^"
            "°1234567890!\"§$%&/\n()=?`'*+'#_-:.;,><|\n~\\}][{³²@€~µáàâéèê",
        .backgroundcolor = YELLOW,
        .position = {10, 360},
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
            "22\nɏqwertzuiopüasdfghjklöäyxcvbnm\nQWERTZUIOPÜASDFGHJKLÖÄYXCVB"
            "NM\nß^"
            "°1234567890!\"§$%&/\n()=?`'*+'#_-:.;,><|\n~\\}][{³²@€~µáàâéèê",
        .backgroundcolor = LIGHTGRAY,
        .position = {10, 600},
        .textcolor = BLACK,
        .fontindex = 2,
    },
    {
        // 5
        .type = BUTTON,
        .position = {1100, 500},
        .size = {250, 70},
        .textcolor = BLACK,
        .fontindex = 1,
        .keycode1 = KEY_ONE,
        .print_event_fn = print,
        .click_event_fn = click,
    },
    {
        // 6
        .type = BUTTON,
        .position = {1100, 600},
        .size = {250, 70},
        .textcolor = BLUE,
        .fontindex = 1,
        .keycode1 = KEY_TWO,
        .print_event_fn = print,
        .click_event_fn = click,
    },
    {
        // 7
        .type = BUTTON,
        .position = {1100, 700},
        .size = {250, 70},
        .textcolor = RED,
        .fontindex = 2,
        .keycode1 = KEY_THREE,
        .print_event_fn = print,
        .click_event_fn = click,
    },
    {
        // 8
        .type = LABEL,
        .text = "A lightning!",
        .backgroundcolor = APP_BACKGROUND_COLOR,
        .position = {1200, 200},
        .textcolor = APP_BACKGROUND_COLOR,
        .fontindex = 2,
        .print_event_fn = print,
    },
    {
        // 9
        .type = LABEL,
        .text = "A cat!",
        .backgroundcolor = APP_BACKGROUND_COLOR,
        .position = {1200, 400},
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
        .position = {1200, 250},
        .click_event_fn = click,
    },
    {
        // 12
        .type = IMAGE,
        .filename = "icon.png",
        .position = {1200, 50},
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
        .allowed = "0123456789.",
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
    {
        // 16
        .type = TEXTSCROLLAREA,
        .textcolor = DARKPURPLE,
        .backgroundcolor = (Color){205, 235, 247, 255},
        .sunken = true,
        .position = {700, 360},
        .size = {380, 200},
        .fontindex = 2,
        .yscrollpos = 0,
        .init_event_fn = init,
    },
    {
        // 17
        .type = CLICKAREA,
        .position = {0, 0},
        .size = {10, 20},
        .keycode1 = KEY_F9,
        .keycode2 = KEY_Q,
        .click_event_fn = click,
    },
    {
        // 18
        .type = LABEL,
        .text = "<<< Click this area, or press [F9] or [Q] to quit app",
        .backgroundcolor = BLANK,
        .position = {10, 1},
        .textcolor = (Color){170, 170, 170, 255},
        .fontindex = 2,
    },
};

/* ----------------------------------------------------------------*/

// Application Data

static int state1 = 0;
static int state2 = 0;
static int state3 = 0;
static bool state4 = false;
static bool state5 = false;
static bool state6 = false;

/* ----------------------------------------------------------------*/

// Main

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

/* ----------------------------------------------------------------*/

// Callback functions

static void click(int id) {
  if (id == 5)
    state1++;
  else if (id == 6)
    state2++;
  else if (id == 7)
    state3++;
  else if (id == 12)
    state4 = !state4;
  else if (id == 11)
    state5 = !state5;
  else if (id == 3)
    state6 = !state6;
  else if (id == 10)
    exitstate = true;
  else if (id == 17)
    exitstate = true;
}

static void print(int id) {
  if (id > 4 && id < 8) {
    int state = 0;
    if (id == 5)
      state = state1;
    if (id == 6)
      state = state2;
    if (id == 7)
      state = state3;
    sprintf(widgets[id].text, "Click/Key[%d]! (%d)", id - 4, state);
  } else if (id == 8)
    widgets[id].textcolor = state4 ? BLACK : APP_BACKGROUND_COLOR;
  else if (id == 9)
    widgets[id].textcolor = state5 ? BLACK : APP_BACKGROUND_COLOR;
  else if (id == 3) {
    widgets[id].backgroundcolor = state6 ? LIME : YELLOW;
    widgets[id].textcolor = state6 ? WHITE : BLACK;
  } else if (id == 15) {
    sprintf(widgets[id].text, "Inputted data:\n%.35s\n%.35s",
            typing_widget != 13 ? widgets[13].text : "...",
            typing_widget != 14 ? widgets[14].text : "...");
  }
}

static void init(int id) {
  if (id == 16) {
    init_longtext(
        id,
        "Scroll this text via the mouse wheel!\nOr click left or right "
        "mouse button to Page-down or Page-Up!\n\n"
        "raylib is a programming library to enjoy videogames programming; "
        "no fancy interface, no visual helpers, no gui tools or editors... "
        "just coding in pure spartan-programmers way. Are you ready to "
        "enjoy coding?\n\n"
        "raylib is a programming library to enjoy videogames programming; "
        "no fancy interface, no visual helpers, no gui tools or editors... "
        "just coding in pure spartan-programmers way. Are you ready to "
        "enjoy coding?\n\n"
        "raylib is a programming library to enjoy videogames programming; "
        "no fancy interface, no visual helpers, no gui tools or editors... "
        "just coding in pure spartan-programmers way. Are you ready to "
        "enjoy coding?");
  }
}

/* ----------------------------------------------------------------*/

#define MYRAYGUI_IMPLEMENTATION
#include "myraygui.h"

/* ----------------------------------------------------------------*/
