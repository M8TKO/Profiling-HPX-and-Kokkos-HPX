set breakpoint pending on
shell > pthread_calls_HPXbackend.log

set logging file pthread_calls_HPXbackend.log
set logging enabled on

set $count = 0

break pthread_create

commands 1
  silent
  set $count = $count + 1
  printf "\n %d--- pthread_create called from: ---\n", $count
  bt
  continue
end

run
print $count
quit

# Run with: gdb -x gdb/trace_pthreads.gdb --args ./build/rebuildHPXvsMP/cuda/exec_HPXvsMP 