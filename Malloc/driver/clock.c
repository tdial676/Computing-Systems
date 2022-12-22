/*
 * clock.c - Routines for using the cycle counters on x86
 *
 * Copyright (c) 2002, R. Bryant and D. O'Hallaron, All rights reserved.
 * May not be used, modified, or copied without permission.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/times.h>
#include "clock.h"


/*******************************************************
 * Machine dependent functions
 *
 * Note: the constants __i386__ and __x86_64__
 * are set by GCC when it calls the C preprocessor
 * You can verify this for yourself using gcc -v.
 *******************************************************/

#if defined(__i386__) || defined(__x86_64__)
/*******************************************************
 * Pentium versions of start_counter() and get_counter()
 *******************************************************/


/* $begin x86cyclecounter */
/* Initialize the cycle counter */
static unsigned cyc_hi = 0;
static unsigned cyc_lo = 0;


/* Set *hi and *lo to the high and low order bits  of the cycle counter.
   Implementation requires assembly code to use the rdtsc instruction. */
void access_counter(unsigned *hi, unsigned *lo)
{
    asm("rdtsc"                     /* Put cycle counter in %edx:%eax */
        : "=d" (*hi), "=a" (*lo));
}

/* Record the current value of the cycle counter. */
void start_counter()
{
    access_counter(&cyc_hi, &cyc_lo);
}

/* Return the number of cycles since the last call to start_counter. */
double get_counter()
{
    unsigned ncyc_hi, ncyc_lo;
    unsigned hi, lo, borrow;
    double result;

    /* Get cycle counter */
    access_counter(&ncyc_hi, &ncyc_lo);

    /* Do double precision subtraction */
    lo = ncyc_lo - cyc_lo;
    borrow = lo > ncyc_lo;
    hi = ncyc_hi - cyc_hi - borrow;
    result = (double) hi * (1 << 30) * 4 + lo;
    if (result < 0) {
	fprintf(stderr, "Error: counter returns neg value: %.0f\n", result);
    }
    return result;
}
/* $end x86cyclecounter */

#else
#error Timer code requires x86
#endif




/*******************************
 * Machine-independent functions
 ******************************/

/* $begin mhz */
/* Get the clock rate from /proc */
double mhz_full(int verbose, int sleeptime)
{
    (void) sleeptime;

    FILE *fp = fopen("/proc/cpuinfo", "r");
    // Hack to keep this working on labradoodle.
    double mhz = 2000.0;
    
    fclose(fp);
    if (verbose)
	printf("Processor clock rate ~= %.1f MHz\n", mhz);
    return mhz;

#if 0
    double rate;

    start_counter();
    sleep(sleeptime);
    rate = get_counter() / (1e6*sleeptime);
    if (verbose)
	printf("Processor clock rate ~= %.1f MHz\n", rate);
    return rate;
#endif
}
/* $end mhz */

/* Version using a default sleeptime */
double mhz(int verbose)
{
    return mhz_full(verbose, 2);
}

/** Special counters that compensate for timer interrupt overhead */

static double cyc_per_tick = 0.0;

#define NEVENT 100
#define THRESHOLD 1000
#define RECORDTHRESH 3000

/* Attempt to see how much time is used by timer interrupt */
static void callibrate(int verbose)
{
    double oldt;
    struct tms t;
    clock_t oldc;
    int e = 0;

    times(&t);
    oldc = t.tms_utime;
    start_counter();
    oldt = get_counter();
    while (e <NEVENT) {
	double newt = get_counter();

	if (newt-oldt >= THRESHOLD) {
	    clock_t newc;
	    times(&t);
	    newc = t.tms_utime;
	    if (newc > oldc) {
		double cpt = (newt-oldt)/(newc-oldc);
		if ((cyc_per_tick == 0.0 || cyc_per_tick > cpt) && cpt > RECORDTHRESH)
		    cyc_per_tick = cpt;
		/*
		  if (verbose)
		  printf("Saw event lasting %.0f cycles and %d ticks.  Ratio = %f\n",
		  newt-oldt, (int) (newc-oldc), cpt);
		*/
		e++;
		oldc = newc;
	    }
	    oldt = newt;
	}
    }
    if (verbose)
	printf("Setting cyc_per_tick to %f\n", cyc_per_tick);
}

static clock_t start_tick = 0;

void start_comp_counter()
{
    struct tms t;

    if (cyc_per_tick == 0.0)
	callibrate(0);
    times(&t);
    start_tick = t.tms_utime;
    start_counter();
}

double get_comp_counter()
{
    double time = get_counter();
    double ctime;
    struct tms t;
    clock_t ticks;

    times(&t);
    ticks = t.tms_utime - start_tick;
    ctime = time - ticks*cyc_per_tick;
    /*
      printf("Measured %.0f cycles.  Ticks = %d.  Corrected %.0f cycles\n",
      time, (int) ticks, ctime);
    */
    return ctime;
}

