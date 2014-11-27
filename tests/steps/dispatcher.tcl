
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

 # This script handles the step dispatching and registration.
 ##############################################################################

# process a `given' step
proc Given { args } { given {*}"$args" }
proc given { args } {

  global asparagus_step_type
  global asparagus_current_step_type
  global asparagus_current_step
  global asparagus_skipping

  set asparagus_skipping 0

  set asparagus_step_type "Given"
  set asparagus_current_step_type "Given"
  set asparagus_current_step $args

  dispatch_step {*}"given $args"

}

# process a `when' step
proc When { args } { when {*}"$args" }
proc when { args } {

  global asparagus_step_type
  global asparagus_current_step_type
  global asparagus_current_step

  set asparagus_step_type "When"
  set asparagus_current_step_type "  When"
  set asparagus_current_step $args

  dispatch_step {*}"when $args"

}

# process a `then' step
proc Then { args } { then {*}"$args" }
proc then { args } {

  global asparagus_step_type
  global asparagus_current_step_type
  global asparagus_current_step

  set asparagus_step_type "Then"
  set asparagus_current_step_type "  Then"
  set asparagus_current_step $args

  dispatch_step {*}"then $args"

}

#process an `and' step
proc And { args } { and {*}"$args" }
proc and { args } {

  global asparagus_step_type
  global asparagus_current_step_type
  global asparagus_current_step

  set asparagus_current_step_type "  And"
  set asparagus_current_step $args

  dispatch_step {*}"$asparagus_step_type $args"

}

# try and execute the proc associated with the current step
proc dispatch_step { args } {

  global asparagus_step_definitions
  global asparagus_skipping

  if { $asparagus_skipping } {
    skip_step
    return
  }

  foreach { func step } $asparagus_step_definitions {
    if { [ string_starts_with "$args" "$step" ] } {
      $func {*}[ string_pop "$args" "$step" ]
      return
    }
  }

  fail_unknown

}

# register a new step for the dispatcher
proc asparagus_register_step { func step } {

  global asparagus_step_definitions

  lappend asparagus_step_definitions $func $step

}
