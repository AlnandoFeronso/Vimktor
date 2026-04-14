#pragma once

#include "common.h"
#include "curses.h"
#include "sequence.h"
#include <memory>
class Window {
public:
  Window() = delete;
  virtual ~Window();
  virtual VimktorErr_t Render() = 0;

private:
  WINDOW *m_window = NULL;
  std::unique_ptr<Sequence> m_sequence;

  VimktorErr_t InitCurses();
  VimktorErr_t OpenEmpty();
  VimktorErr_t RenderHelper();
  void HelperLog(const std::string &msg);
  VimktorErr_t RenderCursor();
  VimktorErr_t RenderText(uint16_t x, uint16_t y, uint16_t width,
                          uint16_t height);
  position_t GetEditorDimensions();
  VimktorErr_t GetInput();
  VimktorErr_t HandleEvents(VimktorEvent_t event);
  VimktorErr_t HandleCommands();
  std::string GetModeStr() const;
};

class ExploerWindow : public Window {
public:
  VimktorErr_t ExplorePath();
  VimktorErr_t ExplorePath(const std::string &path_str);
  VimktorErr_t OpenFileCursor();

private:
};

class EditorWindow : public Window {
public:
  EditorWindow();
  VimktorErr_t Render() override;
  VimktorErr_t LoadFile(const std::string &fileName);
  VimktorErr_t WriteFile(const std::string &fileName);
  VimktorErr_t WriteFile();

public:
  VimktorErr_t RenderLineNumber();
};

class FormWindow : public Window {
public:
  FormWindow();
  VimktorErr_t Render() override;
  std::string GetAnswer();
};

class CollabWindow : public Window {};
