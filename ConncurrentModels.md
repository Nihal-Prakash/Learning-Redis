# Redis from scratch

## Concurrent IO Models

### Thread Based Concurrency

For the server be able to handle multiple different clients *simultaneously* we will use **Multi-Threading**

**PseudoCode**

```
fd = socket()
bind(fd, address)
listen(fd)
while True:
    conn_fd = accept(fd)
    new_thread(do_something_with, conn_fd)
    # continue to accept the next client without blocking

def do_something_with(conn_fd):
    while not_quiting(conn_fd):
        req = read_request(conn_fd)     # blocks thread
        res = process(req)
        write_response(conn_fd, res)    # blocks thread
    close(conn_fd)

```

Why isnt threading enough?
  1. *Memory usage* : Threads use stacks which used for local variables and function calls, memory usage per thread is hard to control

  2. *Overhead* : Stateless  clients like PHP apps will create many short-lived connections adding to latency and CPU usage.

### Concurrent

It is possible to have concurrent IO without multithreading  since syscalls `read()` and `write()` wait for the buffer to usable.

**PseudoCode**

```
while running:
    want_read = [...]           # socket fds
    want_write = [...]          # socket fds
    can_read, can_write = wait_for_readiness(want_read, want_write) # blocks!
    for fd in can_read:
        data = read_nb(fd)      # non-blocking, only consume from the buffer
        handle_data(fd, data)   # application logic without IO
    for fd in can_write:
        data = pending_data(fd) # produced by the application
        n = write_nb(fd, data)  # non-blocking, only append to the buffer
        data_written(fd, n)     # n <= len(data), limited by the available space

```

**Mechanisms**
1. Readiness notification 
2. Non-blocking read
3. Non-blocking write

> [!IMPORTANT]
> Non-blocking reads & writes use the same syscalls as blocking reads & writes.
>The `0_NONBLOCK` flag puts a socket in non-blocking mode. 

### Readiness API

Waiting for IO readiness is platform specific, and there are several ones on Linux.

    can_read, can_write = wait_for_readiness(want_read, want_write)
The simplest one on Linux is poll().

```
int poll(struct pollfd *fds, nfds_t nfds, int timeout);

struct pollfd {
    int   fd;
    short events;   // request: want to read, write, or both?
    short revents;  // returned: can read? can write?
};
```

`poll()` takes an array of fds, each with an input flag and an output flag:

The events flag indicates whether you want to read (POLLIN), write (POLLOUT), or both (POLLIN|POLLOUT).
The revents flag returned from the syscall indicates the readiness.
The timeout argument is used to implement timers later.

We will use `poll()` because it’s the simplest. But note that `epoll` is the default choice on Linux as it’s more scalable and should be used in real projects instead. All the readiness APIs are just differs in shapes, using them isn’t much different.

> [!WARNING]
> Readiness APIs cannot be used with files
> All the readiness APIs can only be used with sockets, pipes, and some special stuff like signalfd.
> They cannot be used with disk files!
> Why? Because when a socket is ready to read, it means that the data is in the read buffer,
> so the read is guaranteed not to block, but for a disk file, no such buffer exists in the kernel, so the readiness for a disk file is undefined.

On Linux, file IO within an event loop may be possible with io_uring, which is a unified interface for both file IO and socket IO. But io_uring is a very different API, so we will not pursue it.

