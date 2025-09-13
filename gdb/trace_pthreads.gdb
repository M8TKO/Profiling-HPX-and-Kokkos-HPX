# --- GDB Script to trace pthread_create calls ---

set breakpoint pending on
shell > pthread_calls_cor.log

# 1. Enable logging to capture the output
set logging file pthread_calls_cor.log
set logging enabled on

set $count = 0

# 2. Set a pending breakpoint
break pthread_create

# 3. Define the commands for the breakpoint
commands 1
  silent
  set $count = $count + 1
  printf "\n--- pthread_create called from: ---\n"
  bt
  continue
end

# 4. Run the program and then exit
run
print $count
quit

# Run with: gdb -x gdb/trace_pthreads.gdb --args ./build/rebuildHPX/hpx_only/exec --hpx:threads 2