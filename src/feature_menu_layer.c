#include "pebble.h"

static Window *window;

// This is a menu layer
// You have more control than with a simple menu layer
static MenuLayer *menu_layer;

// A callback is used to specify the amount of sections of menu items
// With this, you can dynamically add and remove sections
static uint16_t
menu_get_num_sections_callback(MenuLayer *menu_layer,
			       void *data) 
{
  return 1;
}
static char **lines = NULL;
static int allocated_lines = 0;
static int available_lines = 0;
static char *bytes = NULL;
static char *byte_fill = NULL;
static char *last_char = NULL;

static void
add_line(char *line)
{
  int len = strlen(line);
  if ((byte_fill + len) >= last_char) {
    return;
  }
  strcpy(byte_fill, line);
  lines[available_lines++] = byte_fill;
  byte_fill += (len + 1);
}

static void
allocate_storage(int alloc_lines, int alloc_bytes)
{
  lines = (char**)malloc(alloc_lines * sizeof(char*));
  allocated_lines = alloc_lines;
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
  return lines[index];
}

static int
count_entries()
{
  int i;
  if (allocated_lines == 0) {
    return 1;			/* one fake entry to say nothing is loaded */
  }
  return available_lines;
}

// Each section has a number of items;  we use a callback to specify this
// You can also dynamically add and remove items using this
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
  /* todo: make this lower */
  return MENU_CELL_BASIC_HEADER_HEIGHT;
}

// Here we draw what each header is
static void
menu_draw_header_callback(GContext* ctx,
			  const Layer *cell_layer,
			  uint16_t section_index,
			  void *data)
{
  menu_cell_basic_header_draw(ctx, cell_layer, "Some example items");
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

  window_stack_push(window, true /* Animated */);

  allocate_storage(3, 16);
  add_line("foo");
  add_line("bar");
  add_line("baz");

  app_event_loop();

  window_destroy(window);
}
