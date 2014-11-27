
 ##############################################################################
 #                                asparagus                                   #
 #                                                                            #
 #    Copyright (C) 2014  Andreas Grapentin                                   #
 #                                                                            #
 #    This program is free software: you can redistribute it and/or modify    #
 #    it under the terms of the GNU General Public License as published by    #
 #    the Free Software Foundation, either version 3 of the License, or       #
 #    (at your option) any later version.                                     #
 #                                                                            #
 #    This program is distributed in the hope that it will be useful,         #
 #    but WITHOUT ANY WARRANTY; without even the implied warranty of          #
 #    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the           #
 #    GNU General Public License for more details.                            #
 #                                                                            #
 #    You should have received a copy of the GNU General Public License       #
 #    along with this program.  If not, see <http://www.gnu.org/licenses/>.   #
 ##############################################################################

 # This file contains the step pretty print methods of asparagus.
 ##############################################################################

# print a passed step to the output stream, cucumber style, and call `pass'
#   if in verbose mode, print the complete step message, else only '.'
proc pass_step { } {

  global asparagus_current_step_type
  global asparagus_current_step

  set str [ join "$asparagus_current_step" ]

  set msg "$asparagus_current_step_type $str"

  send_log "PASS: $msg\n"
  if { [ info exists ::env(VERBOSE) ] } {
    puts "\033\[00;32m$msg\033\[0m"
  } else {
    puts -nonewline "\033\[00;32m.\033\[0m"
    flush stdout
  }

  incr_count PASS

}

# print a failed step to the output stream, and call `fail'
proc fail_step { err } {

  global asparagus_current_step_type
  global asparagus_current_step
  global asparagus_skipping

  set asparagus_skipping 1

  set str [ join "$asparagus_current_step" ]

  set msg "$asparagus_current_step_type $str"

  if { [ string length $err ] } {
    set msg "$msg: $err"
  }

  send_log "FAIL: $msg\n"
  if { [ info exists ::env(VERBOSE) ] } {
    puts "\033\[00;31m$msg\033\[0m"
  } else {
    puts -nonewline "\033\[00;31mF\033\[0m"
    flush stdout
  }

  incr_count FAIL

  global exit_status
  set exit_status 1

}

# print an unrecognized step to the output stream and call `fail'
proc fail_unknown { } {

  global asparagus_current_step_type
  global asparagus_current_step
  global asparagus_skipping

  set asparagus_skipping 1

  set str [ join "$asparagus_current_step" ]

  set msg "$asparagus_current_step_type $str"

  send_log "MISS: $msg\n"
  if { [ info exists ::env(VERBOSE) ] } {
    puts "\033\[00;33m$msg\033\[0m"
  } else {
    puts -nonewline "\033\[00;33mU\033\[0m"
    flush stdout
  }

  incr_count FAIL

  global exit_status
  set exit_status 1

}

# print a skipped step to the output stream
proc skip_step { } {

  global asparagus_current_step_type
  global asparagus_current_step

  set str [ join "$asparagus_current_step" ]

  set msg "$asparagus_current_step_type $str"

  send_log "SKIP: $msg\n"
  if { [ info exists ::env(VERBOSE) ] } {
    puts "\033\[00;36m$msg\033\[0m"
  } else {
    puts -nonewline "\033\[00;36mS\033\[0m"
    flush stdout
  }

  incr_count UNTESTED

}
