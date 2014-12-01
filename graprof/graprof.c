
/******************************************************************************
 *             graprof - a profiling and trace analysis tool                  *
 *                                                                            *
 *    Copyright (C) 2013 - 2014  Andreas Grapentin                            *
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
#include "traceview/traceview.h"

#include <grapes/feedback.h>
#include <grapes/file.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

FILE *graprof_out = NULL;

int
main (int argc, char *argv[])
{
  // parse arguments
  struct arguments arguments = { NULL, NULL, 0, NULL, 0 };
  argp_parse (&argp, argc, argv, ARGP_IN_ORDER, 0, &arguments);

  // set default trace file in environment, if not specified
  setenv("GRAPROF_OUT", "graprof.out", 0);

  // fork & exec the child
  if (!(arguments.trace_filename))
    {
      pid_t pid = fork();
      feedback_assert(pid != -1, "fork failed", pid);

      if (!pid)
        {
          execv(arguments.binary_invocation[0], (char * const*)arguments.binary_invocation);
          feedback_error(EXIT_FAILURE, "failed to execute `%s'", arguments.binary_invocation[0]);
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
  int res = addr_init(arguments.binary_invocation[0]);
  if (res)
    {
      if (errno == ENOTSUP)
        {
          errno = 0;
          feedback_warning("%s: file format not supported or no debug symbols found", arguments.binary_invocation[0]);
        }
      else
        {
          feedback_error(EXIT_FAILURE, "%s", arguments.binary_invocation[0]);
        }
    }

  unsigned char md5_binary[DIGEST_LENGTH];
  res = md5_digest(md5_binary, arguments.binary_invocation[0]);
  feedback_assert_wrn(!res, "unable to digest '%s'", arguments.binary_invocation[0]);

  // figure out the proper tracefile
  const char *tracefile = getenv("GRAPROF_OUT");
  if (arguments.trace_filename)
    tracefile = arguments.trace_filename;

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
  if (arguments.out_filename)
    {
      graprof_out = fopen(arguments.out_filename, "w");
      feedback_assert(graprof_out, "%s", arguments.out_filename);
    }

  if (!arguments.tasks)
    arguments.tasks = GRAPROF_FLAT_PROFILE | GRAPROF_CALL_GRAPH | GRAPROF_MEMORY_PROFILE;

  // do stuff
  if (arguments.tasks & GRAPROF_FLAT_PROFILE)
    {
      res = flatprofile_print(arguments.verbose);
      assert_inner(!res, "flatprofile_print");
    }

  if (arguments.tasks & GRAPROF_CALL_GRAPH)
    callgraph_print(arguments.verbose);

  if (arguments.tasks & GRAPROF_MEMORY_PROFILE)
    memprofile_print();

  if (graprof_out != stdout)
    fclose(graprof_out);

  if (arguments.tasks & GRAPROF_TRACING_GUI)
    {
      res = traceview_main();
      feedback_assert(!res,
        (errno != ENOSYS
          ? "graprof traceview crashed"
          : "graprof traceview disabled at compile time"
        )
      );
    }

  return 0;
}
