## GDB Debugging Session: Kokkos with HPX Backend Thread Creation

1. I ran `KokoksHPXBackend.cpp` with no command line variables set and used `gdb` to count the number of times the function `pthread_create` was called. 
```
gdb ./build/rebuild/cuda/exec
set $count = 0
break pthread_create
```
```
commands 1
silent
set $count = $count + 1
continue 
end
```
```
run
```
```
print $count
$1 = 28
```
2. Same `.cpp` but I set `--hpx:threads 2`. The `gdb` commands remained the same.
```
run --hpx:threads 2
```

```
print $count
$1 = 28
```
3. Same `.cpp` but I set `–kokkos-threads=3`. The `gdb` commands remained the same.
```
run –kokkos-threads=3
```

```
print $count
$1 = 28
```

# Looking at the backtraces from each call
Enable logging to a file
```
set logging file pthread_calls.log
set logging enabled on
```
Set a breakpoint that prints a backtrace and continues
```
break pthread_create  
commands 1
  silent
  bt
  echo \n--- pthread_create called from: ---\n
  continue
end
```

Run the program and then check the log file
```
run --hpx:threads 2
```
## Output
- Total calls to `pthread_create` is `25`. 
- Backtrace of each thread starts off with a `main` function call. \
`27/28` start of with a call `#0x000055555556d1e3 in main (argc=3, argv=0x7fffffffcf48) at /home/petricic1/Desktop/JSC/Learning/Week5/src/KokkosHPXBackend.cpp:8
`. The exception is the last thread which starts off with  \
`0x000055555556d512 in main (argc=3, argv=0x7fffffffcf48) at /home/petricic1/Desktop/JSC/Learning/Week5/src/KokkosHPXBackend.cpp:39`
Looking at the `.cpp` file, the making of first `24` threads starts at  `Kokkos::initialize(argc, argv);` and the last one starts at `Kokkos::finalize();`. Judging from the rest of the backtrace of the `25th`
 thread, it could be in charge of finalizing the `hpx` runtime. 