#include "include/window.h"
#include "include/common.h"
#include "include/vimktor_debug.h"
#include <cstdint>

VimktorErr_t Window::RenderText(uint16_t x, uint16_t y, uint16_t width,
                                uint16_t height) {

  for (uint16_t i_y = y; i_y < height; i_y++) {
    for (uint16_t i_x = x; i_x < width; i_x++) {

      wmove(m_window, i_y, i_x);
      if (m_sequence->GetGlyphAtRel(i_x - x, i_y).has_value()) {
        const auto *temp = m_sequence->GetGlyphAtRel(i_x - x, i_y).value();
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
  wmove(m_window, cursor.y, cursor.x + LINE_NUM_WIDTH);

  return VIMKTOR_OK;
}

void Window::HelperLog(const std::string &msg) {
  position_t endPoint = GetWindowDimensions();
  size_t y = endPoint.y - 1;
  size_t x = 0;
  wmove(m_window, y, x);
  clrtoeol();
  mvwprintw(m_window, y, x, "%s", msg.c_str());
}
std::string Window::GetModeStr() const {
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

ExploreWindow::ExploreWindow(Window *parent, const size_t width,
                             const size_t height) {}
ExploreWindow::ExploreWindow(const size_t width, const size_t height) {}
ExploreWindow::ExploreWindow() {}
