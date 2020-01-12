# -*- coding: utf-8 -*-
"""
Created on Tue Aug  6 00:04:18 2019

@author: joell

"""

import logging
import serial
import time
import json
import threading
from threading import Thread
from Hardware_interface.Utilities import Singleton
import copy

@Singleton
class HMI(object):
    def __init__(self, port='', baud=115200):
        self.port = port
        self.baud = baud
        self.ser = None
        self.logger = logging.getLogger(__name__)
        self.logger.info('created HMI')
        self.functions = []
        self.exitCondition= False
        self.raw_input = ''
        self.results = {'DI':[0,0,0],'AI': [0,0,0],'DO': [0,0,0]}
        self.async_results = {'DI':[0,0,0],'AI': [0,0,0],'DO': [0,0,0]}
        self.history = []
        self.history_length = 5000
        self.messageQueue = []
        self.thread = threading.Thread(target=self.pollSerial)
        self.thread.start()
        
    def readSerial(self):
        self.raw_input = self.ser.read_until()
        self.logger.debug('raw input:{}'.format(self.raw_input))
        try:
            self.results = json.loads(self.raw_input)
            keys = self.results.keys()
            if 'ERROR' in self.results:
                self.logger.error('-->{}'.format(self.results))
            if 'DI' in keys:
                self.async_results['DI'] = self.results['DI']
            if 'DO' in keys:
                self.async_results['DO'] = self.results['DO']
            if 'AI' in keys:
                self.async_results['AI'] = self.results['AI']
            self.async_results['LastUpdate'] = time.time()
            self.history.append(copy.deepcopy(self.async_results))
            
            while len(self.history)>self.history_length:
                self.history.pop(0)
        except json.JSONDecodeError as e:
            self.logger.error('Non Valid Json Message: {}'.format(e))
        for func in self.functions:
            try:
                func(self.results, self.async_results)
            except Exception as e:
                self.logger.error('Function: "{}" did not except results'.format(func))
                self.logger.error('Error was logged as {}'.format(e))
    
    def sendSerial(self, sendingObject):
        if type(sendingObject) is dict:
            sendingObject = json.JSONEncoder().encode(sendingObject)
        elif type(sendingObject is str):
            pass
        else:
            self.logger.error('trying to send "{}" object, can only send strings and dicts'.format(type(sendingObject)))
            return
        
        self.ser.write(sendingObject.encode())
    
    def addCallback(self, func):
        try:
            func({'test':[0]},{'test':[0]})
            self.functions.append(func)
            return len(self.functions)-1
        except Exception as e:
            self.logger.error('function:{} , does not accept (dict, dict). Error: {}'.format(func,e))
            pass
        return -1
        
    def pollSerial(self):
        while not self.exitCondition:
            try:
                if self.ser.isOpen():
                    if(self.ser.in_waiting>0):
                        self.readSerial()
                    if( len(self.messageQueue)>0):
                        self.sendSerial(self.messageQueue.pop(0))
                    # this is used to yield to other threads
                    time.sleep(0.001)
                else:
                    # sleep while no connected to a USB
                    time.sleep(0.3)
            except Exception as e:
                time.sleep(0.2)
                if self.ser == None:
                    try:
                        self.ser = serial.Serial(self.port, self.baud)
                        self.logger.info('Connecting to port: {} at {} bsp'.format(self.port,self.baud))
                    except:
                        pass
                else:
                    self.logger.debug('Caught An error: {}'.format(e))
        if self.ser is not None:
            self.ser.close()
            self.logger.info("Closed the Serial Port {}".format(self.port))
        self.logger.info("shutting down polling thread")
        del self
        
    def stop(self):
        self.exitCondition = True
        
    def __str__(self):
        return repr(self) + "at com port" + repr(self.port) + repr(self.functions)
    
    def connect(self):
        try:
            self.ser = serial.Serial(port=self.port, baudrate=self.baud)
        except serial.SerialException as e:
            self.logger.error('Could not open serial port: {}'.format(e.strerror))
        return self.ser.is_open
    def __del__(self):
        if self.ser is not None:
            self.ser.close()
        
        
    
        
    
    
if __name__ == "__main__":
    logging.basicConfig(level=logging.INFO, format='%(asctime)s - %(name)s - %(levelname)s - %(message)s')
    hmi = HMI()
    hmi.port = 'COM5'
    #hmi = HMI(port='COM5', baud=115200)
    #hmi.stop()
    #hmi_test.stop()
    
    