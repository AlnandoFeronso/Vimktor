#include "include/vimktor.h"
#include "include/common.h"
#include "include/input_manager.h"
#include "include/sequence.h"
#include "include/vimktor_debug.h"
#include <cassert>
#include <cstdint>
#include <cstdio>
#include <curses.h>
#include <cwchar>
#include <filesystem>
#include <fstream>
#include <memory>
#include <string>

const uint HELPER_HEIGHT = 2;
const uint LINE_NUM_WIDTH = 5;

void Vimktor::Init() {
  InitCurses();
  int w, h;
  getmaxyx(m_window, h, w);
  m_sequence.SetPageDimensions(w - LINE_NUM_WIDTH, h - HELPER_HEIGHT);
  Debug::Log(std::format("max win w: {} , h{}", w, h));
  wrefresh(m_window);
}

void Vimktor::End() { endwin(); }

VimktorErr_t Vimktor::InitCurses() {
  initscr();
  m_window = stdscr;
  keypad(m_window, TRUE);
  raw();
  nonl();
  set_escdelay(50);
  nodelay(stdscr, false);
  noecho();
  curs_set(1);
  init_color(COLOR, 0, 0, 0);
  return VIMKTOR_OK;
}

VimktorErr_t Vimktor::RenderWindow() {
  const auto txtDim = m_sequence.GetPageDimensions();
  if (txtDim.x == 0 || txtDim.y == 0)
    return MEMORY_ERROR;

  RenderText(LINE_NUM_WIDTH, 0, txtDim.x, txtDim.y);
  RenderLineNumber();
  RenderHelper();
  RenderCursor();
  wrefresh(m_window);
  return VIMKTOR_OK;
}

VimktorErr_t Vimktor::RenderLineNumber() {
  size_t first_nr = m_sequence.m_pagePos.y;
  size_t height = m_sequence.GetPageDimensions().y;
  for (uint y = 0; y < height; y++) {
    if (first_nr + y >= m_sequence.Size()) {

      mvwprintw(m_window, y, 1, "    ");
    } else
      mvwprintw(m_window, y, 1, "%u", first_nr + y + 1);
  }

  return VIMKTOR_OK;
}

VimktorErr_t Vimktor::RenderCursor() {
  position_t cursor = m_sequence.GetRelativeCursorPos();
  wmove(m_window, cursor.y, cursor.x + LINE_NUM_WIDTH);

  return VIMKTOR_OK;
}

VimktorErr_t Vimktor::RenderHelper() {
  position_t endPoint = GetEditorDimensions();
  position_t cursorPos = m_sequence.GetCursorPos();
  size_t x = endPoint.x - 6;
  size_t y = endPoint.y - HELPER_HEIGHT;
  // print cursor position
  Debug::Log(std::format("endpoint {}  x: {}", (std::string)endPoint, x));
  mvwprintw(m_window, y, x, "     ");

  mvwprintw(m_window, y, x, "%u:%d", cursorPos.y, cursorPos.x);

  // print activ mode and file name
  mvwprintw(m_window, y, 1, "%s  %s lines: %u ", GetModeStr().c_str(),
            m_filename.c_str(), m_sequence.Size());
  return VIMKTOR_OK;
}

VimktorErr_t Vimktor::RenderText(uint16_t x, uint16_t y, uint16_t width,
                                 uint16_t height) {

  for (uint16_t i_y = y; i_y < height; i_y++) {
    for (uint16_t i_x = x; i_x < width; i_x++) {

      wmove(m_window, i_y, i_x);
      if (m_sequence.GetGlyphAtRel(i_x - x, i_y).has_value()) {
        const auto *temp = m_sequence.GetGlyphAtRel(i_x - x, i_y).value();
        waddch(m_window, temp->ch);
      } else {
        waddch(m_window, ' ');
      }
    }
  }
  // TODO: add colors

  return VIMKTOR_OK;
}
VimktorErr_t Vimktor::GetInput() {
  VimktorEvent_t event = InputManager::Get().GetEvent(m_window, m_mode);
  HandleEvents(event);
  return VIMKTOR_OK;
}

VimktorErr_t Vimktor::HandleEvents(VimktorEvent_t event) {
  VimktorErr_t err = VIMKTOR_OK;
  switch (event) {

  case EV_NONE:

    break;
  case EV_CURSOR_DOWN:
    err = m_sequence.CursorMove(DOWN);
    break;
  case EV_CURSOR_UP:
    err = m_sequence.CursorMove(UP);
    break;
  case EV_CURSOR_RIGHT:
    err = m_sequence.CursorMove(RIGHT);
    break;
  case EV_CURSOR_LEFT:
    err = m_sequence.CursorMove(LEFT);
    break;
  case EV_CLOSE:
    m_mode = EXIT;
    break;
  case EV_ERASE_LINE:
    m_sequence.EraseLineCursor();
    break;
  case EV_MODE_NORMAL:
    m_mode = NORMAL;
    break;
  case EV_MODE_INSERT:
    m_mode = INSERT;
    break;
  case EV_MODE_INSERT_RIGHT:
    m_mode = INSERT;
    m_sequence.m_mode = m_mode;
    m_sequence.CursorMove(RIGHT);
    break;
  case EV_BACKSPACE:
    m_sequence.EraseCharCursor();
    break;
  case EV_INSERT_TEXT: {
    glyph_t gl = glyph_t(InputManager::Get().GetChar());
    m_sequence.InsertCharCursor(gl);
  } break;
  case EV_GO_TO_SOL:
    m_sequence.CursorMoveSol();
    break;
  case EV_GO_TO_NEXT_WORD:
    m_sequence.CursorMoveWordNext();
    break;
  case EV_GO_TO_EOL:
    m_sequence.CursorMoveEol();
    break;
  case EV_SAVE_FILE:
    WriteFile();
    HelperLog("saved to " + m_filename);
    break;
  case EV_NEW_LINE:
    m_sequence.AddNewLineCursor();
    break;
  case EV_GET_COMMAND:
    HandleCommands();
    break;
  case EV_FILE_EXPLORER:
    WriteFile();
    ExplorePath();
    break;
  case EV_ENTER_CURSOR_DIRECTORY:
    OpenFileCursor();
    break;
  }
  return VIMKTOR_OK;
}

