#include "include/window.h"
#include "include/common.h"
#include "include/input_manager.h"
#include "include/vimktor_debug.h"
#include <cstdint>
#include <curses.h>
#include <filesystem>
#include <ncurses.h>
#include <string>
#include <sys/types.h>

std::string Window::GetFileName() const { return "None"; }

Window::~Window() {}

VimktorErr_t Window::Render() {
  const auto txtDim = m_sequence->GetPageDimensions();
  if (txtDim.x == 0 || txtDim.y == 0)
    return MEMORY_ERROR;

  RenderText(LINE_NUM_WIDTH, 0, txtDim.x, txtDim.y);
  RenderLineNumber();
  RenderHelper();
  RenderCursor();
  wrefresh(m_window);
  return VIMKTOR_OK;
}

VimktorErr_t Window::ExplorePath() {
  HelperLog("cannot explore in " + GetModeStr());
  return VimktorErr_t::INVALID_ARRGUMENT;
}
VimktorErr_t Window::ExplorePath(const std::string &path_str) {
  HelperLog("cannot explore in " + GetModeStr());
  return VimktorErr_t::INVALID_ARRGUMENT;
}
VimktorErr_t Window::OpenFileCursor() {
  HelperLog("cannot explore in " + GetModeStr());
  return VimktorErr_t::INVALID_ARRGUMENT;
}
VimktorErr_t Window::LoadFile(const std::string &fileName) {

  std::fstream file;
  m_filename = fileName;
  file.open(fileName, std::ios::in);
  if (!file.good()) {
    file.close();
    file.open(m_filename, std::ios::out | std::ios::in | std::fstream::trunc);
    return FILE_ERROR;
  }
  m_sequence->LoadFile(file);
  file.close();
  return VIMKTOR_OK;
}

void Window::SetWindowPosition(const position_t &pos) { m_position = pos; }
position_t Window::GetWindowPosition() { return m_position; }

VimktorErr_t Window::RenderText(uint16_t x, uint16_t y, uint16_t width,
                                uint16_t height) {

  for (uint16_t i_y = 0; i_y < height; i_y++) {
    for (uint16_t i_x = 0; i_x < width; i_x++) {

      wmove(m_window, i_y + y, i_x + x);
      if (m_sequence->GetGlyphAtRel(i_x, i_y).has_value()) {
        const auto *temp = m_sequence->GetGlyphAtRel(i_x, i_y).value();
        waddch(m_window, temp->ch);
      } else {
        waddch(m_window, ' ');
      }
    }
  }
  return VIMKTOR_OK;
}

position_t Window::GetWindowDimensions() {
  return position_t(getmaxx(m_window), getmaxy(m_window));
}
VimktorErr_t Window::MoveAndResize(const position_t &pos,
                                   const position_t &dim) {
  m_position = pos;
  Resize(dim);
  return VIMKTOR_OK;
}
VimktorErr_t Window::Move(const position_t &pos) { Resize(pos); }
VimktorErr_t Window::ChangeWidth(size_t x) {
  size_t y = getmaxy(m_window);
  position_t dim(x, y);
  Resize(dim);
  return VIMKTOR_OK;
}
VimktorErr_t Window::ChangeHeight(size_t y) {
  size_t x = getmaxx(m_window);
  position_t dim(x, y);
  Resize(dim);
  return VIMKTOR_OK;
}

VimktorErr_t Window::MoveX(size_t x) {
  m_position.x = x;
  Resize();
  return VIMKTOR_OK;
}
VimktorErr_t Window::MoveY(size_t y) {
  m_position.y = y;
  Resize();
  return VIMKTOR_OK;
}
VimktorErr_t Window::Resize() {
  int x = getmaxx(m_window);
  int y = getmaxy(m_window);
  if (m_window != stdscr) {
    delwin(m_window);
  }

  m_window = newwin(y, x, m_position.y, m_position.x);
  m_sequence->SetPageDimensions(x - LINE_NUM_WIDTH, y - HELPER_HEIGHT);
  return VIMKTOR_OK;
}
VimktorErr_t Window::Resize(const position_t &dim) {
  if (m_window != stdscr) {
    delwin(m_window);
  }
  m_window = newwin(dim.y, dim.x, m_position.y, m_position.x);
  m_sequence->SetPageDimensions(dim.x - LINE_NUM_WIDTH, dim.y - HELPER_HEIGHT);
  return VIMKTOR_OK;
}

VimktorErr_t Window::RenderHelper() {
  position_t endPoint = GetWindowDimensions();
  position_t cursorPos = m_sequence->GetCursorPos();
  size_t x = endPoint.x - 6;
  size_t y = endPoint.y - HELPER_HEIGHT;
  // print cursor position
  Debug::Log(std::format("endpoint {}  x: {}", (std::string)endPoint, x));
  mvwprintw(m_window, y, x, "     ");

  mvwprintw(m_window, y, x, "%u:%d", cursorPos.y, cursorPos.x);

  // print activ mode and file name
  mvwprintw(m_window, y, 1, "%s  %s lines: %u ", GetModeStr().c_str(),
            GetFileName().c_str(), m_sequence->Size());
  return VIMKTOR_OK;
}

