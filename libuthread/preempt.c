#include <signal.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include "private.h"
#include "uthread.h"

/*
 * Frequency of preemption
 * 100Hz is 100 times per second
 */
#define HZ 100


// previous signal action
static struct sigaction prev_sa;
// previous timer configuration
static struct itimerval prev_tc;

// indicate if preemption is enabled
bool preemption = false;


void signal_handler(int signal) 
{
	(void)signal;
	uthread_yield();
}

void preempt_disable(void)
{
	// check if preemption is enabled
	if (preemption == false) 
	{
		return;
	}

	sigset_t set;
	sigemptyset(&set);
	sigaddset(&set, SIGVTALRM);
	if (sigprocmask(SIG_BLOCK, &set, NULL) == -1)
	{
		perror("preempt_disable(): sigprocmask");
	}
}

void preempt_enable(void)
{
	// check if preemption is enabled
	if (preemption == false) 
	{
		return;
	}

	sigset_t set;
	sigemptyset(&set);
	sigaddset(&set, SIGVTALRM);
	if (sigprocmask(SIG_UNBLOCK, &set, NULL) == -1)
	{
		perror("preempt_enable(): sigprocmask");
	}
}


void preempt_start(bool preempt)
{
	preemption = preempt;

	// check if preemption is enabled
	if (preemption == false) 
	{
		return;
	}

	struct sigaction sa;
	struct itimerval tc;

	// ref: https://stackoverflow.com/questions/20684290/
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	sa.sa_handler = signal_handler;

	// save old signal action to restore later
	if (sigaction(SIGVTALRM, &sa, &prev_sa) == -1) 
	{
		perror("preempt_start(): sigaction");
	}


	// timer which will fire an alarm a hundred times per second
	// initial expiration
	tc.it_value.tv_sec = 0;
	tc.it_value.tv_usec = 1000000 / HZ;
	// following expiration period
	tc.it_interval.tv_sec = 0;
	tc.it_interval.tv_usec = 1000000 / HZ;

	// save old timer configuration to restore later
	if(setitimer(ITIMER_VIRTUAL, &tc, &prev_tc) == -1)
	{
		perror("preempt_start(): initialize preepmt timer");
		// if timer setup fails restore signal action 
		sigaction(SIGVTALRM, &prev_sa, NULL);
	}

}

void preempt_stop(void)
{	
	// check if preemption is enabled
	if (preemption == false) 
	{
		return;
	}
	
	// restore old signal action
	if (sigaction(SIGVTALRM, &prev_sa, NULL) == -1) 
	{
		perror("preempt_stop(): restore old signal action");
	}

	// restore old timer configuration
	if (setitimer(ITIMER_VIRTUAL, &prev_tc, NULL) == -1) 
	{
		perror("preempt_stop(): restore old timer configuration");
	}
}

