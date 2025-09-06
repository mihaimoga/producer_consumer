"""
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
"""

import threading
import time
from random import seed
from random import randint

# Shared Memory variables
CAPACITY = 0x10  # Size of the buffer (16 slots)
buffer = [-1 for i in range(CAPACITY)]  # Circular buffer for items
in_index = 0  # Index for the next produced item
out_index = 0  # Index for the next consumed item

# Declaring Semaphores
occupied_semaphore = threading.Semaphore(0)  # Counts occupied slots in buffer
empty_semaphore = threading.Semaphore(CAPACITY)  # Counts empty slots in buffer
resource_mutex = threading.Semaphore()  # Binary semaphore for mutual exclusion


def producer(item):
    """
    Producer function to add an item to the buffer.
    Waits for an empty slot, then acquires the mutex to insert the item.
    Releases the mutex and signals that a new item is available.
    """
    global CAPACITY, buffer, in_index, out_index
    global resource_mutex, empty_semaphore, occupied_semaphore
    empty_semaphore.acquire()  # Wait for at least one empty slot
    resource_mutex.acquire()  # Enter critical section
    buffer[in_index] = item  # Insert item into buffer
    in_index = (in_index + 1) % CAPACITY  # Move to next slot (circular)
    print("producer = ", item)
    resource_mutex.release()  # Exit critical section
    occupied_semaphore.release()  # Signal that a new item is available


def consumer():
    """
    Consumer function to remove an item from the buffer.
    Waits for an occupied slot, then acquires the mutex to remove the item.
    Releases the mutex and signals that a slot is now empty.
    Returns the consumed item.
    """
    global CAPACITY, buffer, in_index, out_index
    global resource_mutex, empty_semaphore, occupied_semaphore
    occupied_semaphore.acquire()  # Wait for at least one occupied slot
    resource_mutex.acquire()  # Enter critical section
    item = buffer[out_index]  # Remove item from buffer
    out_index = (out_index + 1) % CAPACITY  # Move to next slot (circular)
    print("consumer = ", item)
    resource_mutex.release()  # Exit critical section
    empty_semaphore.release()  # Signal that a slot is now empty
    return item


# Producer Thread Class
class ProducerThread(threading.Thread):
    """
    Thread class for producing items.
    Continuously generates random items and calls the producer function.
    """

    def run(self):
        while True:
            item = randint(0, 255)
            producer(item)


# Consumer Thread Class
class ConsumerThread(threading.Thread):
    """
    Thread class for consuming items.
    Continuously calls the consumer function to process items from the buffer.
    """

    def run(self):
        while True:
            item = consumer()


seed(1)  # Seed the random number generator for reproducibility

# Creating Threads
producer_thread = ProducerThread()
consumer_thread = ConsumerThread()

# Starting Threads
producer_thread.start()
consumer_thread.start()

# Waiting for threads to complete (commented out to allow infinite execution)
# producer_thread.join()
# consumer_thread.join()
