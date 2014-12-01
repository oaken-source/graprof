
proc when_I_time { } {

  global asparagus_executable_path
  global graprof_time_taken

  set iterations 10

  set time_total 0

  for { set i 0 } { $i < $iterations } { incr i } {

    set time_start [clock clicks -milliseconds]

    if { [ catch {
      spawn $asparagus_executable_path
      wait $spawn_id
    } msg ] } {
      faíl_step "$msg"
      return
    }

    set time_taken [expr [clock clicks -milliseconds] - $time_start]
    set time_total [expr $time_total + $time_taken]

  }

  set time_total [expr $time_total / $iterations]

  send_log "TIME: $time_total ms\n"

  set graprof_time_taken "${time_taken}ms"

  pass_step

}
asparagus_register_step when_I_time "when I time"

proc time_expand { duration } {

  set t 0.0

  set i 0
  while { [string is digit [string index "$duration" $i]] } {
    set t [expr $t * 10 + [string index "$duration" $i]]
    incr i
  }

  if { [string equal "n" [string index "$duration" $i]] } {
    set t [expr $t / 1000000.0]
  }
  if { [string equal "u" [string index "$duration" $i]] } {
    set t [expr $t / 1000.0]
  }
  if { [string equal "m" [string index "$duration" $i]] } {
    set t [expr $t * 1.0]
  }
  if { [string equal "s" [string index "$duration" $i]] } {
    set t [expr $t * 1000.0]
  }

  return $t

}

proc then_it_should_take_less_than { duration } {

  global graprof_time_taken

  set should [time_expand $duration]
  set have   [time_expand $graprof_time_taken]

  if { [expr $have >= $should] } {
    fail_step "$graprof_time_taken >= $duration"
    return
  }

  pass_step

}
asparagus_register_step then_it_should_take_less_than "then it should take less than"

proc then_there_should_be_a_file { filename } {

  if { ! [file exists $filename] } {
    fail_step "does not exist"
  }

  pass_step

}
asparagus_register_step then_there_should_be_a_file "then there should be a file"

proc then_there_should_not_be_a_file { filename } {

  if { [file exists $filename] } {
    fail_step "exists"
  }

  pass_step

}
asparagus_register_step then_there_should_not_be_a_file "then there should not be a file"
