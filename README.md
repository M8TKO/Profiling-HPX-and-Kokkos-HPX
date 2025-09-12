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