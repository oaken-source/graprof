
proc when_I_time { } {

  global asparagus_executable_path

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

    send_user "debug: [expr $time_taken / 10.0]\n"

  }

  set time_total [expr $time_total / $iterations]

  send_log "TIME: $time_total ns\n"

  pass_step

}
asparagus_register_step when_I_time "when I time"
