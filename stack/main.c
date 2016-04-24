#include <stdlib.h>
#include <string.h>
#include <stdio.h>

void check_gets(char in_buf[]) {
  char canary[] = CANARY;
  char buf[USER_BUF_LEN];
  char buf_pad[PAD_BUF_LEN];

  gets(buf);
  if (strncmp(canary, CANARY, strlen(CANARY))) {
    exit(-1);
  }

  memcpy(in_buf, buf, USER_BUF_LEN);
}
  
void main() {
  char my_buf[USER_BUF_LEN];

  check_gets(my_buf);

  puts(my_buf);

  return;
}
