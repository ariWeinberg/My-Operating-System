#ifndef STDOUT
#define STDOUT

typedef enum
{
    UP,
    DOWN,
} ScrollDirection;

void stdout_loop(void);

void stdout_init(void);
void stdout_flush(void);
void stdout_write(char c);
void stdout_write_line(const char *s);
void stdout_scroll_line(ScrollDirection d);
void stdout_scroll_page(ScrollDirection d);

#endif
