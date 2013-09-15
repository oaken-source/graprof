
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

FILE *graprof_out;

int 
main (int argc, char *argv[])
{
  struct arguments args = { 0, 0, 0, 0 };
  argp_parse (&argp, argc, argv, 0, 0, &args);

  if (args.binary_filename)
    {
      int res = addr_init(args.binary_filename);
      if (res)
        {
          if (errno == ENOTSUP)
            {
              errno = 0;
              feedback_warning("%s: file format not supported or no debug symbols found", args.binary_filename);
            }
          else
            {
              feedback_warning("%s", args.binary_filename);
            }
        }
    }
  
  int res = trace_read(args.trace_filename);
  if (res)
    {
      if (errno == ENOTSUP)
        {
          errno = 0;
          feedback_error(EXIT_FAILURE, "%s: invalid trace data", args.trace_filename);
        }
      else
        {
          feedback_error(EXIT_FAILURE, "%s", args.trace_filename);
        }
    }

  graprof_out = stdout;
  if (args.out_filename && (args.tasks & ~GRAPROF_NO_GUI))
    {
      graprof_out = fopen(args.out_filename, "w");
      feedback_assert(graprof_out, "%s", args.out_filename);
    }

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
