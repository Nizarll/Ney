#include "../libs/error.h"
void err(int status, const char *str) {
  printf(ANSI_COLOR_BRIGHT_RED "[error] : " ANSI_COLOR_RESET "%s\n", str);
  exit(status);
}
