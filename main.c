#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/sysinfo.h>
#include <sys/times.h>

int
main (int argc, char **argv)
{
    char summary[1024]="";
    char tmp[1024]="";
    clock_t ct_start_time, ct_last_time, ct_now_time;
    struct tms tms_start_time, tms_last_time, tms_now_time;
    //struct tms {
    //    clock_t tms_utime;  /* user time */
    //    clock_t tms_stime;  /* system time */
    //    clock_t tms_cutime; /* user time of children */
    //    clock_t tms_cstime; /* system time of children */
    //};
    //
    int clock_ticks_per_second = sysconf(_SC_CLK_TCK);
    printf("Clock ticks per second = %d\n", clock_ticks_per_second);

    struct sysinfo info;
    sysinfo (&info);
    //Since Linux 2.3.23 (i386) and Linux 2.3.48 (all architectures) the structure is:
    //    struct sysinfo {
    //        long uptime;             /* Seconds since boot */
    //        unsigned long loads[3];  /* 1, 5, and 15 minute load averages */
    //        unsigned long totalram;  /* Total usable main memory size */
    //        unsigned long freeram;   /* Available memory size */
    //        unsigned long sharedram; /* Amount of shared memory */
    //        unsigned long bufferram; /* Memory used by buffers */
    //        unsigned long totalswap; /* Total swap space size */
    //        unsigned long freeswap;  /* Swap space still available */
    //        unsigned short procs;    /* Number of current processes */
    //        unsigned long totalhigh; /* Total high memory size */
    //        unsigned long freehigh;  /* Available high memory size */
    //        unsigned int mem_unit;   /* Memory unit size in bytes */
    //        char _f[20-2*sizeof(long)-sizeof(int)];
    //                                 /* Padding to 64 bytes */
    //    };
    //In the above structure, sizes of the memory and swap fields are given as multiples of mem_unit bytes.
    printf ("mem_unit (bytes)= %d\n", info.mem_unit);
    //TODO: addjust the program to take care of multi byte unit systems

    printf ("I have total %ld Bytes / %ld MB of installed RAM\n",
            info.totalram, info.totalram / (1024 * 1024));
    printf ("I have available %ld Bytes / %ld MB of free RAM\n",
            info.freeram, info.freeram / (1024 * 1024));

    printf ("size of size_t = %d size of unsigned long = %d\n", sizeof(size_t), sizeof(unsigned long));
    // exit (0);

    unsigned long *playground, *start;
    int allocated = 0;
    unsigned long int_count=0;
    float mem_percentage=0.99;
    do
    {
        int_count=(unsigned long)((info.freeram * mem_percentage)/sizeof (unsigned long));
        playground = (unsigned long *)
            calloc ( int_count, sizeof (unsigned long));
        if (playground == NULL)
        { 
            printf ("memorry allocation failed at : %ld,  %f.\n", int_count, mem_percentage);
            mem_percentage -= 0.02;
        }
        else
        { 
            allocated = 1;
            start=playground;
            printf ("playground is ready. playground = %x , start = %x\n", playground, start);
        }
    }
    while (allocated != 1);

    // Note time before memory write start
    if (( ct_start_time = times(&tms_start_time)) == -1)
        perror("times() error");
    ct_last_time = ct_start_time;
    tms_last_time = tms_start_time;

    // memory write starts
    for (unsigned long i =0; i < int_count; i+=(4096/ sizeof (unsigned long))) {
        // if ( i % 10000 == 0) printf ("%ld %ld\n", int_count, i);
        //*(++playground)=0;
        *playground=0;
        playground+=(4096/ sizeof (unsigned long));
    }

    // Note time after memory write finished
    if (( ct_now_time = times(&tms_now_time)) == -1)
        perror("times() error");
    sprintf (tmp, "%25s == (wall time = %010.6f) (user time = %010.6f) (system time = %010.6f)\n",
            "Memory write time",
            (float)(ct_now_time - ct_last_time)/clock_ticks_per_second,
            (float)(tms_now_time.tms_utime - tms_last_time.tms_utime)/clock_ticks_per_second,
            (float)(tms_now_time.tms_stime - tms_last_time.tms_stime)/clock_ticks_per_second
            );
    strcat(summary, tmp);
    //printf ("\nMemory write time \n== (wall time = %f) (user time = %f) (system time = %f)\n",
    //        (float)(ct_now_time - ct_last_time)/clock_ticks_per_second,
    //        (float)(tms_now_time.tms_utime - tms_last_time.tms_utime)/clock_ticks_per_second,
    //        (float)(tms_now_time.tms_stime - tms_last_time.tms_stime)/clock_ticks_per_second
    //        );

    // Note time before memory read start
    playground=start;
    if (( ct_last_time = times(&tms_last_time)) == -1)
        perror("times() error");
    for (unsigned long i =0; i < int_count; i+=(4096/ sizeof (unsigned long))) {
        playground+=(4096/ sizeof (unsigned long));
        int x=*playground;
    }
    // Note time after memory read finished
    if (( ct_now_time = times(&tms_now_time)) == -1)
        perror("times() error");
    sprintf (tmp, "%25s == (wall time = %010.6f) (user time = %010.6f) (system time = %010.6f)\n",
            "Memory read time",
            (float)(ct_now_time - ct_last_time)/clock_ticks_per_second,
            (float)(tms_now_time.tms_utime - tms_last_time.tms_utime)/clock_ticks_per_second,
            (float)(tms_now_time.tms_stime - tms_last_time.tms_stime)/clock_ticks_per_second
            );
    strcat(summary, tmp);

    unsigned long simulated_file_size = (unsigned long)(info.freeram * mem_percentage);
    sysinfo (&info);
    // printf ("total = %ld\nfree = %ld\nshared = %ld\nbuffram = %ld\ntotalhigh = %ld\nfreehigh = %ld\n", 
    //         info.totalram / (1024 * 1024),
    //         info.freeram / (1024 * 1024),
    //         info.sharedram / (1024 * 1024),
    //         info.bufferram / (1024 * 1024),
    //         info.totalhigh / (1024 * 1024),
    //         info.freehigh / (1024 * 1024)
    //         );
    //                total        used        free      shared  buff/cache   available
    // Mem:           15732       15067         254         197         411         199
    // Swap:              0           0           0
    //
    // total = 15732
    // free = 146
    // shared = 197
    // buffram = 25
    // totalhigh = 0
    // freehigh = 0
    printf ("Used %ld MB memory for memory read/ writes\n",(unsigned long)(simulated_file_size / (1024 * 1024)));
    unsigned long needed_file_size
        = 3
          * (info.freeram
            + info.sharedram
            + info.bufferram);
    printf ("Creating file of %ld Bytes / %ld MB\n", needed_file_size, needed_file_size/ (1024 * 1024));
    unsigned long file_offset = 0;
    unsigned long read_writeoffset = 0;
    FILE *f = fopen("/tmp/playground.txt", "w+");

    // Note time before file write start
    if (( ct_last_time = times(&tms_last_time)) == -1)
        perror("times() error");
    while ( read_writeoffset < simulated_file_size ) {
        read_writeoffset += 4096;
        file_offset = read_writeoffset % needed_file_size;
        fseek(f,file_offset,SEEK_SET);
        fputc('a',f);
    }
    // Note time after file write finished
    if (( ct_now_time = times(&tms_now_time)) == -1)
        perror("times() error");
    //vsprintf (summary,"%s %s",summary,"file write\n");
    //printf ("\nFile write time \n==(wall time = %f) (user time = %f) (system time = %f)\n",
    //        (float)(ct_now_time - ct_last_time)/clock_ticks_per_second,
    //        (float)(tms_now_time.tms_utime - tms_last_time.tms_utime)/clock_ticks_per_second,
    //        (float)(tms_now_time.tms_stime - tms_last_time.tms_stime)/clock_ticks_per_second
    //        );
    sprintf (tmp,"%25s == (wall time = %010.6f) (user time = %010.6f) (system time = %010.6f)\n",
            "File write time",
            (float)(ct_now_time - ct_last_time)/clock_ticks_per_second,
            (float)(tms_now_time.tms_utime - tms_last_time.tms_utime)/clock_ticks_per_second,
            (float)(tms_now_time.tms_stime - tms_last_time.tms_stime)/clock_ticks_per_second
            );
    strcat(summary, tmp);

    // Note time before file read start
    if (( ct_last_time = times(&tms_last_time)) == -1)
        perror("times() error");
    read_writeoffset=0;
    while ( read_writeoffset < simulated_file_size ) {
        read_writeoffset += 4096;
        file_offset = read_writeoffset % needed_file_size;
        fseek(f,file_offset,SEEK_SET);
        int x = fgetc(f);
    }
    // Note time after file write finished
    if (( ct_now_time = times(&tms_now_time)) == -1)
        perror("times() error");
    //vsprintf (summary,"%s %s",summary,"file write\n");
    //printf ("\nFile write time \n==(wall time = %f) (user time = %f) (system time = %f)\n",
    //        (float)(ct_now_time - ct_last_time)/clock_ticks_per_second,
    //        (float)(tms_now_time.tms_utime - tms_last_time.tms_utime)/clock_ticks_per_second,
    //        (float)(tms_now_time.tms_stime - tms_last_time.tms_stime)/clock_ticks_per_second
    //        );
    sprintf (tmp,"%25s == (wall time = %010.6f) (user time = %010.6f) (system time = %010.6f)\n",
            "File read time",
            (float)(ct_now_time - ct_last_time)/clock_ticks_per_second,
            (float)(tms_now_time.tms_utime - tms_last_time.tms_utime)/clock_ticks_per_second,
            (float)(tms_now_time.tms_stime - tms_last_time.tms_stime)/clock_ticks_per_second
            );
    strcat(summary, tmp);
    
    // Print summary
    printf("%s",summary);
    char c= getchar();
}
