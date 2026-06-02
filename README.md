# C++ Thread Pool

A small C++20 thread pool project built with a plain `Makefile`.

## Features

- Fixed-size worker pool
- `submit(...)` API returning `std::future`
- Graceful shutdown in the destructor
- `wait_for_idle()` helper for synchronization
- Example program and lightweight tests

## Layout

```text
include/   public header
src/       implementation
examples/  demo program
tests/     simple assertions-based test binary
```

## Build

```sh
make
```

## Run the demo

```sh
make demo
./build/demo
```

## Run tests

```sh
make test
```

## Next ideas

- Add a bounded queue mode
- Add task priorities
- Add work stealing
- Benchmark throughput and latency
