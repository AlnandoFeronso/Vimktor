#include <stdint.h>
typedef struct _CollabSession_t {
  uint32_t port;
  uint32_t ip;
  uint32_t key;
  bool is_active;
} CollabSession_t;
typedef enum {
  OP_INSERT,
  OP_DELETE,
  OP_REPLACE,   
  OP_NEWLINE,  
  OP_ERASE_LINE 
} OpType;
typedef struct {
  OpType type;
  int32_t x;   // Pozycja X
  int32_t y;   // Pozycja Y
  uint32_t ch; // Znak (pasuje do struktury glyph_t z Twojego kodu)
} CollabMessage;
