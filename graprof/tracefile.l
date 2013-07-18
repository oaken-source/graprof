
%top{
  #define YY_DECL int yylex(const char *filename)

  #include "trace.h"

  #include <grapes/feedback.h>
  #include <grapes/util.h>
}

ENTER           e
EXIT            x
MALLOC          \+
REALLOC         \*
FREE            \-
END             END

HEX             0x[0-9a-fA-F]+
DEC             [0-9]+

%option noinput nounput noyywrap noyy_scan_string noyy_scan_bytes 

%%

{ENTER}\ {HEX}\ {HEX}\ {DEC}                            { int res = trace_enter(yytext);
                                                          assert_inner(!res, "trace_enter"); }
{EXIT}\ {HEX}\ {HEX}\ {DEC}                             { int res = trace_exit(yytext);
                                                          assert_inner(!res, "trace_exit"); }
{MALLOC}\ {DEC}\ {HEX}\ {HEX}\ {DEC}                    { int res = trace_malloc(yytext);
                                                          assert_inner(!res, "trace_malloc"); }
{REALLOC}\ {HEX}\ {DEC}\ {HEX}\ {HEX}\ {DEC}            { int res = trace_realloc(yytext);
                                                          assert_inner(!res, "trace_realloc"); }
{FREE}\ {HEX}\ {HEX}\ {DEC}                             { int res = trace_free(yytext);
                                                          assert_inner(!res, "trace_free"); }
{END}\ {DEC}                                            { int res = trace_end(yytext);
                                                          assert_inner(!res, "trace_end");
							  return 0; }

[ \t]+

\n                                                      ++yylineno;

.* {                                                     
  feedback_warning("%s:%u: invalid trace data near `%s'", filename, yylineno, yytext);
}

%%


