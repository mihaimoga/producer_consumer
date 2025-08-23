# Producer-Consumer Problem Implementation

This repository contains an implementation of the classic **Producer-Consumer problem**, using semaphores for synchronization. It demonstrates how multiple producers and consumers can safely share a bounded buffer without race conditions.

## Overview

The Producer-Consumer problem is a fundamental example of a multi-process synchronization scenario. This implementation uses:

- **Counting semaphores** to track the number of occupied and empty slots in the buffer.
- **Binary semaphore (mutex)** to ensure mutual exclusion when accessing the shared buffer.

Semaphore primitives are sourced from the [`mihaimoga/semaphore`](https://github.com/mihaimoga/semaphore)

- **C++** (primary implementation)
- **Python** (alternative implementation)

## Key Files

- `ProducerConsumer.cpp` – C++ implementation of the producer-consumer logic.
- `ProducerConsumer.py` – Python version of the same logic.
- `cyan_semaphore.h` – Header file for semaphore definitions.
- `ProducerConsumer.sln`, `.vcxproj`, `.vcxproj.filters` – Visual Studio project files.

## Example (C++)

```cpp
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
```
