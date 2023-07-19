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
//

#include <iostream>
#include <thread>
#include <mutex>
#include "cyan_semaphore.h"

constexpr auto BSIZE = 0x10;

char buffer[BSIZE];
int nextin = 0;
int nextout = 0;
cyan::counting_semaphore<BSIZE> occupied_semaphore(0);
cyan::counting_semaphore<BSIZE> empty_semaphore(BSIZE);
cyan::binary_semaphore resource_mutex(1);

void producer(char item) {
	empty_semaphore.acquire();
	resource_mutex.acquire();

	buffer[nextin] = item;
	nextin++;
	nextin %= BSIZE;
	printf("producer = %d\n", item);

	resource_mutex.release();
	occupied_semaphore.release();
}

char consumer() {
	char item = 0;

	occupied_semaphore.acquire();
	resource_mutex.acquire();

	item = buffer[nextout];
	nextout++;
	nextout %= BSIZE;
	printf("consumer = %d\n", item);

	resource_mutex.release();
	empty_semaphore.release();

	return(item);
}

void producer_thread() {
	for (;;) {
		const char item = rand() % 255 + 1;
		producer(item);
	}
}

void consumer_thread() {
	for (;;) {
		const char item = consumer();
	}
}

int main()
{
	srand((unsigned int)time(nullptr));
	std::thread t1(producer_thread);
	std::thread t2(consumer_thread);
	t1.join();
	t2.join();
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
