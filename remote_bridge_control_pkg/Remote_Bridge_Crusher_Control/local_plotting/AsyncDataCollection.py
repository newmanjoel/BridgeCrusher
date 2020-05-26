# -*- coding: utf-8 -*-
"""
Created on Mon May 18 15:54:01 2020

the threaded wrapper came from here
https://stackoverflow.com/questions/19846332/python-threading-inside-a-class

@author: joell
"""

import socket
import pickle
import time
import logging

import threading
def threaded(fn):
    def wrapper(*args, **kwargs):
        thread = threading.Thread(target=fn, args=args, kwargs=kwargs)
        thread.start()
        return thread
    return wrapper

class Async(object):
    ''' Basic class to allow for async reading from a UDP stream'''
    def __enter__(self):
        ''' allow the user to use the 'with' command '''
        return self

    def __init__(self ):
        self.logger = logging.getLogger("AsyncMain")
        self.data = []
        self.maxDataSize = 200
        self.port = 9999
        self.host = '0.0.0.0'
        self.server = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.exitCondition = False
        self.threadReference = ""
        self.saveDict = ['FORCE','KG']
        self.saveType = float
        self.lastConfigMessage = None
        
    
    def begin(self):
        self.server.bind((self.host, self.port))
        self.threadReference = self.asyncCollection()
    
    def dataReset(self):
        self.data = []
    
    def findSearchItem(self, refinedSearch):
        for searchTerm in self.saveDict:
            if(searchTerm in refinedSearch):
                refinedSearch = refinedSearch[searchTerm]
            else:
                self.logger.debug("could not find {} in {}".format(searchTerm, refinedSearch))
        if(type(refinedSearch) == self.saveType):
            self.data.append(refinedSearch)
        else:
            self.logger.error("identified Data not found in {}".format(refinedSearch))
        while(len(self.data) > self.maxDataSize):
            self.data.pop(0)
    
    def findConfigItem(self, inputDict):
        if("Config" in inputDict):
            self.lastConfigMessage = inputDict['Config']
    
    @threaded
    def asyncCollection(self):
        localLogger = logging.getLogger("AsyncWorker")
        localLogger.info("Async Worker Started")
        while not self.exitCondition:
            try:
                (rawData, fromAddress) = self.server.recvfrom(4096)
                pdata = pickle.loads(rawData)
                self.findSearchItem(pdata)
                self.findConfigItem(pdata)
                    
            except pickle.UnpicklingError as e:
                print("Pickle Error")
                #localLogger.exception(e)
                
        localLogger.info("Async Worker Stopped")
    
    def stop(self):
        self.exitCondition = True
        self.logger.info("Sent Stop Signal")
        self.threadReference.join()
        self.logger.info("All threads stopped")
    
    def __del__(self):
        self.stop()

if (__name__=="__main__"):
    logging.basicConfig(level=logging.DEBUG, format='%(asctime)s - %(name)s - %(levelname)s - %(message)s')#, filename='sampleData.log')
    test = Async()
    test.begin()
    time.sleep(3)
    test.stop()
    
    
print("ALL DONE")
