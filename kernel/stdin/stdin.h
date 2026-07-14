#ifndef STDIN
#define STDIN

#define STDIN_SIZE (1024u * 1024u)

void stdin_insert(char c);
char read_char(void);
char* read_line(void);
void stdin_loop(void);
char peek_char(void);
void stdin_init();

#endif
