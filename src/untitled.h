#ifndef UNTITLED_H
#define UNTITLED_H

#include <stdbool.h>

#include "log.h"
#define LOG_H_IMPLEMENTATION

typedef struct {
  int something;
} App;

void run_app(App *app);

/* Defined in the client's code */
App create_app(void);

typedef enum {
  EventTypeWindowClose,
  EventTypeWindowResize,
  EventTypeWindowFocus,
  EventTypeWindowLostFocus,
  EventTypeWindowMoved,

  EventTypeAppTick,
  EventTypeAppUpdate,
  EventTypeAppRender,

  EventTypeKeyPressed,
  EventTypeKeyReleased,

  EventTypeMouseButtonPressed,
  EventTypeMouseButtonReleased,
  EventTypeMouseMoved,
  EventTypeMouseScrolled,
} EventType;

typedef enum {
  EventCategoryApplication = 1 << 0, // 1
  EventCategoryInput       = 1 << 1, // 2
  EventCategoryKeyboard    = 1 << 2, // 4
  EventCategoryMouse       = 1 << 3, // 8
  EventCategoryMouseButton = 1 << 4, // 16
} EventCategory;

/* The first function is not need because it's not virtual? */
/* #define EVENT_STRUCT_TYPE(type) EventType (*get_static_type)(void)\ */
/* 	EventType (*get_event_type)(void)\ */
/* 	const char (*get_name)(void) */

/* Simplify these structs (potentially) */
/* typedef struct { */
/*   EventType (*get_event_type)(void); */
/*   const char (*get_name)(void); */
/*   int (*get_category_flags)(void); */
/*   const char (*to_string)(void); */

/*   bool handled; */
/* } Event; */

/* bool dispatch_event(Event *event) */
/* { */
/*   if (event->get_event_type() ==  */
/*   return false; */
/* } */

struct Event;

typedef size_t (*event_to_string_func)(const struct Event* event, char* buffer, size_t buffer_size);

typedef struct Event {
  EventType type;
  int category_flags;
  const char* name;

  event_to_string_func to_string;  /* may be NULL -> use default (name only) */
  bool handled;
} Event;

static inline EventType event_get_type(const Event* e) { return e->type; }
static inline const char* event_get_name(const Event* e) { return e->name; }
static inline int event_get_category_flags(const Event* e) { return e->category_flags; }
static inline bool event_is_in_category(const Event* e, EventCategory cat)
{
  /* 0001 & 0010 = 0000 */
  return (e->category_flags & cat) != 0;
}

// size_t is the std unsigned integer type that std functions take or return
// and is also the return type of the siezof operator
static inline size_t event_default_to_string(const Event* e, char* buf, size_t n)
{
  if (buf == NULL || n == 0) return 0; // To ensure buf is not NULL and 
  int w = snprintf(buf, n, "%s", e->name ? e->name : "Event"); // If e->name is not NULL, then we write it to the buffer
  // w = number of chars written to the buffer (excluding \0)
  // If negative, then an error occurred
  return (w < 0) ? 0u : (size_t) w;
}

/* Safe wrapper that always writes something */
static inline size_t event_to_string(const Event* e, char* buf, size_t n)
{
  if (e->to_string != NULL) return e->to_string(e, buf, n);
  // in case the to_string function is not defined
  return event_default_to_string(e, buf, n);
}

// MAY FAIL!!!
// Usage:
/*
bool on_key_pressed(KeyPressedEvent* e) {
  printf("Pressed %d\n", e->key_code);
  return true;
}

Event* e = (Event*)&some_key_pressed_event;
if (EVENT_DISPATCH(e, KeyPressedEvent, EVENT_TYPE_KEY_PRESSED, on_key_pressed)) {
  printf("Handler ran and set handled = %d\n", e->handled);
}
*/

#define EVENT_DISPATCH(base_ptr, DerivType, ExpectedType, handler_func)	\
  ((base_ptr) != NULL &&												\
   (base_ptr)->type == (ExpectedType) &&								\
   ((base_ptr)->handled = (handler_func)((DerivType*)(base_ptr))))

