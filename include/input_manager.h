#pragma once
// singleton ?
#include "common.h"
#include <ncurses.h>

#define KEY_ESCAPE 27 // in ncurses ESC and R_ALT have same int value
#define KEY_ENTER_VIMKTOR 13
#define KEY_CTRL(k) ((k) & 0x1f)
class InputManager {
public:
  static InputManager &Get() {
    static InputManager _instance;
    return _instance;
  };
  void GetCmdBuffer() = delete;
  inline uint32_t GetChar() const noexcept { return inputCh; }
  VimktorEvent_t GetEvent(WINDOW *win, VimktorMode_t mode);
  InputManager(const InputManager &) = delete;
  InputManager &operator=(const InputManager &) = delete;

private:
  int32_t inputCh;

  VimktorEvent_t IsEscapePressed();
  VimktorEvent_t HandleDeleteEvent(WINDOW *win);

  VimktorEvent_t GetInputFileExp(WINDOW *win);
  VimktorEvent_t GetInputInsert(WINDOW *win);
  VimktorEvent_t GetInputNormal(WINDOW *win);
  InputManager() { inputCh = 0; }
};
