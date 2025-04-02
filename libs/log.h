#ifndef LOGGER
#define LOGGER

#include <stdlib.h>
#include <stdio.h>

#define RED "\033[31m"
#define YELLOW "\033[33m"
#define RESET "\033[0m"


#define NEY_LOG_PREFIX "[log]: "
#define NEY_ERR_PREFIX RED "[error]: "
#define NEY_WARN_PREFIX YELLOW "[warn]: "

#define END_PRINTF printf(RESET);

#define ney_log(text, ...)                       \
  printf(NEY_LOG_PREFIX text ## __VA_ARGS__ "\n") \
  END_PRINTF

#define ney_err(text, ...)                        \
{                                                  \
  printf(NEY_ERR_PREFIX text ## __VA_ARGS__ "\n");  \
  END_PRINTF                                         \
  exit(EXIT_FAILURE);                                 \
}

#define ney_warn(text, ...)                      \
  printf(NEY_WARN_PREFIX text ## __VA_ARGS__"\n") \
  END_PRINTF

#endif // !LOGGER
