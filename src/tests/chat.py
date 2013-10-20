#!/usr/bin/env python
# author: Velichko Rostislav, @: 20.02.13

# example test: telnet localhost 12345 

import os
import sys
import time
import atexit
import pymongo

from signal import SIGTERM 
from twisted.internet import reactor
from twisted.internet.protocol import ServerFactory 
from twisted.protocols.basic import LineOnlyReceiver 


class Daemon:
    def __init__(self, pidfile, stdout='/dev/null', stderr='/dev/null', stdin='/dev/null'):
        self.pidfile = pidfile
        self.stdout = stdout
        self.stderr = stderr
        self.stdin = stdin

 
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

        print "Service is starting." 
        sys.stdout.flush()
        sys.stderr.flush()
 
        # Start daemon
        self.daemonize()
        self.run()
 
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
            print "Terminate service." 
            sys.stdout.flush()
            sys.stderr.flush()

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
 

    def restart(self):
        self.stop()
        self.start()
 

    def run(self):
        pass


class ChatProtocol(LineOnlyReceiver): 

    name = ""
    line = ""

    def getName(self): 
        if self.name != "": 
            return self.name 
        return self.transport.getPeer().host 

    def connectionMade(self): 
        self.flushPrint("New connection from " + self.getName())
        
        self.sendLine("Welcome to chat server.") 
        self.sendLine("Send '/NAME [new name]' to change your name.") 
        self.sendLine("Send '/EXIT' to quit.") 

        self.factory.sendMessageToAllClients(self.getName() + " has joined the party.") 
        self.factory.clientProtocols.append(self)

    def connectionLost(self, reason): 
        self.flushPrint("Lost connection from " + self.getName())

        self.factory.clientProtocols.remove(self) 
        self.factory.sendMessageToAllClients(self.getName() + " has disconnected.") 

    def lineReceived(self, line): 
        self.flushPrint(self.getName() + " said " + line)
        
        if line[:5] == "/NAME": 
            oldName = self.getName() 
            self.name = line[5:].strip()
            self.factory.sendMessageToAllClients(oldName + " changed name to " + self.getName()) 
        elif line == "/EXIT": 
            self.transport.loseConnection() 
        else: 
            self.factory.sendMessageToAllClients(self.getName() + " says " + line) 
            self.line = line

    def sendLine(self, line): 
        self.transport.write(line+"\r\n") 

    
    def flushPrint(self, msg):
        print msg
        sys.stdout.flush()
        sys.stderr.flush()
        

class ChatProtocolFactory(ServerFactory): 

    protocol = ChatProtocol

    def __init__(self):
        self.clientProtocols = [] 
        
        try:
            self.connection = pymongo.Connection('localhost', 27017)
            self.connection.drop_database('chat-db')
            self.database = self.connection['chat-db']
            self.collection = self.database['chat-coll']
        except:
            pass

    def sendMessageToAllClients(self, mesg): 
        for client in self.clientProtocols:
            try:
                self.collection.update({'name' : client.name}, {'$push' : {'says' : client.line}})
            except:
                pass
            client.sendLine(mesg) 


class CharService(Daemon):
    def run(self):
        print "Starting Server" 
        sys.stdout.flush()
        sys.stderr.flush()

        factory = ChatProtocolFactory()
        reactor.listenTCP(12345, factory)
        reactor.run()


if __name__ == "__main__":
    service = CharService('/tmp/char-service.pid', '/tmp/char-service-stdout', '/tmp/char-service-stderr')

    if len(sys.argv) == 2:
        if 'start' == sys.argv[1]:
            service.start()
        elif 'stop' == sys.argv[1]:
            service.stop()
        elif 'restart' == sys.argv[1]:
            service.restart()
        else:
            print "Unknown command"
            sys.exit(2)

        sys.exit(0)
    else:
        print "usage: %s start|stop|restart" % sys.argv[0]
        sys.exit(2)
