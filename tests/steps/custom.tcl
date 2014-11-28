
proc when_I_time { } {

  global asparagus_executable_path

  set iterations 50

  set time_start [clock clicks -milliseconds]

  for { set i 0 } { $i < $iterations } { incr i } {
    if { [ catch {
      spawn $asparagus_executable_path
      wait $spawn_id
    } msg ] } {
      faíl_step "$msg"
      return
    }
  }

  set time_taken [expr [expr [clock clicks -milliseconds] - $time_start] / [expr $iterations]]

  send_log "TIME: $time_taken ns\n"

  pass_step

}
asparagus_register_step when_I_time "when I time"
