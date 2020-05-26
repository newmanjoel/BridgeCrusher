
import serial
import serial.tools.list_ports as list_ports
import logging
import pickle
from threading import Semaphore
import time

import threading
def threaded(fn):
    def wrapper(*args, **kwargs):
        thread = threading.Thread(target=fn, args=args, kwargs=kwargs)
        thread.start()
        return thread
    return wrapper

class Microcontroller(object):
    ''' Basic class to allow for reading and writing to a uno '''
    def __enter__(self):
        ''' allow the user to use the 'with' command '''
        return self

    def __init__(self, uniqueVID = "" ):
        ''' setup the defaults and conect to the arduino '''
        self.logger = logging.getLogger('Microcontroller')
        self.threadLocker = Semaphore()
        self.messageQueue = []
        self.port = -1
        self.ports = list(list_ports.comports())
        self.lastMessage = ""
        for (localport, name, portVID) in self.ports:
            self.logger.debug("scanning ... name:{}, port:{}, other:{}".format(name, localport, portVID))
            #self.logger.debug(f"scanning ... name:{name}, port:{localport}, other:{portVID}")
            if "VID:" in portVID:
                if uniqueVID in portVID and uniqueVID != "":
                    self.port = localport
                    self.logger.info("Connected to: {} -> {}".format(name, localport))
            if "Uno" in name:
                self.port = localport
                self.logger.info("Connected to: {} -> {}".format(name, localport))
            
        self.arduino = None
        if (self.port != -1):
            self.arduino = serial.Serial(self.port, 57600, xonxoff = False, timeout = None)
        if ((self.arduino is None) and (uniqueVID != "")):
            self.logger.warning("ERROR: Could not connect to microcontroller")
    
    def queueWrite(self, whatToSend):
        ''' Adds a message to send to the microcontroller when it can '''
        self.messageQueue.append(whatToSend)
        
    @property
    def hasQueue(self):
        ''' if there are messages in the sending queue '''
        return len(self.messageQueue)>0
    
    def writeAll(self):
        ''' writes all of the messages in the queue'''
        while self.hasQueue:
            threadRef = self.write(self.messageQueue.pop(0))
            threadRef.join()
        #self.arduino.flushOutput()
    
    def writeOne(self):
         if self.hasQueue:
            threadRef = self.write(self.messageQueue.pop(0))

    @threaded
    def write(self, whatToSend):
        ''' send data to uc '''
        self.logger.debug("Sending: (TYPE) {} (PAYLOAD) {}".format(type(whatToSend), whatToSend))
        #while( not self.arduino.in_waiting):
        #    time.sleep(0.01)
        try:
            if(not(type(whatToSend) is bytes)):
                if(type(whatToSend) is str):
                    if(not(whatToSend.endswith('\n'))):
                        self.logger.debug("Adding a \\n to the sending command")
                        #whatToSend = whatToSend + '\n'
                binaryDataStream = pickle.dumps(whatToSend, protocol=3)
                #self.arduino.writelines(whatToSend.encode('utf-8'))
                self.arduino.write(binaryDataStream)
                self.arduino.flush()
                pass
            if(type(whatToSend) is bytes):
                #tempToSend = bytearray(whatToSend)
                #tempToSend.append('\n')
                self.arduino.write(whatToSend)
                self.arduino.flush()
                pass
        except TypeError as e:
            self.logger.exception(e)
    
    @threaded
    def threadRead(self):
        while(self.arduino.in_waiting <= 40):
            time.sleep(0.01)
        if (self.arduino.in_waiting>0):
            self.lastMessage = self.arduino.readline()
            
    
    def read(self):
        '''
        Read a line from the uc
        WARNING: THIS IS BLOCKING
        '''
        threadRef = self.threadRead()
        threadRef.join()
        #lineRead = ""
        #lineRead = self.arduino.readline()
        #self.arduino.reset_input_buffer()
        
        self.logger.debug('read the data {}:{}'.format(type(self.lastMessage), self.lastMessage))
        return self.lastMessage

    def __exit__(self, exc_type, exc_value, traceback):
        ''' make sure that the python gc can close the connection '''
        if(self.arduino is not None):
            self.arduino.close()
            self.logger.info("closed connection to port: {}".format(self.port))
    
    def close(self):
        if(self.arduino is not None):
            self.arduino.close()
            self.logger.info("closed connection to port: {}".format(self.port))
        

if (__name__=="__main__"):
    try:
        logging.basicConfig(level=logging.DEBUG)
        controlBoard = Microcontroller(uniqueVID="239A:800F")
        import time
        print("Received")
        
        start0 = time.time()
        controlBoard.read()
        start1 = time.time()
        controlBoard.write('{"Config":{"ENCODER":true}}'.encode('utf-8'))
        start2 = time.time()
        controlBoard.read()
        start3 = time.time()        
        controlBoard.read()
        start4 = time.time()        
        controlBoard.read()
        start5 = time.time()
        controlBoard.read()
        start6 = time.time()
        
        print('1 - readTime: {}'.format(start1-start0))
        print('writeTime: {}'.format(start2-start1))
        print("2 - readTime: {}".format(start3-start2))
        print("3 - readTime: {}".format(start4-start3))
        print("4 - readTime: {}".format(start5-start4))
        print("5 - readTime: {}".format(start6-start5))
        
        
    finally:
        controlBoard.close()
    