/* typedef struct { */
/*   Event base; */
/*   int key_code; */
/* } KeyEvent; */

/* static inline size_t key_event_to_string(const Event* e, char* buf, size_t n) */
/* { */
/*   const KeyPressedEvent* ev = (const KeyPressedEvent*)e; */
/*   int w = snprintf(buf, n, "%s: key=%d repeats=%d", */
/* 				   e->name, ev->keycode, ev->repeat_count); */
/*   return (w < 0) ? 0u : (size_t)w; */
/* } */

/* static inline void key_event_init(KeyEvent *ev, int key_code) */
/* { */
  // why?
/*   ev->base.type = EVENT_TYPE_KEY_PRESSED; */
/*   ev->base.category_flags = EVENT_CATEGORY_INPUT | EVENT_CATEGORY_KEYBOARD; */
/*   ev->base.name = "KeyEvent"; */
/*   ev->base.to_string = key_pressed_to_string; */
/*   ev->base.handled = false; */
/*   ev->keycode = keycode; */
/*   ev->repeat_count = repeat_count; */
/* } */

/* KeyPressedEvent */
typedef struct {
  Event base;
  int key_code;
  int repeat_count; /* like Hazel: repeat for held-down keys */
} KeyPressedEvent;

static inline size_t key_pressed_to_string(const Event* e, char* buf, size_t n)
{
  // wtf is this type casting?
  // well apparently you can do this if the first member is an Event - which it is...
  // this is because Event is at offset 0 and so is the KeyPressedEvent
  const KeyPressedEvent* ev = (const KeyPressedEvent*)e;
  int w = snprintf(buf, n, "%s: %d (%d repeats)", // Just like Cherno's code...
				   e->name, ev->key_code, ev->repeat_count);
  return (w < 0) ? 0u : (size_t)w;
}

static inline void key_pressed_event_init(KeyPressedEvent* ev, int key_code, int repeat_count)
{
  ev->base.type = EventTypeKeyPressed;
  ev->base.category_flags = EventCategoryInput | EventCategoryKeyboard;
  ev->base.name = "EventTypeKeyPressed"; // for the sake of consistency with Cherno's code
  ev->base.to_string = key_pressed_to_string;
  ev->base.handled = false;
  ev->key_code = key_code;
  ev->repeat_count = repeat_count;
}

typedef struct {
  Event base;
  int key_code;
} KeyReleasedEvent;

static inline size_t key_released_to_string(const Event* e, char* buf, size_t n)
{
  // wtf is this type casting?
  // well apparently you can do this if the first member is an Event - which it is...
  // this is because Event is at offset 0 and so is the KeyPressedEvent
  const KeyReleasedEvent* ev = (const KeyReleasedEvent*)e;
  int w = snprintf(buf, n, "%s: %d",
				   e->name, ev->key_code);
  return (w < 0) ? 0u : (size_t)w;
}

static inline void key_released_event_init(KeyPressedEvent* ev, int key_code, int repeat_count)
{
  ev->base.type = EventTypeKeyReleased;
  ev->base.category_flags = EventCategoryInput | EventCategoryKeyboard;
  ev->base.name = "EventTypeKeyReleased"; // for the sake of consistency with Cherno's code
  ev->base.to_string = key_pressed_to_string;
  ev->base.handled = false;
  ev->key_code = key_code;
  ev->repeat_count = repeat_count;
}

// Let's just wish that the inline functions defined for the event will work as expected...

typedef struct {
  Event base;
  float x, y;
} MouseMovedEvent;

static inline size_t mouse_moved_to_string(const Event* e, char* buf, size_t n)
{
  // wtf is this type casting?
  // well apparently you can do this if the first member is an Event - which it is...
  // this is because Event is at offset 0 and so is the KeyPressedEvent
  const MouseMovedEvent* ev = (const MouseMovedEvent*)e;
  int w = snprintf(buf, n, "%s: (%f, %f)",
				   e->name, ev->x, ev->y);
  return (w < 0) ? 0u : (size_t)w;
}