WINDOW *Window::SplitHorizontal() {
  if (m_window == nullptr)
    return nullptr;
  position_t dim = GetWindowDimensions();
  size_t new_heigth_my = dim.y / 2;
  size_t new_heigth_other = dim.y - new_heigth_my;
  delwin(m_window);
  m_window = newwin(m_position.y, m_position.x, new_heigth_my, dim.x);
  return newwin(m_position.y + new_heigth_my, m_position.x, new_heigth_other,
                dim.x);
}

VimktorErr_t Window::RenderLineNumber() {
  size_t first_nr = m_sequence->m_pagePos.y;
  size_t height = m_sequence->GetPageDimensions().y;
  for (uint y = 0; y < height; y++) {
    if (first_nr + y >= m_sequence->Size()) {

      mvwprintw(m_window, y, 1, "    ");
    } else
      mvwprintw(m_window, y, 1, "%u", first_nr + y + 1);
  }

  return VIMKTOR_OK;
}

VimktorErr_t Window::RenderCursor() {
  position_t cursor = m_sequence->GetRelativeCursorPos();
  Debug::Log(std::format("PATRYK: {}", (std::string)cursor));
  wmove(m_window, cursor.y, cursor.x + LINE_NUM_WIDTH);

  return VIMKTOR_OK;
}
void Window::HelperLog(std::string_view msg) {
  position_t endPoint = GetWindowDimensions();
  size_t y = endPoint.y - 1;
  size_t x = 0;
  wmove(m_window, y, x);
  clrtoeol();
  mvwprintw(m_window, y, x, "%s", msg.data());
  wrefresh(m_window);
}

// void Window::HelperLog(const std::string &msg) {
//   position_t endPoint = GetWindowDimensions();
//   size_t y = endPoint.y - 1;
//   size_t x = 0;
//   wmove(m_window, y, x);
//   clrtoeol();
//   mvwprintw(m_window, y, x, "%s", msg.c_str());
//   wrefresh(m_window);
// }
std::string Window::GetModeStr() const {
  if (m_mode == VimktorMode_t::FILES)
    return "Files";
  if (m_mode == VimktorMode_t::INSERT)
    return "Insert";
  if (m_mode == VimktorMode_t::NORMAL)
    return "Normal";
  if (m_mode == VimktorMode_t::VISUAL)
    return "Visual";
  if (m_mode == VimktorMode_t::EXIT)
    return "EXIT !!!!!! ";
  return "INVALID";
}

ExploreWindow::ExploreWindow(Window *parent, const size_t width,
                             const size_t height) {}
ExploreWindow::ExploreWindow(const size_t width, const size_t height) {}

ExploreWindow::ExploreWindow() {
  auto dim = GetWindowDimensions();
  m_sequence->SetPageDimensions(dim.x - LINE_NUM_WIDTH, dim.y - HELPER_HEIGHT);
  m_mode = FILES;
  ExplorePath();
  Render();
}

VimktorErr_t ExploreWindow::ExplorePath() {
  m_sequence->m_cursorPos = position_t(0, 0);
  m_mode = FILES;
  HelperLog(std::filesystem::current_path().string());
  m_sequence->LoadCurrentDirectory();
  return VIMKTOR_OK;
}

VimktorEvent_t Window::HandleInput() {
  VimktorEvent_t event = InputManager::Get().GetEvent(m_window, m_mode);
  event = HandleEvents(event);

  return event;
}

