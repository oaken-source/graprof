
#pragma once

int trace_enter(const char*);

int trace_exit(const char*);

int trace_malloc(const char*);

int trace_realloc(const char*);

int trace_free(const char*);

int trace_end(const char*);
