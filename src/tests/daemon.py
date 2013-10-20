#!/usr/bin/env python
# -*- coding: utf-8 -*-

import os
import sys
import time
import atexit

from signal import SIGTERM 


class Daemon:
    """
    Родительский класс дэмона
    Использование: создайте подкласс и переопределите метод run()
    """
    def __init__(self, pidfile, stdout='/dev/null', stderr='/dev/null', stdin='/dev/null'):
        self.pidfile = pidfile
        self.stdout = stdout
        self.stderr = stderr
        self.stdin = stdin

 
    """
    UNIX double-fork magic, смотрите подробности сдесь Stevens' "Advanced
    Programming in the UNIX Environment" (ISBN 0201563177)
    <a href="http://www.erlenstar.demon.co.uk/unix/faq_2.html#SEC16<br />
    " title="http://www.erlenstar.demon.co.uk/unix/faq_2.html#SEC16<br />
    ">http://www.erlenstar.demon.co.uk/unix/faq_2.html#SEC16<br />
    </a>
    """
    def daemonize(self):
        try:
            pid = os.fork()
            
            if pid > 0:
                sys.exit(0)
        
        except OSError, e:
            sys.stderr.write("fork #1 failed: %d (%s)\n" % (e.errno, e.strerror))
            sys.exit(1)
 
        os.chdir("/")
        os.setsid()
        os.umask(0) 
 
        # Make 2 fork
        try:
            pid = os.fork()
        
            if pid > 0:
                sys.exit(0)

        except OSError, e:
            sys.stderr.write("fork #2 failed: %d (%s)\n" % (e.errno, e.strerror))
            sys.exit(1) 
 
        # Rerouting stdin/stdout
        sys.stdout.flush()
        sys.stderr.flush()
        so = file(self.stdout, 'a+')
        se = file(self.stderr, 'a+', 0)
        si = file(self.stdin, 'r')
        os.dup2(so.fileno(), sys.stdout.fileno())
        os.dup2(se.fileno(), sys.stderr.fileno())
        os.dup2(si.fileno(), sys.stdin.fileno())
 
        # Sawe pidfile
        atexit.register(self.delpid)
        pid = str(os.getpid())
        file(self.pidfile,'w+').write("%s\n" % pid)

 
    def delpid(self):
        os.remove(self.pidfile)

 
    """
    Запуск дэмона
    """
    def start(self):
        # Check pidfile, for process loaded
        try:
            pf = file(self.pidfile,'r')
            pid = int(pf.read().strip())
            pf.close()
        except IOError:
            pid = None
 
        if pid:
            message = "pidfile %s already exist. Daemon already running?\n"
            sys.stderr.write(message % self.pidfile)
            sys.exit(1)
 
        # Start daemon
        self.daemonize()
        self.run()

 
    """
    Остановка дэмона
    """
    def stop(self):
        # Take pid from pidfile
        try:
            pf = file(self.pidfile,'r')
            pid = int(pf.read().strip())
            pf.close()
        except IOError:
            pid = None
 
        if not pid:
            message = "pidfile %s does not exist. Daemon not running?\n"
            sys.stderr.write(message % self.pidfile)
            return
         
        # Kill daemon process
        try:
            
            while 1:
                os.kill(pid, SIGTERM)
                time.sleep(0.1)
                
        except OSError, err:
            err = str(err)

            if err.find("No such process") > 0:
                if os.path.exists(self.pidfile):
                    os.remove(self.pidfile)

            else:
                print str(err)
                sys.exit(1)
 

    """
    Перезапуск дэмона
    """
    def restart(self):
        self.stop()
        self.start()
 

    """
    Необходимо переопределить данный метод в подклассе. Он должен быть вызван
    после вызова метода daemonize() в методе start().
    """
    def run(self):
        pass


class MyDaemon(Daemon):
    def run(self):
        sys.stdout.write('_ I start!')

        while True:
            time.sleep(1)
            print '_ W I work!'
            #sys.stdout.write('_ W I work!')
            
            sys.stdout.flush()
            sys.stderr.flush()

 
if __name__ == "__main__":
    daemon = MyDaemon('/tmp/daemon-example.pid', '/tmp/d-stdout', '/tmp/d-stderr')

    if len(sys.argv) == 2:
        if 'start' == sys.argv[1]:
            daemon.start()
        elif 'stop' == sys.argv[1]:
            daemon.stop()
        elif 'restart' == sys.argv[1]:
            daemon.restart()
        else:
            print "Unknown command"
            sys.exit(2)

        sys.exit(0)
    else:
        print "usage: %s start|stop|restart" % sys.argv[0]
        sys.exit(2)
