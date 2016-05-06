#include <libcgc.h>
#include "libc.h"

/* #define GETS_BUF_LEN 16 */
/* #define PRINT_BUF_LEN 16 */
/* #define CANARY "vito" */
/* #define ECHO_BUF_LEN 24 */

void *memcpy(void *dest, const void *src, size_t n);
int gets(char* dest);
void print(char* src);
int streq(char* left, char* right, int count);
void echo();
int main();

###

void *memcpy(void *dest, const void *src, size_t n)
{
    char *dp = dest;
    const char *sp = src;
    while (n--)
        *dp++ = *sp++;
    return dest;
}

###

int gets(char* dest) {
  char buf[GETS_BUF_LEN];

  int dest_cursor = 0;

  while (1) {
    size_t got;
    if (receive(STDIN, buf, GETS_BUF_LEN, &got) != 0) _terminate(-1);

    for (int buf_cursor = 0; buf_cursor < got; buf_cursor++) {
      if (buf[buf_cursor] == '\n') {
        dest[dest_cursor + buf_cursor] = 0;
        return dest_cursor + buf_cursor;
      }

#ifdef PATCHED_1
      if (dest_cursor + buf_cursor > ECHO_BUF_LEN) _terminate(-1);
#endif

      dest[dest_cursor + buf_cursor] = buf[buf_cursor];
    }

    dest_cursor += got;
  }
}

###

void print(char* src) {
  int src_len = 0;

  while (1) {
    if (src[src_len] == 0) break;
    src_len++;
  }

  int total_sent = 0;

  while(1) {
    int remaining = src_len - total_sent;
    int to_send = (PRINT_BUF_LEN > remaining) ? PRINT_BUF_LEN : remaining;

    size_t did_send;

    if (transmit(STDOUT, src + total_sent, to_send, &did_send) != 0) {
      _terminate(-1);
    }

    total_sent += did_send;

    if (total_sent >= src_len) return;
  }
}

###

int streq(char* left, char* right, int count) {
  for (int cursor = 0; cursor < count; cursor++) {
    char l = left[cursor];
    char r = right[cursor];

    if (l != r) return 0;
  }

  return 1;
}

###

void echo() {
  char canary[] = CANARY;
  char buf[ECHO_BUF_LEN];

  gets(buf);
  print(buf);

  if (!streq(canary, CANARY, 4)) {
    print("hacking detected, see ya");
    _terminate(-1);
  }
}

###

int main() {
  echo();
  return 0;
}

###
