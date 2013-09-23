
/******************************************************************************
 *             graprof - a profiling and trace analysis tool                  *
 *                                                                            *
 *    Copyright (C) 2013  Andreas Grapentin                                   *
 *                                                                            *
 *    This program is free software: you can redistribute it and/or modify    *
 *    it under the terms of the GNU General Public License as published by    *
 *    the Free Software Foundation, either version 3 of the License, or       *
 *    (at your option) any later version.                                     *
 *                                                                            *
 *    This program is distributed in the hope that it will be useful,         *
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of          *
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the           *
 *    GNU General Public License for more details.                            *
 *                                                                            *
 *    You should have received a copy of the GNU General Public License       *
 *    along with this program.  If not, see <http://www.gnu.org/licenses/>.   *
 ******************************************************************************/


#include "graprof.h"

#include "addr.h"
#include "trace.h"
#include "flatprofile.h"
#include "callgraph.h"
#include "memprofile.h"

#include <grapes/feedback.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

FILE *graprof_out = NULL;
int graprof_verbosity = 0;

int
main (int argc, char *argv[])
{
  struct arguments args = { NULL, NULL, 0, NULL };
  argp_parse (&argp, argc, argv, ARGP_IN_ORDER, 0, &args);

  // set default trace file in environment, if not specified
  setenv("GRAPROF_OUT", "graprof.out", 0);

  // fork & exec the child
  if (!(args.trace_filename))
    {
      pid_t pid = fork();
      feedback_assert(pid != -1, "fork failed", pid);

      if (!pid)
        {
          execv(args.binary_invocation[0], (char * const*)args.binary_invocation);
          feedback_error(EXIT_FAILURE, "failed to execute `%s'", args.binary_invocation[0]);
          exit(1);
        }
      else
        {
          int status = 0;
          waitpid(pid, &status, 0);
          feedback_assert_wrn(WIFEXITED(status), "child exited abnormally. Tracefile may be invalid!");
        }
    }

  // read debug symbols from child binary
  int res = addr_init(args.binary_invocation[0]);
  if (res)
    {
      if (errno == ENOTSUP)
        {
          errno = 0;
          feedback_warning("%s: file format not supported or no debug symbols found", args.binary_invocation[0]);
        }
      else
        {
          feedback_error(EXIT_FAILURE, "%s", args.binary_invocation[0]);
        }
    }

  // get md5 hash from child binary
  unsigned char md5_binary[16] = { 0 };

  FILE *binary = fopen(args.binary_invocation[0], "r");
  if (!binary)
    {
      feedback_warning("%s: unable to open for digest verification", args.binary_invocation[0]);
    }
  else
    {
      // TODO: get md5 from file
    }

  free(args.binary_invocation);

  // figure out the proper tracefile
  const char *tracefile = getenv("GRAPROF_OUT");
  if (args.trace_filename)
    tracefile = args.trace_filename;

  // read the tracefile
  res = trace_read(tracefile, md5_binary);
  if (res)
    {
      if (errno == ENOTSUP)
        {
          errno = 0;
          feedback_error(EXIT_FAILURE, "%s: invalid trace data", tracefile);
        }
      else
        {
          feedback_error(EXIT_FAILURE, "%s", tracefile);
        }
    }

  // figure out the proper output file
  graprof_out = stdout;
  if (args.out_filename)
    {
      graprof_out = fopen(args.out_filename, "w");
      feedback_assert(graprof_out, "%s", args.out_filename);
    }

  // do stuff
  if (args.tasks & GRAPROF_FLAT_PROFILE)
    flatprofile_print(args.tasks & GRAPROF_CALL_GRAPH);

  if (args.tasks & GRAPROF_CALL_GRAPH)
    callgraph_print();

  if (args.tasks & GRAPROF_MEMORY_PROFILE)
    memprofile_print();

  if (graprof_out != stdout)
    fclose(graprof_out);

  if (!(args.tasks & GRAPROF_NO_GUI))
    {
      errno = ENOSYS;
      feedback_assert_wrn(0, "tracing gui");
    }

  return 0;
}
