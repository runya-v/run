#!/usr/bin/env python
# author: Velichko Rostislav, @: 20.02.13

import sys
import Queue
import threading
import random
import time


class Help:
    
    def __init__(self):
        msg = "Use: %s <num threads> <max message id>\n"
        print msg % sys.argv[0]


class MessageGenerator(threading.Thread):

    def __init__(self, lock, msg_queue_dict, max_msg_id):
        super(MessageGenerator, self).__init__()

        self.stop_process = False
        self.lock = lock
        self.msg_queue_dict = msg_queue_dict
        self.max_msg_id = max_msg_id

    def stop(self):
        with self.lock:
            self.stop_process = True

    def run(self):
        try:
            while not self.stop_process:
                sequance = random.randint(1, self.max_msg_id)
                
                while sequance != 0:
                    sequance = sequance-1
                    msg_id = random.randint(0, self.max_msg_id)
                    
                    with self.lock:
                        list_len = len(self.msg_queue_dict)
                        queue = Queue.Queue()
                        
                        if msg_id in self.msg_queue_dict:
                            queue = self.msg_queue_dict[msg_id]
                        else:
                            self.msg_queue_dict[msg_id] = queue
                            
                        queue.put(random.random())
        finally:
            pass


class Worker(threading.Thread):

    def __init__(self, lock, msg_queue_dict):
        super(Worker, self).__init__()

        self.stop_process = False
        self.lock = lock
        self.msg_queue_dict = msg_queue_dict

    def stop(self):
        with self.lock:
            self.stop_process = True

    def run(self):
        try:
            while not self.stop_process:
                with self.lock:
                    if len(self.msg_queue_dict):
                        msg_id = self.msg_queue_dict.keys()[0]
                        queue = self.msg_queue_dict[msg_id]
                        
                        print '>'
                        
                        while not queue.empty():
                            print 'msg: '+ str(msg_id) + " content: " + str(queue.get())
                            
                        self.msg_queue_dict.pop(msg_id)
        finally:
            pass


class Model:
    
    def __init__(self, threads_num, max_msg_id=10):
        lock = threading.Lock()
        msg_queue_dict = {}

        msg_gen = MessageGenerator(lock, msg_queue_dict, max_msg_id)
        msg_gen.start()
        
        workers = []

        for i in range(threads_num):
            wkr = Worker(lock, msg_queue_dict)
            workers.append(wkr)
            wkr.start()
            
        time.sleep(20)

        msg_gen.stop()

        for i in range(len(workers)):
            workers[i].stop()


def main(argv=sys.argv):

    if len(argv) <= 1:
        Help()
        return 1

    if len(argv) == 3:
        Model(int(argv[1]), int(argv[2]))
    else:
        Model(int(argv[1]))

    return 0    


if __name__ == "__main__":
    sys.exit(main())
