# -*- coding: utf-8 -*-
"""
Created on Sun May 17 18:32:59 2020

@author: joell
"""

import socket
import time
import json
import logging
import pickle
from microcontrollerClass import Microcontroller
from multiprocessing import Process
from threading import Thread, Lock

def threaded(fn):
    def wrapper(*args, **kwargs):
        thread = Thread(target=fn, args=args, kwargs=kwargs)
        thread.start()
        return thread
    return wrapper

delayTime = 1
exitCondition = False
controlBoard = Microcontroller(uniqueVID="239A:800F")

def startListeningAndBroadcasting(host='127.0.0.1', sendingPort = 9999, receivingPort = 9998):
    global exitCondition
    logger = logging.getLogger("MasterController")
    logger.debug("Creating all of the daemons")
    
    broadcasting = Thread(target=startBroadcasting, args=(host,sendingPort), daemon=True, name="Broadcaster")
    listening = Thread(target=startListening, args=(host, receivingPort), daemon=True, name="Listener")

    threads = [listening, broadcasting]
    for thread in threads:
        logger.debug("starting: {} Process".format(thread.name))
        thread.start()
        logger.debug("started: {} Process".format(thread.name))
    logger.debug("All threads running")
    
    try:
        while(True):
            time.sleep(1)
    except KeyboardInterrupt:
        logging.debug("Telling all threads to Stop")
        exitCondition = True
    finally:
        exitCondition = True
        for thread in threads:
            logging.info("Waiting on {} thread to end".format(thread.name))
            thread.join()
        logging.info("ALL THREADS HAVE EXITED")
        controlBoard.close()

def startListening(host = '127.0.0.1', receivingPort = 9998):
    global exitCondition, controlBoard
    logger = logging.getLogger('Listening')
    serv = socket.socket(socket.AF_INET, type=socket.SOCK_DGRAM)
    #serv = socket.socket(socket.AF_INET, type=socket.SOCK_STREAM)
    logger.info("listening on {}:{}".format(host, receivingPort))
    serv.bind((host, receivingPort))
    serv.settimeout(0.5)
    while not exitCondition:
        try:
            # TODO: Create local boolean to test if we have the lock to adjust for error handling
            (rawData, fromAddress) = serv.recvfrom(4096)
            data = pickle.loads(rawData)
            jData = json.dumps(data)
            controlBoard.queueWrite(jData.encode('utf-8'))
 
        except pickle.PickleError as e:
            logger.exception(e.with_traceback)
        except socket.timeout as e:
            pass

    logger.info("Listening has ended")

def startBroadcasting(host = '127.0.0.1', sendingPort=9999):
    global exitCondition,controlBoard
    logger = logging.getLogger('Broadcasting')
    logger.debug("starting the broadcasting loop")
    errorCode = 0
    
    
    while not exitCondition:
        sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        logger.info("broadcasting to {}:{}".format(host,sendingPort))
    
        try:
            while not exitCondition:
                try:
                    if(controlBoard.arduino.inWaiting()):
                        rawDataIn = controlBoard.read()
                        stringDataIn = rawDataIn.decode('utf-8')
                        jsonData = json.loads(stringDataIn)
                        binaryStreamOut= pickle.dumps(jsonData)
                        sock.sendto(binaryStreamOut,(host, sendingPort))
                        
                    if(controlBoard.hasQueue):
                        controlBoard.writeOne()
                        
                        
                except json.JSONDecodeError as e:
                    logger.error("JError: {}, rawDataIn: {}".format(e, stringDataIn))
                except UnicodeDecodeError as e:
                    #logger.error("DError: {}, rawDataIn: {}".format(e, rawDataIn))
                    logger.exception(e)
        except ConnectionAbortedError as e:
            logger.info(e)
        except ConnectionResetError as e:
            logger.info(e)
        except Exception as e:
            logger.exception(e)
        finally:
            #sock.close()
            logger.info("Broadcasting Has stopped")
    
    logger.info("Broadcasting has ended")
    return errorCode

if (__name__=="__main__"):
    logging.basicConfig(level=logging.INFO, format='%(asctime)s - %(name)s - %(levelname)s - %(message)s') #, filename='testing.log')
    startListeningAndBroadcasting()
    print("ALL DONE")
        