VimktorEvent_t ExploreWindow::HandleEvents(VimktorEvent_t ev) {
  static VimktorEvent_t event = ev;
  switch (event) {
  case EV_NONE:
    break;
  case EV_CURSOR_DOWN:
    m_sequence->CursorMove(DOWN);
    break;
  case EV_CURSOR_UP:
    m_sequence->CursorMove(UP);
    break;
  case EV_CURSOR_RIGHT:
    m_sequence->CursorMove(RIGHT);
    break;
  case EV_CURSOR_LEFT:
    m_sequence->CursorMove(LEFT);
    break;
  case EV_CLOSE:
    m_mode = EXIT;
    break;
  case EV_ERASE_LINE:
    m_sequence->EraseLineCursor();
    break;
  case EV_MODE_NORMAL:
    break;
  case EV_MODE_INSERT:
    break;
  case EV_MODE_INSERT_RIGHT:
    m_mode = INSERT;
    m_sequence->m_mode = m_mode;
    m_sequence->CursorMove(RIGHT);
    break;
  case EV_BACKSPACE:
    m_sequence->EraseCharCursor();
    break;
  case EV_INSERT_TEXT: {
    glyph_t gl = glyph_t(InputManager::Get().GetChar());
    m_sequence->InsertCharCursor(gl);
  } break;
  case EV_GO_TO_SOL:
    m_sequence->CursorMoveSol();
    break;
  case EV_GO_TO_NEXT_WORD:
    m_sequence->CursorMoveWordNext();
    break;
  case EV_GO_TO_EOL:
    m_sequence->CursorMoveEol();
    break;
  case EV_SAVE_FILE:
    WriteFile();
    HelperLog("saved to " + GetFileName());
    break;
  case EV_NEW_LINE:
    m_sequence->AddNewLineCursor();
    break;
  case EV_GET_COMMAND:
    event = HandleCommands();
    break;
  case EV_CHANGE_WINDOW_MENU:
    event = HandleWindowMenu();
    break;
  case EV_FILE_EXPLORER:
    WriteFile();
    ExplorePath();
    break;
  case EV_ENTER_CURSOR_DIRECTORY:
    OpenFileCursor();
    break;
  }
  return event;
}
VimktorErr_t ExploreWindow::WriteFile() {
  std::fstream file;
  file.open(m_filename, std::ios::out);
  if (!file.good()) {
    return FILE_ERROR;
  }
  m_sequence->WriteFile(file);
  file.close();

  return VIMKTOR_OK;
}
VimktorErr_t ExploreWindow::LoadFile(const std::string &fileName) {
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
  m_sequence->LoadFile(file);
  file.close();
  return VIMKTOR_OK;
}
VimktorErr_t ExploreWindow::WriteFile(const std::string &fileName) {
  return VIMKTOR_OK;
}
VimktorEvent_t Window::HandleWindowMenu() {
  char16_t ch;
  VimktorEvent_t event = EV_NONE;
  ch = wgetch(m_window);
  if (ch == 'l')
    return EV_CHANGE_WINDOW_RIGHT;
  if (ch == 'h')
    return EV_CHANGE_WINDOW_LEFT;
  return event;
}
VimktorEvent_t Window::HandleCommands() {
  std::string cmd;

  char16_t ch;
  VimktorEvent_t event = EV_NONE;
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
    Debug::Log("COMAND: " + cmd);
    event = HandleEvents(commandList.at(cmd));
  }
  return event;
}

VimktorErr_t ExploreWindow::ExplorePath(const std::string &path_str) {
  // TODO: Implement path string exploration
  m_sequence->m_cursorPos = position_t(0, 0);
  m_mode = FILES;
  HelperLog(std::filesystem::current_path().string());
  m_sequence->LoadCurrentDirectory();
  return VIMKTOR_OK;
}

VimktorErr_t ExploreWindow::OpenFileCursor() {
  auto path = std::filesystem::current_path();

  std::string path_str = path.string() + '/' + m_sequence->GetStringCursor();
  if (std::filesystem::is_directory(path_str) ||
      m_sequence->GetStringCursor() == "../" ||
      m_sequence->GetStringCursor() == "./") {
    ExplorePath(path_str);
  } else {
    HandleEvents(EV_OPEN_EDITOR);
  }
  return VIMKTOR_OK;
}

EditorWindow::EditorWindow(Window *parent, const size_t width,
                           const size_t height) {}
EditorWindow::EditorWindow(const size_t width, const size_t height) {}
EditorWindow::EditorWindow(const std::string &fileName) {}
EditorWindow::EditorWindow() {}
VimktorErr_t EditorWindow::Render() {}
VimktorEvent_t EditorWindow::HandleEvents(VimktorEvent_t event) {}
VimktorEvent_t EditorWindow::HandleCommands() {}
VimktorEvent_t EditorWindow::HandleWindowMenu() {}

VimktorErr_t EditorWindow::OpenFileCursor() {}
VimktorErr_t EditorWindow::LoadFile(const std::string &fileName) {}
VimktorErr_t EditorWindow::WriteFile(const std::string &fileName) {}
VimktorErr_t EditorWindow::WriteFile() {}

void EditorWindow::Init() { m_mode = NORMAL; }

CollabWindow::CollabWindow(Window *parent, const size_t width,
                           const size_t height) {}
CollabWindow::CollabWindow(const size_t width, const size_t height) {}
CollabWindow::CollabWindow(const std::string &fileName) {}
CollabWindow::CollabWindow() {}
VimktorErr_t CollabWindow::Render() {}
VimktorEvent_t CollabWindow::HandleEvents(VimktorEvent_t event) {}
VimktorEvent_t CollabWindow::HandleCommands() {}
VimktorEvent_t CollabWindow::HandleWindowMenu() {}

VimktorErr_t CollabWindow::OpenFileCursor() {}
VimktorErr_t CollabWindow::LoadFile(const std::string &fileName) {}
VimktorErr_t CollabWindow::WriteFile(const std::string &fileName) {}
VimktorErr_t CollabWindow::WriteFile() {}
VimktorErr_t CollabWindow::ExplorePath() {}
VimktorErr_t CollabWindow::ExplorePath(const std::string &path_str) {}
