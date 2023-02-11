# iops-test
Test RAM and HDD read/ write throughput

A `Hacker News` post claimed [	I/O is no longer the bottleneck](https://news.ycombinator.com/item?id=33751266) (interesting thread, if you deal in storage)

This program is to test simplest of the data access patterns:
- Write/ read one byte in every block of 4KB in RAM 
- Write/ read one byte in every block of 4KB in file on disk (/tmp for simplicity)

## How program works?
- Allocated large pool of memory.
- Write to this memory in loop (1 byte every 4KB)
- Read the written data
- Create a test file larger than remaining available RAM (/tmp/playground.txt)
- Follow with write and read on disk file, simulating same volume and pattern as in RAM
- report wall time, user time and system time spent in operations

## How does the output look like?
Here is a saample run on a Linux machine, with
- Motherboard: Lenovo 20W40029MX/ width: 64 bits
- CPU: 11th Gen Intel(R) Core(TM) i5-1135G7 @ 2.40GHz (4 core/ 2 thread each)
- RAM: SODIMM DDR4 Synchronous 3200 MHz (0.3 ns)
- NVME: Size: 500118192 sectors a 512 bytes/ Geometry (Logical): CHS 244198/64/32
- OS: Debian 11
- C compiler: GCC, default options
 
```
$ gcc -o iops-test main.c
```
In this run, file was written to/ read from about 6 times to match 12018 MB RAM access.

```
Clock ticks per second = 100
mem_unit (bytes)= 1
I have total 16497295360 Bytes / 15733 MB of installed RAM
I have available 12602064896 Bytes / 12018 MB of free RAM
size of size_t = 8 size of unsigned long = 8
playground is ready. playground = a05f5010 , start = a05f5010
Used 11898 MB memory for memory read/ writes
Creating file of 2335076352 Bytes / 2226 MB

        Memory write time == (wall time = 001.370000) (user time = 000.050000) (system time = 001.300000)
         Memory read time == (wall time = 000.020000) (user time = 000.020000) (system time = 000.000000)
          File write time == (wall time = 065.470001) (user time = 000.800000) (system time = 014.980000)
           File read time == (wall time = 014.700000) (user time = 000.290000) (system time = 003.900000)
```

## Observations

Wall time
- RAM writes are about 47 times faster than disk writes (with no cache effect).
- RAM reads are about 735 times faster than disk reads

User time
- Almost same ratio for read and writes, about 15 times faster with RAM, than disk

System time (most important)
- RAM writes are about 12 times faster
- RAM reads were so fast these were not even recorded

Multiple runs (~10) gave almost same pattern, and I did not want to wear my disk, so not many repeats and fancy charts :D

### What about CPU usage?

- Most of the time CPU use was about 25% for the process with load average about 0.3, peaking about 0.7.
- State was almost always `D`

```
top - 19:30:25 up 56 min,  1 user,  load average: 0.68, 0.35, 0.21
Tasks: 274 total,   1 running, 273 sleeping,   0 stopped,   0 zombie
%Cpu(s):  0.2 us,  7.7 sy,  0.0 ni, 84.5 id,  7.4 wa,  0.0 hi,  0.1 si,  0.0 st
MiB Mem :  15733.0 total,    181.4 free,  13521.1 used,   2030.5 buff/cache
MiB Swap:      0.0 total,      0.0 free,      0.0 used.   1571.9 avail Mem 

    PID USER      PR  NI    VIRT    RES    SHR S  %CPU  %MEM     TIME+ COMMAND                                     
   5245 user      20   0   11.3g  11.3g   1244 D  22.3  73.5   0:12.91 iops-test 
```

while after about a minute of test run

```
top - 19:32:52 up 58 min,  1 user,  load average: 0.24, 0.34, 0.23
```

## Conclusion
Storage is definitely quite fast, but is it fast enough to not having to think of data access patterns?
I am not sure. specialy for read operations, compared to RAM.

Nice will be to see these tests run on cloud instances where IOPS are highly rationed. ;)

Feel free to run this test and see how it works for you.
