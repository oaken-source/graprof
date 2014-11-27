
proc when_I_time { } {

  global asparagus_executable_path

  set time_start [clock clicks -milliseconds]

  for { set i 0 } { $i < 10 } { incr i } {
    if { [ catch {
      spawn $asparagus_executable_path
      wait $spawn_id
    } msg ] } {
      faíl_step "$msg"
      return
    }
  }

  set time_taken [expr [expr [clock clicks -milliseconds] - $time_start] / 10]

  send_log "TIME: $time_taken ms\n"

  pass_step

}
asparagus_register_step when_I_time "when I time"
