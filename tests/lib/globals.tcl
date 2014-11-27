
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

 # This file lists the global state information of asparagus, mainly for
 # reference.
 ##############################################################################

## the path to the current executable
# used by "Given an executable"
global asparagus_executable_path

## the type of the last executed step
# can be "Given" "When" or "Then"
global asparagus_last_step_type

## the actual type of the currently executing step
# can be "Given" "  When" "  Then" or "  And"
global asparagus_current_step_type

## the string form of the current step
# used by the *_send procs
global asparagus_current_step

## the known step definitions and their string representations
# use register_step to add your own
global asparagus_step_definitions

## if we are currently in skipping mode (there were errors)
global asparagus_skipping
