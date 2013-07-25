
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <grapes/feedback.h>

void *foo = NULL;

int
single ()
{
  //printf("single\n");

  //free(foo);

  return 0;
}

int
recursive (unsigned int x)
{
  --x;
  //printf("recursive\n");

  return (x ? recursive(x) : 0);
}

__attribute__ ((no_instrument_function)) int
timing_inner (void)
{
  return;
}

int 
timing (unsigned int x)
{
  volatile unsigned int i;
  for (i = 0; i < x; ++i)
    timing_inner();
  return 0;
}

#define SIN_MAX 1000000
#define REC_MAX 1000

int
dostuff ()
{
  unsigned int i;
  for (i = 0; i < SIN_MAX; ++i)
    single();
  recursive (REC_MAX);

  timing(SIN_MAX);

  return 0;
}

int
main ()
{
  feedback_readline("say");

  foo = calloc(1000, 1);
  dostuff();
  free(foo);

  char *foo = strdup("hello world");

  return 0;
}