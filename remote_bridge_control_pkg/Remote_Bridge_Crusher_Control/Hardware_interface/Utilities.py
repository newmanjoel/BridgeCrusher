# -*- coding: utf-8 -*-
"""
Created on Fri Aug  9 13:26:41 2019

@author: joell
"""

class Singleton(object):
    _instance = None
    def __init__(self, decorated):
        self._decorated = decorated;
        Singleton._instance = self._decorated()
        
    def __call__(self):
        if Singleton._instance is not None:
            return Singleton._instance
        else:
            Singleton._instance = self._decorated()
            return Singleton._instance

    @classmethod
    def clear(cls):
        print('trying to clear')
        if Singleton._instance is not None:
            print('trying to set to none')
            Singleton._instance = None
        else:
            print('passing')
            pass