static inline void mouse_moved_event_init(MouseMovedEvent* ev, float x, float y)
{
  ev->base.type = EventTypeMouseMoved;
  ev->base.category_flags = EventCategoryInput | EventCategoryMouse;
  ev->base.name = "EventTypeMouseMoved"; // for the sake of consistency with Cherno's code
  ev->base.to_string = mouse_moved_to_string;
  ev->base.handled = false;
  ev->x = x;
  ev->y = y;
}

typedef struct {
  Event base;
  int button;
} MouseButtonPressedEvent;

static inline size_t mouse_button_pressed_to_string(const Event* e, char* buf, size_t n)
{
  // wtf is this type casting?
  // well apparently you can do this if the first member is an Event - which it is...
  // this is because Event is at offset 0 and so is the KeyPressedEvent
  const MouseButtonPressedEvent* ev = (const MouseButtonPressedEvent*)e;
  int w = snprintf(buf, n, "%s: %d",
				   e->name, ev->button);
  return (w < 0) ? 0u : (size_t)w;
}

static inline void mouse_button_pressed_event_init(MouseButtonPressedEvent* ev, int button)
{
  ev->base.type = EventTypeMouseButtonPressed;
  ev->base.category_flags = EventCategoryInput | EventCategoryMouse;
  ev->base.name = "EventTypeMouseButtonPressed"; // for the sake of consistency with Cherno's code
  ev->base.to_string = mouse_button_pressed_to_string;
  ev->base.handled = false;
  ev->button = button;
}

typedef struct {
  Event base;
  int button;
} MouseButtonReleasedEvent;

static inline size_t mouse_button_released_to_string(const Event* e, char* buf, size_t n)
{
  // wtf is this type casting?
  // well apparently you can do this if the first member is an Event - which it is...
  // this is because Event is at offset 0 and so is the KeyPressedEvent
  const MouseButtonReleasedEvent* ev = (const MouseButtonReleasedEvent*)e;
  int w = snprintf(buf, n, "%s: %d",
				   e->name, ev->button);
  return (w < 0) ? 0u : (size_t)w;
}

static inline void mouse_button_released_event_init(MouseButtonReleasedEvent* ev, int button)
{
  ev->base.type = EventTypeMouseButtonReleased;
  ev->base.category_flags = EventCategoryInput | EventCategoryMouse;
  ev->base.name = "EventTypeMouseButtonReleased"; // for the sake of consistency with Cherno's code
  ev->base.to_string = mouse_button_released_to_string;
  ev->base.handled = false;
  ev->button = button;
}

typedef struct {
  Event base;
  float x_offset, y_offset;
} MouseScrolledEvent;

static inline size_t mouse_scrolled_to_string(const Event* e, char* buf, size_t n)
{
  // wtf is this type casting?
  // well apparently you can do this if the first member is an Event - which it is...
  // this is because Event is at offset 0 and so is the KeyPressedEvent
  const MouseScrolledEvent* ev = (const MouseScrolledEvent*)e;
  int w = snprintf(buf, n, "%s: (%f, %f)",
				   e->name, ev->x_offset, ev->y_offset);
  return (w < 0) ? 0u : (size_t)w;
}

static inline void mouse_scrolled_event_init(MouseScrolledEvent* ev, float x_offset, float y_offset)
{
  ev->base.type = EventTypeMouseScrolled;
  ev->base.category_flags = EventCategoryInput | EventCategoryMouse;
  ev->base.name = "EventTypeMouseScrolled"; // for the sake of consistency with Cherno's code
  ev->base.to_string = mouse_scrolled_to_string;
  ev->base.handled = false;
  ev->x_offset = x_offset;
  ev->y_offset = y_offset;
}

#endif /* UNTITLED_H */

#ifndef UNTITLED_H_IMPLEMENTATION
#define UNTITLED_H_IMPLEMENTATION

void run_app(App *app)
{
  while (true);
}

int main(int argc, const char **argv)
{
  log_set_level(LOG_TRACE);
  log_trace("Initialised logging!");
  
  App app = {0};
  run_app(&app);
  return 0;
}

#endif /* UNTITLED_H_IMPLEMENTATION */
