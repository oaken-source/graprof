#!/bin/bash

set -e
set -u

if [[ $# -lt 1 ]]; then
  task="pass2"
else
  task=$1
fi

case $task in
clean)
  rm -f gp.out.* graprof.out.*
  ;;
pass[0-9]*)  
  n=${task//[a-z]/}

  GRAPROF_OUT=gp.out.0 graprof -FCMg -o graprof.out.pass0 /usr/local/bin/graprof-gp -- --help &> /dev/null
  echo "[$(date +%T)]  finished pass 0 / $n"
  for (( i=1; i<=$n; i+=1)); do
    GRAPROF_OUT=gp.out.$i graprof -FCMg -o graprof.out.pass$i /usr/local/bin/graprof-gp -- -FCMg /usr/local/bin/graprof-gp -t gp.out.$(($i - 1)) &> /dev/null
    echo "[$(date +%T)]  finished pass $i / $n"
  done
  ;;
*)
  echo invalid task \`$task\'
  exit 1
  ;;
esac
