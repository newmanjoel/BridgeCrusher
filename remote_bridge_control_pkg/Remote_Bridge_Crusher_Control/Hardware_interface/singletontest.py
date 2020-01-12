# -*- coding: utf-8 -*-
"""
Created on Fri Aug  9 13:33:07 2019

@author: joell
"""
from Utilities import Singleton

@Singleton
class Foo(object):
    def __init__(self):
        print('foo created')

f = Foo()
g = Foo()

