// CSE 375/475 Assignment #1
// Fall 2017
//
// Description: This file implements a function 'run_custom_tests' that should be able to use
// the configuration information to drive tests that evaluate the correctness
// and performance of the map_t object.

#include <iostream>
#include <ctime>
#include "config_t.h"
#include "tests.h"
#include <stdlib.h>
#include <thread>
#include <time.h>
#include <mutex>
#include <sys/time.h>

#include "simplemap.h"

std::mutex* locks;
simplemap_t<int, float> *map;
int iters;
int key_max;
int *durations;

void deposit(int seed){
	int account1 = (int) (rand() % (key_max + 1));
	int account2 = (int) (rand() % (key_max + 1));
	
	float left = 0;

    std::lock_guard<std::mutex> lock(locks[0]);
	while((left = map->lookup(account1).first) <= 0)account1 = (int) (rand() % (key_max + 1));

	float fund = ((float) rand() / (float) (RAND_MAX)) * left;
	fund = (float)((int) (fund*100)) / (float)(100);
	map->update(account1, left - fund);
	map->update(account2, map->lookup(account2).first + fund);
	//std::cout << "thread " << seed << " deposite " << fund << " from " << account1 << " to " << account2 << std::endl;
}

float balance(int seed){
    std::lock_guard<std::mutex> lock(locks[0]);
	float sum = 0;
	for(int i  = 0; i <= key_max; ++i){
		sum += map->lookup(i).first;
	}
	//std::cout << "balance: " << sum << std::endl;
	return sum;
}

void do_work(int seed){
    struct timeval start;
    gettimeofday(&start, NULL);
	for(int i = 0; i < iters; ++i){
		if(rand() % 100 < 80)deposit(seed);
		else balance(seed);
	}
    struct timeval end;
    gettimeofday(&end, NULL);
    durations[seed] = (end.tv_sec - start.tv_sec) * 1000000 + ((int)end.tv_usec - (int)start.tv_usec);
}

void printer(int k, float v) {
	std::cout<<"<"<<k<<","<<(float)((int)(v * 100)) / 100<<">"<< std::endl;
}

void run_custom_tests(config_t& cfg) {
	// Step 1
	// Define a seed for each executing thread
	// << use a thread local variable >>

	// Step 2
	// Define a simplemap_t of types <int,float>
	// this map represents a collection of bank accounts:
	// each account has a unique ID of type int;
	// each account has an amount of fund of type float.
	map = new simplemap_t<int, float>();
    locks = new std::mutex[cfg.key_max + 1];
    iters = cfg.iters;
    key_max = cfg.key_max;
    durations = new int[cfg.threads];  
    srand (time(NULL));

	// Step 3
	// Populate the entire map with the 'insert' function
	// Initialize the map in a way the sum of the amounts of
	// all the accounts in the map is 1000
	float init_fund = int(1000 / (cfg.key_max + 1));
	for(int i  = 0; i < cfg.key_max; i++){
		map->insert(i, init_fund);
	}
	map->insert(cfg.key_max, 1000 - cfg.key_max * init_fund);

	// Step 4
	// Define a function "deposit" that selects two random bank accounts
	// and an amount. This amount is subtracted from the amount
	// of the first account and summed to the amount of the second
	// account. In practice, give two accounts B1 and B2, and a value V,
	// the function performs B1-=V and B2+=V.
	// The execution of the whole function should happen atomically:
	// no operation should happen on B1 and B2 (or on the whole map?)
	// while the function is executing.

	// Step 5
	// Define a function "balance" that sums the amount of all the
	// bank accounts in the map. In order to have a consistent result,
	// the execution of this function should happen atomically:
	// no other deposit operations or balance operations should interleave.

	// Step 6
	// Define a function 'do_work', which has a for-loop that
	// iterates for config_t.iters times. Each iteration, the function should
	// call the function 'deposit' with 80% of the probability;
	// otherwise (the rest 20%) the function 'balance' should be called.
	// The function 'do_work' should measure 'exec_time_i', which is the
	// time needed to perform the entire for-loop. This time will be shared with
	// the main thread once the thread executing the 'do_work' joins its execution
	// with the main thread.

	// Step 8
	// The evaluation should be performed in the following way:
	// - the main thread creates #threads threads (as defined in config_t)
	//   << use std:threds >>
	// - each thread executes the function 'do_work' until completion
	// - the (main) spawning thread waits for all the threads to be executed
	//   << use std::thread::join() >>
	//	 and collect all the 'exec_time_i' from each joining thread
	// - once all the threads have joined, the function "balance" must be called
   
    std::thread* threads[cfg.threads];  
	for(int i = 0; i < cfg.threads; ++i){
        threads[i] = new std::thread(do_work, i);
	}

    for(int i = 0; i < cfg.threads; ++i){
        threads[i]->join();
    }

	// WHAT IS THE OUTPUT OF this call of "balance"?
	// DOES IT MATCH WHAT YOU EXPECT?
	// WHAT DO YOU EXPECT?
	// WHAT ARE THE OUTCOMES OF ALL THE "balance" CALLS DURING THE EXECUTION?
	// IS THAT WHAT YOU EXPECT?
    std::cout << "final balance: " << balance(-1) << std::endl;

    for(int i = 0; i < cfg.threads; ++i){
        //std::cout << "thread " << i << " run for " << durations[i] << std::endl;
    }
	// Step 9
	// Now configure your application to perform the same total amount
	// of iterations just executed, but all done by a single thread.
	// Measure the time to perform them and compare with the time
	// previously collected.
	// Which conclusion can you draw?
	// Which optimization can you do to the single-threaded execution in
	// order to improve its performance?

	// Step 10
	// Remove all the items in the map by leveraging the 'remove' function of the map
	// Destroy all the allocated resources (if any)
	// Execution terminates.
	// If you reach this stage happy, then you did a good job!


	// << ADVISE FOR USING LOCK(s) >>
	// For locks, use the type std::mutex
	// Suggestion: do not use the std::mutex directly,
	// rely on std::lock_guard<std::mutex> lock(name_of_lock)
	// unlock is not needed because the lock is released when
	// the function method terminates.

	// You might need the following function to print the entire map.
	// Attention if you use it while multiple threads are operating
	map->apply(printer);

}

void test_driver(config_t &cfg) {
	run_custom_tests(cfg);
}
