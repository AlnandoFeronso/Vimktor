#pragma once
#include "format"
#include <cstdint>
#include <inttypes.h>
#include <string>
#include <unordered_map>
#define DEBUG_MODE 1
#define COLOR 1

// TODO: create config system for now definitions should be here

const uint8_t HELPER_HEIGHT = 2;
const uint8_t LINE_NUM_WIDTH = 4;
// TODO: Macro checking, something like ESP_ERR_CECK :)
enum VimktorErr_t {
  VIMKTOR_OK,
  MEMORY_ERROR,
  INVALID_ARRGUMENT,
  NULLEXCEPT,
  FILE_ERROR,
  EOL_ERROR
};

enum VimktorMode_t { INSERT, VISUAL, NORMAL, EXIT, FILES };

enum VimktorWindowType_t {
  COLLAB,
  EDITOR,
  EXPLORER,
  FORM,
};

enum CursorDirection {
  LEFT,
  RIGHT,
  UP,
  DOWN,
  NONE,
};

typedef struct positionStruct {
  int32_t x;
  int32_t y;
  positionStruct() : x(0), y(0) {}
  positionStruct(uint32_t x, uint32_t y) : x(x), y(y) {}
  // positionStruct(positionStruct const &&other) {
  //   x = other.x;
  //   y = other.y;
  // }
  positionStruct(positionStruct const &other) {
    x = other.x;
    y = other.y;
  }
  inline operator std::string() { return std::format("x: {}, y: {}", x, y); }

  // inline positionStruct &operator=(const positionStruct &&other) {
  //   x = other.x;
  //   y = other.y;
  //   return *this;
  // }

  inline positionStruct &operator=(const positionStruct &other) {
    x = other.x;
    y = other.y;
    return *this;
  }

  inline positionStruct &operator-(positionStruct &other) {
    x -= other.x;
    y -= other.y;
    return *this;
  }

  inline positionStruct &operator+(positionStruct &other) {
    x += other.x;
    y += other.y;
    return *this;
  }

} position_t;

enum VimktorEvent_t {
  EV_OPEN_EDITOR,
  EV_OPEN_COLAB,
  EV_CHANGE_WINDOW_MENU,
  EV_CHANGE_WINDOW_LEFT,
  EV_CHANGE_WINDOW_RIGHT,
  EV_CURSOR_UP,
  EV_CURSOR_DOWN,
  EV_CURSOR_LEFT,
  EV_CURSOR_RIGHT,
  EV_ADD_LETTER,
  EV_NONE,
  EV_CLOSE,
  EV_GO_TO_EOL,
  EV_GO_TO_SOL,
  EV_GO_TO_POS,
  EV_MODE_INSERT,
  EV_MODE_INSERT_RIGHT,
  EV_MODE_NORMAL,
  EV_INSERT_TEXT,
  EV_BACKSPACE,
  EV_ERASE_LINE,
  EV_SAVE_FILE,
  EV_NEW_LINE,
  EV_GET_COMMAND,
  EV_GO_TO_NEXT_WORD,
  EV_FILE_EXPLORER,
  EV_ENTER_CURSOR_DIRECTORY,
  EV_COLAB,
  EV_NEW_WINDOW_HORIZONTAL,
  EV_NEW_WINDOW_VERTICAL,
};

constexpr std::string_view VimktorEventToString(VimktorEvent_t event) {
  switch (event) {
  case EV_CURSOR_UP:
    return "EV_CURSOR_UP";
  case EV_CURSOR_DOWN:
    return "EV_CURSOR_DOWN";
  case EV_CURSOR_LEFT:
    return "EV_CURSOR_LEFT";
  case EV_CURSOR_RIGHT:
    return "EV_CURSOR_RIGHT";
  case EV_ADD_LETTER:
    return "EV_ADD_LETTER";
  case EV_NONE:
    return "EV_NONE";
  case EV_CLOSE:
    return "EV_CLOSE";
  case EV_GO_TO_EOL:
    return "EV_GO_TO_EOL";
  case EV_GO_TO_SOL:
    return "EV_GO_TO_SOL";
  case EV_GO_TO_POS:
    return "EV_GO_TO_POS";
  case EV_MODE_INSERT:
    return "EV_MODE_INSERT";
  case EV_MODE_INSERT_RIGHT:
    return "EV_MODE_INSERT_RIGHT";
  case EV_MODE_NORMAL:
    return "EV_MODE_NORMAL";
  case EV_INSERT_TEXT:
    return "EV_INSERT_TEXT";
  case EV_BACKSPACE:
    return "EV_BACKSPACE";
  case EV_ERASE_LINE:
    return "EV_ERASE_LINE";
  case EV_SAVE_FILE:
    return "EV_SAVE_FILE";
  case EV_NEW_LINE:
    return "EV_NEW_LINE";
  case EV_GET_COMMAND:
    return "EV_GET_COMMAND";
  case EV_GO_TO_NEXT_WORD:
    return "EV_GO_TO_NEXT_WORD";
  case EV_FILE_EXPLORER:
    return "EV_FILE_EXPLORER";
  case EV_ENTER_CURSOR_DIRECTORY:
    return "EV_ENTER_CURSOR_DIRECTORY";
  case EV_COLAB:
    return "EV_COLAB";
  case EV_NEW_WINDOW_HORIZONTAL:
    return "EV_NEW_WINDOW_HORIZONTAL";
  case EV_NEW_WINDOW_VERTICAL:
    return "EV_NEW_WINDOW_VERTICAL";
  default:
    return "UNKNOWN_EVENT";
  }
}

typedef std::unordered_map<std::string, VimktorEvent_t> CommandList_t;
const CommandList_t commandList = {
    {"w", EV_SAVE_FILE}, {"Explore", EV_FILE_EXPLORER},
    {"q", EV_CLOSE},     {"ala", EV_NEW_WINDOW_HORIZONTAL},
    {"Colab", EV_COLAB},
};
