#pragma once

#include <cstdint>
typedef struct _CollabSession_t {
  uint32_t port = 0;
  uint32_t ip = 0;
  uint32_t key = 0;
  bool is_active = false;
} CollabSession_t;
