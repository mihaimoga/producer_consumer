# producer_consumer
This is an implementation of producer-consumer problem as described in [Wikipedia's Producer–consumer problem](https://en.wikipedia.org/wiki/Producer%E2%80%93consumer_problem), but using the semaphore implementation from https://github.com/mihaimoga/semaphore.
## example
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
