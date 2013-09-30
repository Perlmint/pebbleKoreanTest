#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"

#define MY_UUID { 0x98, 0x24, 0x92, 0xE8, 0x50, 0xDB, 0x4A, 0xE3, 0xA6, 0x9A, 0x1B, 0x43, 0xE0, 0x12, 0xC9, 0x3F }
PBL_APP_INFO(MY_UUID,
             "Template App", "Your Company",
             1, 0, /* App version */
             DEFAULT_MENU_ICON,
             APP_INFO_STANDARD_APP);

Window window;
Layer textLayer;
HeapBitmap font;
GBitmap fontCursor;
GPoint *fontCursorOrigin;

typedef enum {
  first, middle_u, middle_r, last, none
} glyphType_t;

void koreanTextLayerUpdate(struct Layer *layer, GContext *ctx) {
  char sampleText[] = "uvamribwkvf;rxubnfsjdifygkvufRuvyvxmfhghdzjd;vjbmfnfjbydhfyfifsncibkbamvfnfzotsydmvfyekfanfsurmfsnfyfzurmfsjgyvkdwjd;vltsmfnc";
  int16_t xPos = 0, yPos = 0, code;
  glyphType_t prev = none, current = none;
  graphics_context_set_compositing_mode(ctx, GCompOpAnd);
  for(uint16_t strPos = 0, max = strlen(sampleText); strPos < max; ++strPos) {
    switch(sampleText[strPos]) {
    case 'y': case 'u': case 'i': case 'o':
    case 'p': case '0': case 'h': case 'j':
    case 'k': case 'l': case ';': case '\'':
    case 'n': case 'm':
      current = first; break;
    case '4': case '5': case '9': case 'g':
    case 'v': case 'b': case '/':
      current = middle_u; break;
    case '6': case '7': case '8': case 'e':
    case 'r': case 't': case 'd': case 'f':
    case 'c':
      current = middle_r; break;
    default: current = last; break;
    }
    if (current == first) {
      if (prev == first) {
        xPos += 18;
      } else if (prev == middle_u) {
        xPos += 16;
      } else if (prev == middle_r) {
        xPos += 14;
      } else if (prev == last) {
        xPos += 12;
      }
    } else if ((prev == first && current == middle_u) ||
               (prev == middle_u && current == middle_r)) {
      xPos += 2;
    } else if ((prev == middle_u && current == last) ||
               (prev == first && current == middle_r)) {
      xPos += 4;
    } else if ((prev == first && current == last) ||
               (prev == middle_r && current == last)) {
      xPos += 6;
    }
    if (xPos >= 126) {
      xPos = 0; yPos += 18;
    }
    code = sampleText[strPos] - '!';
    fontCursorOrigin->x = (code & 0x07) * 18;
    fontCursorOrigin->y = (code >> 3) * 18;
    graphics_draw_bitmap_in_rect(ctx, &fontCursor, GRect(xPos, yPos, 18, 18));
    if (prev == middle_r && current == last && xPos != 0) {
      xPos -= 4;
    }
    prev = current;
  }  
}

void handle_init(AppContextRef ctx) {
  (void)ctx;
  window_init(&window, "Window Name");
  window_stack_push(&window, true /* Animated */);
  resource_init_current_app(&APP_RESOURCES);
  
  // init font
  heap_bitmap_init(&font, RESOURCE_ID_KOREAN_FONT_18);
  gbitmap_init_as_sub_bitmap(&fontCursor, &font.bmp, GRect(0, 0, 18, 18));
  layer_init(&textLayer, window.layer.frame);
  fontCursorOrigin = &fontCursor.bounds.origin;
  layer_set_update_proc(&textLayer, &koreanTextLayerUpdate);
  layer_add_child(&window.layer, &textLayer);
}

void pbl_main(void *params) {
  PebbleAppHandlers handlers = {
    .init_handler = &handle_init
  };
  app_event_loop(params, &handlers);
}