VimktorErr_t Vimktor::HandleCommands() {
  std::string cmd;

  char16_t ch;

  nodelay(m_window, 0);
  while (1) {
    HelperLog(":" + cmd);
    wrefresh(m_window);
    ch = wgetch(m_window);
    if (ch == KEY_ESCAPE || ch == 13 || ch == KEY_ENTER) {
      nodelay(m_window, 1);
      // ch = wgetch(m_window);
      break;
      wrefresh(stdscr);
    }
    if (ch == KEY_BACKSPACE) {
      if (cmd.size() > 0)
        cmd.pop_back();

    } else {
      cmd.push_back(ch);
    }
  }
  nodelay(m_window, 0);

  // nodelay(m_window, 1);
  HelperLog("                                           ");
  wrefresh(stdscr);
  if (commandList.contains(cmd)) {
    HandleEvents(commandList[cmd]);
  }

  return VIMKTOR_OK;
}

VimktorErr_t Vimktor::OpenEmpty() { LoadFile(".vimktor_temp"); return VIMKTOR_OK;}

VimktorErr_t Vimktor::LoadFile(const std::string &fileName) {
  if (fileName == ".") {
    ExplorePath();
    return VIMKTOR_OK;
  }
  std::fstream file;
  m_filename = fileName;
  file.open(fileName, std::ios::in);
  if (!file.good()) {
    file.close();
    file.open(m_filename, std::ios::out | std::ios::in | std::fstream::trunc);
    return FILE_ERROR;
  }
  Debug::Log("plik zapisu: " + m_filename);
  m_sequence.LoadFile(file);
  file.close();
  return VIMKTOR_OK;
};

VimktorErr_t Vimktor::WriteFile(const std::string &fileName) {
  std::fstream file;
  file.open(fileName, std::ios::out);
  if (!file.good()) {
    return FILE_ERROR;
  }
  m_sequence.WriteFile(file);
  file.close();
  return VIMKTOR_OK;
};

VimktorErr_t Vimktor::WriteFile() {
  std::fstream file;
  file.open(m_filename, std::ios::out);
  if (!file.good()) {
    return FILE_ERROR;
  }
  m_sequence.WriteFile(file);
  file.close();
  return VIMKTOR_OK;
};

void Vimktor::Loop() {
  while (m_mode != EXIT) {
    RenderWindow();
    GetInput();
    m_sequence.m_mode = m_mode;
  }
}

position_t Vimktor::GetEditorDimensions() {
  return position_t(getmaxx(m_window), getmaxy(m_window));
}

void Vimktor::HelperLog(const std::string &msg) {
  position_t endPoint = GetEditorDimensions();
  size_t y = endPoint.y - 1;
  size_t x = 0;
  wmove(m_window, y, x);
  clrtoeol();
  mvwprintw(m_window, y, x, "%s", msg.c_str());
}

Vimktor::CommandList_t Vimktor::commandList = {
    {"w", EV_SAVE_FILE},
    {"Explore", EV_FILE_EXPLORER},
    {"q", EV_CLOSE},
};

std::string Vimktor::GetModeStr() const {
  if (m_mode == VimktorMode_t::FILES)
    return "Files";
  if (m_mode == VimktorMode_t::INSERT)
    return "Insert";
  if (m_mode == VimktorMode_t::NORMAL)
    return "Normal";
  if (m_mode == VimktorMode_t::VISUAL)
    return "Visual";
  return "INVALID";
}

VimktorErr_t Vimktor::ExplorePath() {
  m_sequence.m_cursorPos = position_t(0, 0);
  m_mode = FILES;
  HelperLog(std::filesystem::current_path().string());
  m_sequence.LoadCurrentDirectory();
  return VIMKTOR_OK;
}

VimktorErr_t Vimktor::ExplorePath(const std::string &path_str) {
  m_sequence.m_cursorPos = position_t(0, 0);
  m_mode = FILES;
  std::filesystem::current_path(path_str);
  HelperLog(std::filesystem::current_path().string());
  m_sequence.LoadCurrentDirectory();
  return VIMKTOR_OK;
}

VimktorErr_t Vimktor::OpenFileCursor() {
  auto path = std::filesystem::current_path();

  std::string path_str = path.string() + '/' + m_sequence.GetStringCursor();
  if (std::filesystem::is_directory(path_str) ||
      m_sequence.GetStringCursor() == "../" ||
      m_sequence.GetStringCursor() == "./") {
    ExplorePath(path_str);
  } else {
    LoadFile(m_sequence.GetStringCursor());
    m_mode = NORMAL;
  }
  return VIMKTOR_OK;
}
