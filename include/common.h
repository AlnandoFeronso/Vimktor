#pragma once
#include "format"
#include <cstdint>
#include <inttypes.h>
#include <string>
#define DEBUG_MODE 1
#define COLOR 1

enum VimktorErr_t {
  VIMKTOR_OK,
  MEMORY_ERROR,
  INVALID_ARRGUMENT,
  NULLEXCEPT,
  FILE_ERROR,
  EOL_ERROR
};

enum VimktorMode_t { INSERT, VISUAL, NORMAL, EXIT, FILES };

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
  positionStruct(positionStruct const &&other) {
    x = other.x;
    y = other.y;
  }
  positionStruct(positionStruct const &other) {
    x = other.x;
    y = other.y;
  }
  inline operator std::string() { return std::format("x: {}, y: {}", x, y); }

  inline positionStruct &operator=(const positionStruct &&other) {
    x = other.x;
    y = other.y;
    return *this;
  }

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
};
