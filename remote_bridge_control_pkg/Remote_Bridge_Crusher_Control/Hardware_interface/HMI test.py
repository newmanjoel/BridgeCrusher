# -*- coding: utf-8 -*-
"""
Created on Fri Aug  9 12:50:00 2019

@author: joell
"""

from HMIBox import HMI
import logging


def localFunction(inputDict, totalDict):
    if 'DI' in inputDict.keys():
        localArr = inputDict['DI']
        hmi.messageQueue.append({'DO0':localArr[0]*255})
        hmi.messageQueue.append({'DO1':localArr[1]*255})
        hmi.messageQueue.append({'DO2':localArr[2]*255})

logging.basicConfig(level=logging.INFO, format='%(asctime)s - %(name)s - %(levelname)s - %(message)s')

hmi = HMI()
hmi.addCallback(localFunction)
hmi.port = 'COM7'

