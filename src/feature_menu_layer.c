#include "pebble.h"

#define KEY_SET_SPACE 0
#define KEY_SET_LINES 1
#define KEY_LINEINDEX 2
#define KEY_LINETEXT 3
#define KEY_ALL_DONE 4

static Window *window;

static MenuLayer *menu_layer;

/* todo: use size_t heap_bytes_free(void) to see how much we can allocate */

static uint16_t
menu_get_num_sections_callback(MenuLayer *menu_layer,
			       void *data) 
{
  return 1;
}

/* A rudimentary collection of "lines" of text.  Each one is
 * null-terminated, and they shouldn't have a newline or carriage
 * return.  They're stored in a common buffer, pointed to by `bytes'
 * and with `byte_fill' pointing where the next line should be
 * added. `last_char' is the limit of allocation.
 *
 * The line starts are pointed to by elements of the array `lines',
 * for which `allocated_lines' gives the maximum size, and
 * `available_lines' gives the number that have actually been filled
 * in.  Lines can be added using the function `add_line'.
 *
 * Both these structures are created by `allocate_storage'.
 */

static char **lines = NULL;
static int allocated_lines = 0;
static int available_lines = 0;
static char *bytes = NULL;
static char *byte_fill = NULL;
static char *last_char = NULL;

static int got_all_lines = 0;


static void
add_line(int line_index, char *line)
{
  int len = strlen(line);
  if ((lines == NULL) || (bytes == NULL)) {
    return;
  }
  if ((byte_fill + len) >= last_char) {
    return;
  }
  strcpy(byte_fill, line);
  lines[line_index] = byte_fill;
  if (line_index >= available_lines) {
    available_lines = line_index + 1;
  }
  byte_fill += (len + 1);
}

static void
allocate_storage(int alloc_lines, int alloc_bytes)
{
  if (lines != NULL) {
    free(lines);
  }
  lines = (char**)malloc(alloc_lines * sizeof(char*));
  allocated_lines = alloc_lines;
  available_lines = 0;
  if (bytes != NULL) {
    free(bytes);
  }
  byte_fill = bytes = (char*)malloc(alloc_bytes + 1);
  last_char = bytes + alloc_bytes;
    if ((lines == NULL) || (bytes == NULL)) {
//         todo: some kind of error handling here
    }
}

static char*
entry_line(int index) 
{
  if (allocated_lines == 0) {
    return "Nothing loaded";
  }
  if (index >= allocated_lines) {
    return "Fallen off end of array!";
  }
  if (lines[index] == NULL) {
    return "Empty entry!";
  }
  return lines[index];
}

static int
count_entries()
{
  if (allocated_lines == 0) {
    return 1;			/* one fake entry to say nothing is loaded */
  }
  return available_lines;
}

static void
prod_phone(uint8_t key, uint8_t cmd)
{
  DictionaryIterator *iter;
  app_message_outbox_begin(&iter);
 
  Tuplet value = TupletInteger(key, cmd);
  dict_write_tuplet(iter, &value);
 
  app_message_outbox_send();
}

static void
inbox_received_callback(DictionaryIterator *iterator, void *context)
{
  // Read first item
  Tuple *t = dict_read_first(iterator);
  int line_index = -1;
  int n_chars = -1;
  int n_lines = -1;
  char *line_text = NULL;

  while (t != NULL) {
    // Which key was received?
    switch (t->key) {
    case KEY_SET_SPACE:
      n_chars = t->value->int32;
      break;

    case KEY_SET_LINES:
      n_lines = t->value->int32;
      break;

    case KEY_LINEINDEX:
      line_index = t->value->int32;
      break;

    case KEY_LINETEXT:
      line_text = t->value->cstring;
      break;

    case KEY_ALL_DONE:
      got_all_lines = 1;
      break;

    default:
      APP_LOG(APP_LOG_LEVEL_ERROR, "Key %d not recognized!", (int)t->key);
      break;
    }

    // Look for next item
    t = dict_read_next(iterator);
  }

  APP_LOG(APP_LOG_LEVEL_INFO, "End of message %d %d %d %p", n_chars, n_lines, line_index, line_text);
#if 0
  /* Having got all the keys, act on the message */
  if ((n_chars > 0) && (n_lines > 0)) {
    allocate_storage(n_lines, n_chars);
  }
  if ((line_index > 0) && (line_text != NULL)) {
    add_line(line_index, line_text);
  }
#endif
}

static void inbox_dropped_callback(AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped!");
}

