set breakpoint pending on

set $count = 0

break pthread_create

commands 1
  silent
  set $count = $count + 1
  continue
end

run
print $count
quit

# Run with: gdb -x gdb/count_pthreads_create.gdb --args ./build/rebuildHPXvsMP/cuda/exec_HPXvsMP --hpx:threads 2