# Simple Linux Block Device Driver

This is a basic block device driver for Linux. It provides a simple framework for learning and experimenting with Linux storage drivers.

## Features
- Implements a block device with a fixed capacity of 1024 sectors.
- Handles basic read and write operations.

## Prerequisites
- Linux kernel development headers.
- Basic knowledge of Linux kernel module programming.

## Building and Running
1. Build the module:
   ```bash
   make
   ```

2. Load the module:
   ```bash
   sudo insmod simple_block_driver.ko
   ```

3. Check the kernel logs for messages:
   ```bash
   dmesg | tail
   ```

4. Unload the module:
   ```bash
   sudo rmmod simple_block_driver
   ```

5. Clean up the build files:
   ```bash
   make clean
   ```

## Notes
- The current implementation is a skeleton driver and does not support real I/O operations.
- You can extend it by adding file operations and handling I/O requests in the `process_request` function.
