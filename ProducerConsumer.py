import threading
import time
from random import seed
from random import randint

# Shared Memory variables
CAPACITY = 0x10
buffer = [-1 for i in range(CAPACITY)]
in_index = 0
out_index = 0

# Declaring Semaphores
occupied_semaphore = threading.Semaphore(0)
empty_semaphore = threading.Semaphore(CAPACITY)
resource_mutex = threading.Semaphore()


def producer(item):
    global CAPACITY, buffer, in_index, out_index
    global resource_mutex, empty_semaphore, occupied_semaphore
    empty_semaphore.acquire()
    resource_mutex.acquire()
    buffer[in_index] = item
    in_index = (in_index + 1) % CAPACITY
    print("producer = ", item)
    resource_mutex.release()
    occupied_semaphore.release()


def consumer():
    global CAPACITY, buffer, in_index, out_index
    global resource_mutex, empty_semaphore, occupied_semaphore
    occupied_semaphore.acquire()
    resource_mutex.acquire()
    item = buffer[out_index]
    out_index = (out_index + 1) % CAPACITY
    print("consumer = ", item)
    resource_mutex.release()
    empty_semaphore.release()
    return item


# Producer Thread Class
class ProducerThread(threading.Thread):
    def run(self):
        while True:
            item = randint(0, 255)
            producer(item)


# Consumer Thread Class
class ConsumerThread(threading.Thread):
    def run(self):
        while True:
            item = consumer()


seed(1)
# Creating Threads
producer_thread = ProducerThread()
consumer_thread = ConsumerThread()
# Starting Threads
producer_thread.start()
consumer_thread.start()
# Waiting for threads to complete
# producer_thread.join()
# consumer_thread.join()
