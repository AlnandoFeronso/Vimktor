#pragma once

#include "common.h"
#include "curses.h"
#include "sequence.h"
#include <memory>
class Window {
public:
  Window()
      : m_window(stdscr), m_position(position_t(0, 0)),
        m_sequence(std::make_unique<Sequence>()) {}
  virtual ~Window();
  virtual VimktorErr_t Render();

  // private:
  //  fields
  WINDOW *m_window = nullptr;
  std::unique_ptr<Sequence> m_sequence;
  position_t m_position;
  VimktorMode_t m_mode = NORMAL;
  VimktorEvent_t m_current_event = EV_NONE;
  WINDOW *getWindow() const { return m_window; }

  VimktorErr_t RenderText(uint16_t x, uint16_t y, uint16_t width,
                          uint16_t height); // DONE
  position_t GetWindowDimensions();         // DONE
  VimktorErr_t RenderHelper();              // DONE
  WINDOW *SplitHorizontal();                // TODO: AFTER VIMKTOR CREATION
  WINDOW *SplitVertical();                  // DONE
  VimktorErr_t RenderCursor();              // DONE
  VimktorErr_t RenderLineNumber();          // DONE
  void HelperLog(const std::string &msg);   // DONE

  virtual std::string GetFileName() const;
  std::string GetModeStr() const;
  VimktorEvent_t GetCurrentEvent() const { return m_current_event; };
  virtual VimktorEvent_t HandleInput() = 0;
  virtual VimktorErr_t HandleEvents(VimktorEvent_t event) = 0;
  virtual VimktorErr_t HandleCommands() = 0;

  virtual VimktorErr_t ExplorePath() = 0;
  virtual VimktorErr_t ExplorePath(const std::string &path_str) = 0;
  virtual VimktorErr_t OpenFileCursor() = 0;
  virtual VimktorErr_t LoadFile(const std::string &fileName) = 0;
  virtual VimktorErr_t WriteFile(const std::string &fileName) = 0;
  virtual VimktorErr_t WriteFile() = 0;
};

class ExploreWindow : public Window {
public:
  ExploreWindow();
  ExploreWindow(Window *parent, const size_t width, const size_t height);
  ExploreWindow(const size_t width, const size_t height);
  virtual ~ExploreWindow() = default;
  virtual VimktorEvent_t HandleInput() override;
  virtual VimktorErr_t HandleEvents(VimktorEvent_t event) override;
  virtual VimktorErr_t HandleCommands() override;
  virtual VimktorErr_t LoadFile(const std::string &fileName) override;
  virtual VimktorErr_t WriteFile(const std::string &fileName) override;
  virtual VimktorErr_t WriteFile() override;
  virtual VimktorErr_t ExplorePath() override;
  virtual VimktorErr_t ExplorePath(const std::string &path_str) override;
  virtual VimktorErr_t OpenFileCursor() override;

private:
};

class EditorWindow : public Window {
public:
  EditorWindow(Window *parent, const size_t width, const size_t height);
  EditorWindow(const size_t width, const size_t height);
  EditorWindow(const std::string &fileName);
  EditorWindow();
  VimktorErr_t Render() override;

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
