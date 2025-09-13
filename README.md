## GDB Debugging Session: Kokkos with HPX Backend Thread Creation

1. I ran `KokoksHPXBackend.cpp` with no command line variables set and used `gdb` to count the number of times the function `pthread_create` was called. 
```
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
$1 = 25
```
2. Same `.cpp` but I set `--hpx:threads 2`. The `gdb` commands remained the same.
```
run --hpx:threads 2
```

```
print $count
$1 = 25
```
3. Same `.cpp` but I set `–kokkos-threads=2`. The `gdb` commands remained the same.
```
run –kokkos-threads=2
```

```
print $count
$1 = 25
```

# Looking at the backtraces from each call
Instead of using `gdb` in the command line, I made a helper file `src/trace_pthreads.gdb`


```
## Output
- Total calls to `pthread_create` is `25`. 
- Backtrace of each thread starts off with a `main` function call. \
`27/28` start of with a call `#0x000055555556d1e3 in main (argc=3, argv=0x7fffffffcf48) at /home/petricic1/Desktop/JSC/Learning/Week5/src/KokkosHPXBackend.cpp:8
`. The exception is the last thread which starts off with  \
`0x000055555556d512 in main (argc=3, argv=0x7fffffffcf48) at /home/petricic1/Desktop/JSC/Learning/Week5/src/KokkosHPXBackend.cpp:39`
Looking at the `.cpp` file, the making of first `24` threads starts at  `Kokkos::initialize(argc, argv);` and the last one starts at `Kokkos::finalize();`. Judging from the rest of the backtrace of the `25th`
 thread, it could be in charge of finalizing the `hpx` runtime.  
 - The only difference between the backtraces of the first `2` threads is the final call to `pthread_create` (arg=):
 ```
 __pthread_create_2_1 (newthread=0x7fffffffbea8, attr=0x0, start_routine=0x7ffff6ee1210, arg=0x555555736190) at ./nptl/pthread_create.c:631

vs

#0  __pthread_create_2_1 (newthread=0x7fffffffbea8, attr=0x0, start_routine=0x7ffff6ee1210, arg=0x5555557414e0) at ./nptl/pthread_create.c:631
 ```
- The only difference between 3 and 4 is the final call to `pthread_create` (arg=):
 ```
 __pthread_create_2_1 (newthread=0x7fffffffbc48, attr=0x0, start_routine=0x7ffff6ee1210, arg=0x5555557315c0) at ./nptl/pthread_create.c:631

 vs

#0  __pthread_create_2_1 (newthread=0x7fffffffbc48, attr=0x0, start_routine=0x7ffff6ee1210, arg=0x555555728c30) at ./nptl/pthread_create.c:631
 ```
 - The difference betwwen 1,2 and 3,4 is that 3,4 call
 `0x00007ffff74685e9 in hpx::threads::threadmanager::run() const () from /home/petricic1/hpx-gcc11-nordtsc/lib/libhpx_core.so`
 - After this there are no more `hpx::util::io_service_pool` calls.
 - Backtraces of threads `5` to `24` are the same except the final call to `pthreads_create` when the `arg=` is different.
 - If these are the worker threads, there is `24 - 5 + 1 = 20` of them which is the exact number of cores on my workstation. Hyperthreading would bump up that number to `28`. If we ignore the Kokkos part of all these backtraces, calls differ a bit from the backtraces HPX makes by itself.