/*
MIT License

Copyright (c) 2022 Stefan-Mihai MOGA

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

// ProducerConsumer.cpp : This file contains the 'main' function. Program execution begins and ends there.
// Implementation of the classic Producer-Consumer problem using semaphores and threads in C++.

#include <iostream>
#include <thread>
#include <mutex>
#include "cyan_semaphore.h"

// Buffer size (number of slots in the circular buffer)
constexpr auto BSIZE = 0x10;

// Shared buffer and indices for producer/consumer
char buffer[BSIZE];      // Circular buffer for items
int nextin = 0;          // Index for next produced item
int nextout = 0;         // Index for next consumed item

// Semaphores for synchronization
cyan::counting_semaphore<BSIZE> occupied_semaphore(0); // Counts occupied slots
cyan::counting_semaphore<BSIZE> empty_semaphore(BSIZE); // Counts empty slots
cyan::binary_semaphore resource_mutex(1); // Binary semaphore for mutual exclusion

/**
 * @brief Producer function. Inserts an item into the buffer.
 *        Waits for an empty slot, then acquires the mutex to insert the item.
 *        Releases the mutex and signals that a new item is available.
 * @param item The item to be produced and inserted into the buffer.
 */
void producer(char item) {
	empty_semaphore.acquire();      // Wait for at least one empty slot
	resource_mutex.acquire();       // Enter critical section

	buffer[nextin] = item;          // Insert item into buffer
	nextin++;
	nextin %= BSIZE;                // Wrap around (circular buffer)
	printf("producer = %d\n", item);

	resource_mutex.release();       // Exit critical section
	occupied_semaphore.release();   // Signal that a new item is available
}

/**
 * @brief Consumer function. Removes an item from the buffer.
 *        Waits for an occupied slot, then acquires the mutex to remove the item.
 *        Releases the mutex and signals that a slot is now empty.
 * @return The consumed item.
 */
char consumer() {
	char item = 0;

	occupied_semaphore.acquire();   // Wait for at least one occupied slot
	resource_mutex.acquire();       // Enter critical section

	item = buffer[nextout];         // Remove item from buffer
	nextout++;
	nextout %= BSIZE;               // Wrap around (circular buffer)
	printf("consumer = %d\n", item);

	resource_mutex.release();       // Exit critical section
	empty_semaphore.release();      // Signal that a slot is now empty

	return(item);
}

/**
 * @brief Producer thread function. Continuously produces random items.
 */
void producer_thread() {
	for (;;) {
		const char item = rand() % 255 + 1; // Generate random item
		producer(item);
	}
}

/**
 * @brief Consumer thread function. Continuously consumes items from the buffer.
 */
void consumer_thread() {
	for (;;) {
		const char item = consumer();
	}
}

/**
 * @brief Program entry point. Initializes random seed, starts producer and consumer threads, and waits for them to finish.
 */
int main()
{
	srand((unsigned int)time(nullptr)); // Seed random number generator
	std::thread t1(producer_thread);    // Start producer thread
	std::thread t2(consumer_thread);    // Start consumer thread
	t1.join();                          // Wait for producer thread to finish (never returns)
	t2.join();                          // Wait for consumer thread to finish (never returns)
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
