
#define loop(F, N) \
    do { \
      unsigned int i; \
      for (i = 0; i < (N); ++i) F; \
    } while (0)

void
func3 (void) { }

void
func2 (void)
{
  loop(, 100000);
}

void
func1 (void)
{
  loop(func3(), 100000);
}

int
main (void)
{
  loop(func3(), 1000);
  return 0;
}