static void
outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context)
{
  APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox send failed!");
}

static void
outbox_sent_callback(DictionaryIterator *iterator, void *context)
{
  APP_LOG(APP_LOG_LEVEL_INFO, "Outbox send success!");
}

static uint16_t
menu_get_num_rows_callback(MenuLayer *menu_layer,
			   uint16_t section_index,
			   void *data)
{
  return count_entries();
}

// A callback is used to specify the height of the section header
static int16_t
menu_get_header_height_callback(MenuLayer *menu_layer,
				uint16_t section_index,
				void *data)
{
  // This is a define provided in pebble.h that you may use for the default height
  /* todo: make this lower; perhaps calculate from the font metrics */
  return MENU_CELL_BASIC_HEADER_HEIGHT;
}

// Here we draw what each header is
static void
menu_draw_header_callback(GContext* ctx,
			  const Layer *cell_layer,
			  uint16_t section_index,
			  void *data)
{
  char buf[32];
  snprintf(buf, 32, "%d items; %d bytes", available_lines, heap_bytes_free());
  menu_cell_basic_header_draw(ctx, cell_layer, buf);
}

// This is the menu item draw callback where you specify what each item should look like
static void
menu_draw_row_callback(GContext* ctx,
		       const Layer *cell_layer,
		       MenuIndex *cell_index,
		       void *data)
{
  menu_cell_title_draw(ctx, cell_layer, entry_line(cell_index->row));
}

// Here we capture when a user selects a menu item
static void
menu_select_callback(MenuLayer *menu_layer,
		     MenuIndex *cell_index,
		     void *data)
{
  // Use the row to specify which item will receive the select action
#if 0
  /* todo: try it out the following, to set the title to the selected entry */
  /* fixme: ctx isn't available here, neither is cell_layer */
  menu_cell_basic_header_draw(ctx, cell_layer, entry_line(cell_index->row));
#endif
#if 0
  switch (cell_index->row) {
    // This is the menu item with the cycling icon
  case 1:
    // Cycle the icon
    current_icon = (current_icon + 1) % NUM_MENU_ICONS;
    // After changing the icon, mark the layer to have it updated
    layer_mark_dirty(menu_layer_get_layer(menu_layer));
    break;
  }
#endif
}

// This initializes the menu upon window load
static void 
window_load(Window *window)
{
  // Now we prepare to initialize the menu layer
  // We need the bounds to specify the menu layer's viewport size
  // In this case, it'll be the same as the window's
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_frame(window_layer);

  // Create the menu layer
  menu_layer = menu_layer_create(bounds);

  // Set all the callbacks for the menu layer
  menu_layer_set_callbacks(menu_layer, NULL, (MenuLayerCallbacks){
      .get_num_sections = menu_get_num_sections_callback,
	.get_num_rows = menu_get_num_rows_callback,
	.get_header_height = menu_get_header_height_callback,
	.draw_header = menu_draw_header_callback,
	.draw_row = menu_draw_row_callback,
	.select_click = menu_select_callback,
	});

  // Bind the menu layer's click config provider to the window for interactivity
  menu_layer_set_click_config_onto_window(menu_layer, window);

  // Add it to the window for display
  layer_add_child(window_layer, menu_layer_get_layer(menu_layer));

  /* todo: fetch the menu data from the phone. */
}

static void
window_unload(Window *window)
{
  // Destroy the menu layer
  menu_layer_destroy(menu_layer);
}

int
main(void)
{
  window = window_create();

  window_set_window_handlers(window, (WindowHandlers) {
      .load = window_load,
	.unload = window_unload,
	});

  allocate_storage(16, 4096);
  add_line(0, "preload one");
  add_line(1, "preload two");
  add_line(2, "preload three");
  add_line(3, "preload four");
  add_line(4, "preload five");
  add_line(5, "preload six");

  window_stack_push(window, true /* Animated */);

#if 1
  app_message_register_inbox_received(inbox_received_callback);
  app_message_register_inbox_dropped(inbox_dropped_callback);
  app_message_register_outbox_failed(outbox_failed_callback);
  app_message_register_outbox_sent(outbox_sent_callback);

  app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
#endif

#if 1
  add_line(6, "preload seven");
  add_line(7, "preload eight");
  add_line(8, "preload nine");

  menu_layer_reload_data(menu_layer);
#endif

#if 0
  while (!got_all_lines) {
    prod_phone(0, 0);
  }
#endif

  app_event_loop();

  window_destroy(window);
}
