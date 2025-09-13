## GDB Debugging Session: HPX Thread Creation

I ran `HPX.cpp` - very simple pure HPX program and profiled it to examine thread spawning. 
`nsys` showed 7 `pthread_create` calls and I ran it with `--hpx:threads 2`. Also `gdb` showed 7 calls to `pthread_create`.

### Setup and Execution

1.  **Run GDB with Arguments:**
    The executable was started using `gdb` to trace its execution. The `--hpx:threads 2` argument was passed to the HPX application.
    ```bash
    gdb --args ./build/rebuildHPX/hpx_only/exec_hpx --hpx:threads 2
    ```

2.  **Set Breakpoint:**
    Inside GDB, a breakpoint was set on `pthread_create` to halt execution when a new thread is created.
    ```gdb
    break pthread_create
    ```

---

### GDB Output and Backtrace

After running the program, the breakpoint was triggered. The following is the complete output from the GDB session, including the backtrace (`bt`) command.

# Thread 1
```console
#0  __pthread_create_2_1 (newthread=0x7fffffffbd78, attr=0x0, start_routine=0x7ffff70e1210, arg=0x555555681f50) at ./nptl/pthread_create.c:631

#1  0x00007ffff70e12f9 in std::thread::_M_start_thread(std::unique_ptr<std::thread::_State, std::default_delete<std::thread::_State> >, void (*)()) () from /lib/x86_64-linux-gnu/libstdc++.so.6

#2  0x00007ffff7d63e83 in hpx::util::io_service_pool::run_locked(unsigned long, bool, hpx::util::barrier*) () from /home/petricic1/hpx-gcc11-nordtsc/lib/libhpx_core.so

#3  0x00007ffff7d64627 in hpx::util::io_service_pool::run(bool, hpx::util::barrier*) () from /home/petricic1/hpx-gcc11-nordtsc/lib/libhpx_core.so

#4  0x00007ffff7de132c in hpx::runtime::start(hpx::function<int (), false> const&, bool) () from /home/petricic1/hpx-gcc11-nordtsc/lib/libhpx_core.so

#5  0x00007ffff7dd638c in hpx::runtime::run(hpx::function<int (), false> const&) () from /home/petricic1/hpx-gcc11-nordtsc/lib/libhpx_core.so

#6  0x00007ffff7d5d8c4 in hpx::local::detail::run(hpx::runtime&, hpx::function<int (hpx::program_options::variables_map&), false> const&, hpx::program_options::variables_map&, hpx::move_only_function<void (), false>, hpx::move_only_function<void (), false>) () from /home/petricic1/hpx-gcc11-nordtsc/lib/libhpx_core.so

#7  0x00007ffff7d5e773 in hpx::local::detail::run_or_start(bool, 
std::unique_ptr<hpx::runtime, std::default_delete<hpx::runtime> >, hpx::local::detail::command_line_handling&, hpx::move_only_function<void (), false>, hpx::move_only_function<void (), false>) () from /home/petricic1/hpx-gcc11-nordtsc/lib/libhpx_core.so

#8  0x00007ffff7d602c3 in hpx::local::detail::run_or_start(hpx::function<int (hpx::program_options::variables_map&), false> const&, int, char**, hpx::local::init_params const&, bool) ()
   from /home/petricic1/hpx-gcc11-nordtsc/lib/libhpx_core.so

#9  0x0000555555564fe4 in hpx::local::detail::init_start_impl (f=..., argc=3, argv=0x7fffffffcdb8, params=..., blocking=true)
    at /home/petricic1/hpx-gcc11-nordtsc/include/hpx/init_runtime_local/init_runtime_local.hpp:155

#10 0x000055555556511b in hpx::local::init (f=..., argc=3, argv=0x7fffffffcdb8, params=...) at /home/petricic1/hpx-gcc11-nordtsc/include/hpx/init_runtime_local/init_runtime_local.hpp:181

#11 0x00005555555634b7 in main (argc=3, argv=0x7fffffffcdb8) at /home/petricic1/Desktop/JSC/Learning/Week5/src/HPX.cpp:21
```
Maybe some kind of a I/O srvice thread (look at #3). 
- handles blocking and asynchronous I/O operations
- https://hpx-docs.stellar-group.org/latest/html/libs/core/io_service/api/io_service_pool.html 
# # Thread 2 - [New Thread 0x7ffff5dfe000 (LWP 932436)]

```
#0  __pthread_create_2_1 (newthread=0x7fffffffbb18, attr=0x0, start_routine=0x7ffff70e1210, arg=0x555555695200) at ./nptl/pthread_create.c:631

#1  0x00007ffff70e12f9 in std::thread::_M_start_thread(std::unique_ptr<std::thread::_State, std::default_delete<std::thread::_State> >, void (*)()) () from /lib/x86_64-linux-gnu/libstdc++.so.6

#2  0x00007ffff7d63e83 in hpx::util::io_service_pool::run_locked(unsigned long, bool, hpx::util::barrier*) () from /home/petricic1/hpx-gcc11-nordtsc/lib/libhpx_core.so

#3  0x00007ffff7d64627 in hpx::util::io_service_pool::run(bool, hpx::util::barrier*) () from /home/petricic1/hpx-gcc11-nordtsc/lib/libhpx_core.so

#4  0x00007ffff7e685e9 in hpx::threads::threadmanager::run() const () from /home/petricic1/hpx-gcc11-nordtsc/lib/libhpx_core.so

#5  0x00007ffff7de1412 in hpx::runtime::start(hpx::function<int (), false> const&, bool) () from /home/petricic1/hpx-gcc11-nordtsc/lib/libhpx_core.so

#6  0x00007ffff7dd638c in hpx::runtime::run(hpx::function<int (), false> const&) () from /home/petricic1/hpx-gcc11-nordtsc/lib/libhpx_core.so

#7  0x00007ffff7d5d8c4 in hpx::local::detail::run(hpx::runtime&, hpx::function<int (hpx::program_options::variables_map&), false> const&, hpx::program_options::variables_map&, hpx::move_only_function<void (), false>, hpx::move_only_function<void (), false>) () from /home/petricic1/hpx-gcc11-nordtsc/lib/libhpx_core.so

#8  0x00007ffff7d5e773 in hpx::local::detail::run_or_start(bool, 
std::unique_ptr<hpx::runtime, std::default_delete<hpx::runtime> >, hpx::local::detail::command_line_handling&, hpx::move_only_function<void (), false>, hpx::move_only_function<void (), false>) () from /home/petricic1/hpx-gcc11-nordtsc/lib/libhpx_core.so

#9  0x00007ffff7d602c3 in hpx::local::detail::run_or_start(hpx::function<int (hpx::program_options::variables_map&), false> const&, int, char**, hpx::local::init_params const&, bool) ()
   from /home/petricic1/hpx-gcc11-nordtsc/lib/libhpx_core.so

#10 0x0000555555564fe4 in hpx::local::detail::init_start_impl (f=..., argc=3, argv=0x7fffffffcdb8, params=..., blocking=true)
    at /home/petricic1/hpx-gcc11-nordtsc/include/hpx/init_runtime_local/init_runtime_local.hpp:155

#11 0x000055555556511b in hpx::local::init (f=..., argc=3, argv=0x7fffffffcdb8, params=...) at /home/petricic1/hpx-gcc11-nordtsc/include/hpx/init_runtime_local/init_runtime_local.hpp:181

#12 0x00005555555634b7 in main (argc=3, argv=0x7fffffffcdb8) at /home/petricic1/Desktop/JSC/Learning/Week5/src/HPX.cpp:21
```
T
# Thread 3 - [New Thread 0x7ffff55fd000 (LWP 932437)]
```
#0  __pthread_create_2_1 (newthread=0x7fffffffbb18, attr=0x0, start_routine=0x7ffff70e1210, arg=0x55555567a310) at ./nptl/pthread_create.c:631

#1  0x00007ffff70e12f9 in std::thread::_M_start_thread(std::unique_ptr<std::thread::_State, std::default_delete<std::thread::_State> >, void (*)()) () from /lib/x86_64-linux-gnu/libstdc++.so.6

#2  0x00007ffff7d63e83 in hpx::util::io_service_pool::run_locked(unsigned long, bool, hpx::util::barrier*) () from /home/petricic1/hpx-gcc11-nordtsc/lib/libhpx_core.so

#3  0x00007ffff7d64627 in hpx::util::io_service_pool::run(bool, hpx::util::barrier*) () from /home/petricic1/hpx-gcc11-nordtsc/lib/libhpx_core.so

#4  0x00007ffff7e685e9 in hpx::threads::threadmanager::run() const () from /home/petricic1/hpx-gcc11-nordtsc/lib/libhpx_core.so

#5  0x00007ffff7de1412 in hpx::runtime::start(hpx::function<int (), false> const&, bool) () from /home/petricic1/hpx-gcc11-nordtsc/lib/libhpx_core.so

#6  0x00007ffff7dd638c in hpx::runtime::run(hpx::function<int (), false> const&) () from /home/petricic1/hpx-gcc11-nordtsc/lib/libhpx_core.so

#7  0x00007ffff7d5d8c4 in hpx::local::detail::run(hpx::runtime&, hpx::function<int (hpx::program_options::variables_map&), false> const&, hpx::program_options::variables_map&, hpx::move_only_function<void (), false>, hpx::move_only_function<void (), false>) () from /home/petricic1/hpx-gcc11-nordtsc/lib/libhpx_core.so

#8  0x00007ffff7d5e773 in hpx::local::detail::run_or_start(bool, std::unique_ptr<hpx::runtime, std::default_delete<hpx::runtime> >, hpx::local::detail::command_line_handling&, hpx::move_only_function<void (), false>, hpx::move_only_function<void (), false>) () from /home/petricic1/hpx-gcc11-nordtsc/lib/libhpx_core.so

#9  0x00007ffff7d602c3 in hpx::local::detail::run_or_start(hpx::function<int (hpx::program_options::variables_map&), false> const&, int, char**, hpx::local::init_params const&, bool) ()
   from /home/petricic1/hpx-gcc11-nordtsc/lib/libhpx_core.so

#10 0x0000555555564fe4 in hpx::local::detail::init_start_impl (f=..., argc=3, argv=0x7fffffffcdb8, params=..., blocking=true)
    at /home/petricic1/hpx-gcc11-nordtsc/include/hpx/init_runtime_local/init_runtime_local.hpp:155

#11 0x000055555556511b in hpx::local::init (f=..., argc=3, argv=0x7fffffffcdb8, params=...) at /home/petricic1/hpx-gcc11-nordtsc/include/hpx/init_runtime_local/init_runtime_local.hpp:181

#12 0x00005555555634b7 in main (argc=3, argv=0x7fffffffcdb8) at /home/petricic1/Desktop/JSC/Learning/Week5/src/HPX.cpp:21
```
Thread 2 and 3 seem to be similar, I think HPX creates timer threads that manage a queue of tasks and notify the system when they are ready. These threads could be involved in that.
# Thread 4 - [New Thread 0x7ffff4dfc000 (LWP 932438)]
```
#0  __pthread_create_2_1 (newthread=0x7fffffffb870, attr=0x0, start_routine=0x7ffff70e1210, arg=0x555555680370) at ./nptl/pthread_create.c:631

#1  0x00007ffff70e12f9 in std::thread::_M_start_thread(std::unique_ptr<std::thread::_State, std::default_delete<std::thread::_State> >, void (*)()) () from /lib/x86_64-linux-gnu/libstdc++.so.6

#2  0x00007ffff7e1f461 in hpx::threads::detail::scheduled_thread_pool<hpx::threads::policies::local_priority_queue_scheduler<std::mutex, hpx::threads::policies::lockfree_fifo, hpx::threads::policies::lockfree_fifo, hpx::threads::policies::lockfree_fifo> >::add_processing_unit_internal(unsigned long, unsigned long, std::shared_ptr<hpx::util::barrier>, hpx::error_code&) ()
   from /home/petricic1/hpx-gcc11-nordtsc/lib/libhpx_core.so

#3  0x00007ffff7e2c9dc in hpx::threads::detail::scheduled_thread_pool<hpx::threads::policies::local_priority_queue_scheduler<std::mutex, hpx::threads::policies::lockfree_fifo, hpx::threads::policies::lockfree_fifo, hpx::threads::policies::lockfree_fifo> >::run(std::unique_lock<std::mutex>&, unsigned long) () from /home/petricic1/hpx-gcc11-nordtsc/lib/libhpx_core.so

#4  0x00007ffff7e68651 in hpx::threads::threadmanager::run() const () from /home/petricic1/hpx-gcc11-nordtsc/lib/libhpx_core.so

#5  0x00007ffff7de1412 in hpx::runtime::start(hpx::function<int (), false> const&, bool) () from /home/petricic1/hpx-gcc11-nordtsc/lib/libhpx_core.so

#6  0x00007ffff7dd638c in hpx::runtime::run(hpx::function<int (), false> const&) () from /home/petricic1/hpx-gcc11-nordtsc/lib/libhpx_core.so

#7  0x00007ffff7d5d8c4 in hpx::local::detail::run(hpx::runtime&, hpx::function<int (hpx::program_options::variables_map&), false> const&, hpx::program_options::variables_map&, hpx::move_only_function<void (), false>, hpx::move_only_function<void (), false>) () from /home/petricic1/hpx-gcc11-nordtsc/lib/libhpx_core.so

#8  0x00007ffff7d5e773 in hpx::local::detail::run_or_start(bool, std::unique_ptr<hpx::runtime, std::default_delete<hpx::runtime> >, hpx::local::detail::command_line_handling&, hpx::move_only_function<void (), false>, hpx::move_only_function<void (), false>) () from /home/petricic1/hpx-gcc11-nordtsc/lib/libhpx_core.so

#9  0x00007ffff7d602c3 in hpx::local::detail::run_or_start(hpx::function<int (hpx::program_options::variables_map&), false> const&, int, char**, hpx::local::init_params const&, bool) ()
   from /home/petricic1/hpx-gcc11-nordtsc/lib/libhpx_core.so

#10 0x0000555555564fe4 in hpx::local::detail::init_start_impl (f=..., argc=3, argv=0x7fffffffcdb8, params=..., blocking=true)
    at /home/petricic1/hpx-gcc11-nordtsc/include/hpx/init_runtime_local/init_runtime_local.hpp:155

#11 0x000055555556511b in hpx::local::init (f=..., argc=3, argv=0x7fffffffcdb8, params=...) at /home/petricic1/hpx-gcc11-nordtsc/include/hpx/init_runtime_local/init_runtime_local.hpp:181

#12 0x00005555555634b7 in main (argc=3, argv=0x7fffffffcdb8) at /home/petricic1/Desktop/JSC/Learning/Week5/src/HPX.cpp:21
```
# Thread 5 - [New Thread 0x7fffe7fff000 (LWP 932439)]
```
#0  __pthread_create_2_1 (newthread=0x7fffffffb870, attr=0x0, start_routine=0x7ffff70e1210, arg=0x5555556866b0) at ./nptl/pthread_create.c:631

#1  0x00007ffff70e12f9 in std::thread::_M_start_thread(std::unique_ptr<std::thread::_State, std::default_delete<std::thread::_State> >, void (*)()) () from /lib/x86_64-linux-gnu/libstdc++.so.6

#2  0x00007ffff7e1f461 in hpx::threads::detail::scheduled_thread_pool<hpx::threads::policies::local_priority_queue_scheduler<std::mutex, hpx::threads::policies::lockfree_fifo, hpx::threads::policies::lockfree_fifo, hpx::threads::policies::lockfree_fifo> >::add_processing_unit_internal(unsigned long, unsigned long, std::shared_ptr<hpx::util::barrier>, hpx::error_code&) ()
   from /home/petricic1/hpx-gcc11-nordtsc/lib/libhpx_core.so

#3  0x00007ffff7e2c9dc in hpx::threads::detail::scheduled_thread_pool<hpx::threads::policies::local_priority_queue_scheduler<std::mutex, hpx::threads::policies::lockfree_fifo, hpx::threads::policies::lockfree_fifo, hpx::threads::policies::lockfree_fifo> >::run(std::unique_lock<std::mutex>&, unsigned long) () from /home/petricic1/hpx-gcc11-nordtsc/lib/libhpx_core.so

#4  0x00007ffff7e68651 in hpx::threads::threadmanager::run() const () from /home/petricic1/hpx-gcc11-nordtsc/lib/libhpx_core.so

#5  0x00007ffff7de1412 in hpx::runtime::start(hpx::function<int (), false> const&, bool) () from /home/petricic1/hpx-gcc11-nordtsc/lib/libhpx_core.so

#6  0x00007ffff7dd638c in hpx::runtime::run(hpx::function<int (), false> const&) () from /home/petricic1/hpx-gcc11-nordtsc/lib/libhpx_core.so

#7  0x00007ffff7d5d8c4 in hpx::local::detail::run(hpx::runtime&, hpx::function<int (hpx::program_options::variables_map&), false> const&, hpx::program_options::variables_map&, hpx::move_only_function<void (), false>, hpx::move_only_function<void (), false>) () from /home/petricic1/hpx-gcc11-nordtsc/lib/libhpx_core.so

#8  0x00007ffff7d5e773 in hpx::local::detail::run_or_start(bool, std::unique_ptr<hpx::runtime, std::default_delete<hpx::runtime> >, hpx::local::detail::command_line_handling&, hpx::move_only_function<void (), false>, hpx::move_only_function<void (), false>) () from /home/petricic1/hpx-gcc11-nordtsc/lib/libhpx_core.so

#9  0x00007ffff7d602c3 in hpx::local::detail::run_or_start(hpx::function<int (hpx::program_options::variables_map&), false> const&, int, char**, hpx::local::init_params const&, bool) ()
   from /home/petricic1/hpx-gcc11-nordtsc/lib/libhpx_core.so

#10 0x0000555555564fe4 in hpx::local::detail::init_start_impl (f=..., argc=3, argv=0x7fffffffcdb8, params=..., blocking=true)
    at /home/petricic1/hpx-gcc11-nordtsc/include/hpx/init_runtime_local/init_runtime_local.hpp:155

#11 0x000055555556511b in hpx::local::init (f=..., argc=3, argv=0x7fffffffcdb8, params=...) at /home/petricic1/hpx-gcc11-nordtsc/include/hpx/init_runtime_local/init_runtime_local.hpp:181

#12 0x00005555555634b7 in main (argc=3, argv=0x7fffffffcdb8) at /home/petricic1/Desktop/JSC/Learning/Week5/src/HPX.cpp:21
```
Thread 4 and 5 are most likely worker threads, only these 2 have a similar output with `add_processing_unit_internal` which could be HPX adding them to task scheduler pool.
# Thread 6 - [New Thread 0x7fffe77fe000 (LWP 932440)]
```
Hello from the main HPX thread 1.
Hello from HPX worker thread 0!

#0  __pthread_create_2_1 (newthread=0x7fffffffbdc8, attr=0x0, start_routine=0x7ffff70e1210, arg=0x55555567e830) at ./nptl/pthread_create.c:631

#1  0x00007ffff70e12f9 in std::thread::_M_start_thread(std::unique_ptr<std::thread::_State, std::default_delete<std::thread::_State> >, void (*)()) () from /lib/x86_64-linux-gnu/libstdc++.so.6

#2  0x00007ffff7dddc7d in hpx::runtime::wait() () from /home/petricic1/hpx-gcc11-nordtsc/lib/libhpx_core.so

#3  0x00007ffff7dd6395 in hpx::runtime::run(hpx::function<int (), false> const&) () from /home/petricic1/hpx-gcc11-nordtsc/lib/libhpx_core.so

#4  0x00007ffff7d5d8c4 in hpx::local::detail::run(hpx::runtime&, hpx::function<int (hpx::program_options::variables_map&), false> const&, hpx::program_options::variables_map&, hpx::move_only_function<void (), false>, hpx::move_only_function<void (), false>) () from /home/petricic1/hpx-gcc11-nordtsc/lib/libhpx_core.so

#5  0x00007ffff7d5e773 in hpx::local::detail::run_or_start(bool, std::unique_ptr<hpx::runtime, std::default_delete<hpx::runtime> >, hpx::local::detail::command_line_handling&, hpx::move_only_function<void (), false>, hpx::move_only_function<void (), false>) () from /home/petricic1/hpx-gcc11-nordtsc/lib/libhpx_core.so

#6  0x00007ffff7d602c3 in hpx::local::detail::run_or_start(hpx::function<int (hpx::program_options::variables_map&), false> const&, int, char**, hpx::local::init_params const&, bool) ()
   from /home/petricic1/hpx-gcc11-nordtsc/lib/libhpx_core.so

#7  0x0000555555564fe4 in hpx::local::detail::init_start_impl (f=..., argc=3, argv=0x7fffffffcdb8, params=..., blocking=true)
    at /home/petricic1/hpx-gcc11-nordtsc/include/hpx/init_runtime_local/init_runtime_local.hpp:155

#8  0x000055555556511b in hpx::local::init (f=..., argc=3, argv=0x7fffffffcdb8, params=...) at /home/petricic1/hpx-gcc11-nordtsc/include/hpx/init_runtime_local/init_runtime_local.hpp:181

#9  0x00005555555634b7 in main (argc=3, argv=0x7fffffffcdb8) at /home/petricic1/Desktop/JSC/Learning/Week5/src/HPX.cpp:21
```
Something regarding the termination of HPX runtime, I am not really sure what this is used for (#2)
- https://hpx-docs.stellar-group.org/latest/html/libs/core/runtime_local/api/runtime_local.html?highlight=runtime%20wait#_CPPv4N3hpx7runtime4waitEv
# Thread 7 - [New Thread 0x7fffe6ffd000 (LWP 932441)]
```
[Thread 0x7fffe6ffd000 (LWP 932441) exited]
[Thread 0x7fffe77fe000 (LWP 932440) exited]
[Thread 0x7fffe7fff000 (LWP 932439) exited]
[Thread 0x7ffff4dfc000 (LWP 932438) exited]
[Thread 0x7ffff55fd000 (LWP 932437) exited]
[Thread 0x7ffff65ff000 (LWP 932435) exited]
[Thread 0x7ffff5dfe000 (LWP 932436) exited]
[Inferior 1 (process 885778) exited normally]
```
Not and HPX thread, probably used for the orderly shudown of the whole multi-threaded process.
- I have tried with different values of `--hpx:threads` and it seems that the formula is:  
`pthread_create calls = 5 + hpx:threads`.