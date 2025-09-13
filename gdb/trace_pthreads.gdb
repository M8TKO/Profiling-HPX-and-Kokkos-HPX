set breakpoint pending on
shell > pthread_calls_cor.log

set logging file pthread_calls_cor.log
set logging enabled on

set $count = 0

break pthread_create

commands 1
  silent
  set $count = $count + 1
  printf "\n $count--- pthread_create called from: ---\n"
  bt
  continue
end

run
print $count
quit

# Run with: gdb -x gdb/trace_pthreads.gdb --args ./build/rebuildHPX/hpx_only/exec --hpx:threads 2