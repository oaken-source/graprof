
#pragma once

extern void *libgraprof_buf;
extern unsigned long libgraprof_bufsize;

#define buffer_enlarge(S) \
  unsigned long index = libgraprof_bufsize; \
  libgraprof_bufsize += (S); \
  libgraprof_buf = realloc(libgraprof_buf, libgraprof_bufsize)

#define buffer_append(T, V) \
  *((T*)(libgraprof_buf + index)) = (V); \
  index += sizeof(T)

