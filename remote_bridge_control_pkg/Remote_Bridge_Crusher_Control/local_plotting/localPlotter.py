# -*- coding: utf-8 -*-
"""
Created on Sun May 17 20:14:20 2020

@author: joell
"""

# -*- coding: utf-8 -*-
"""
Demonstrates use of PlotWidget class. This is little more than a 
GraphicsView with a PlotItem placed in its center.
"""


#from pyqtgraph.Qt import QtGui, QtCore
import pyqtgraph.Qt as qt
import numpy as np
import pyqtgraph as pg
import time
from pyqtgraph.dockarea import Dock, DockArea

import pyqtgraph.parametertree.parameterTypes as pTypes
from pyqtgraph.parametertree import Parameter, ParameterTree, ParameterItem, registerParameterType


from AsyncDataCollection import Async

import json
import pickle
import socket
import timeit

host = '127.0.0.1'
sendingPort = 9998

class MainWindow(qt.QtGui.QMainWindow):
    def __init__(self, *args, **kwargs):
        super(MainWindow, self).__init__(*args, **kwargs)
    
        self.counter = 0
        self.setWindowTitle('pyqtgraph example: Plot Widget')
        self.resize(800, 800)
        cw = qt.QtGui.QWidget()
        self.setCentralWidget(cw)
        self.layout = qt.QtGui.QVBoxLayout()
        cw.setLayout(self.layout)
        area = DockArea()
        d1 = Dock('Live Force', size=(1, 1))
        d2 = Dock('History')
        d3 = Dock('Controls', size=(2,2))
        area.addDock(d1, 'top')
        area.addDock(d2, 'bottom')
        area.addDock(d3, 'left')
        controlLayout = pg.LayoutWidget()
        startButton = qt.QtGui.QPushButton('Start Button')
        stopButton = qt.QtGui.QPushButton('Stop Button')
        resetData = qt.QtGui.QPushButton('Reset Data')
        controlLayout.addWidget(startButton, row=0, col = 0)
        controlLayout.addWidget(stopButton, row=1, col = 0)
        controlLayout.addWidget(resetData, row =2, col = 0)
        d3.addWidget(controlLayout)
        
        ## Parameter Tree Stuff!
        self.parameterTree = ParameterTree()
        
        t.setParameters(p, showTop=False)
        t.setWindowTitle('pyqtgraph example: Parameter Tree')
        
        
        
        self.layout.addWidget(area)
        self.pw = pg.PlotWidget(name='Plot1')
        d1.addWidget(self.pw)
        #self.layout.addWidget(self.pw)
        #self.pw2 = pg.PlotWidget(name='Plot2')
        
        self.pw3 = pg.PlotWidget()
        #self.layout.addWidget(self.pw3)
        d2.addWidget(self.pw3)
        self.dataCollection = Async()
        self.updateTimer = qt.QtCore.QTimer()
        self.updateTimer.timeout.connect(self.updateData)
        
        # hookup all of the buttons
        startButton.clicked.connect(self.startFunc)
        stopButton.clicked.connect(self.stopFunc)
        resetData.clicked.connect(self.dataCollection.dataReset)
        

    def begin(self):
        self.dataCollection.begin()
        self.updateTimer.start(50)
        self.p1 = self.pw.plot()
        self.p1.setPen((200, 200, 100))
        self.pw.setLabel('left', 'Value', units='V')
        self.pw.setLabel('bottom', 'Time', units='s')
        #self.pw.setXRange(0, 2)
        #self.pw.setYRange(0, 1e-10)

        curve = self.pw3.plot(np.random.normal(size=100)*1e0, clickable=True)
        curve.curve.setClickable(True)
        curve.setPen('w')
        curve.setShadowPen(pg.mkPen((70, 70, 30), width=6, cosmetic=True))

        lr = pg.LinearRegionItem([1, 30], bounds=[0, 100], movable=True)
        self.pw3.addItem(lr)
        line = pg.InfiniteLine(angle=90, movable=True)
        self.pw3.addItem(line)
        line.setBounds([0, 200])

        curve.sigClicked.connect(self.clicked)

    def updateData(self):
        self.ydata = self.dataCollection.data
        self.xdata = list(range(0, len(self.ydata)))
        self.p1.setData(y=self.ydata, x=self.xdata)
        #print("updated")
    
    def startFunc(self):
        print("Start")
        try:
            sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
            jData = json.loads('{"Command":{"Test":true}}')
            bytesToSend= pickle.dumps(jData)
            sock.sendto(bytesToSend, (host, sendingPort))
            print("Sent -> {}".format(jData))
        except pickle.UnpicklingError as e:
            print("caught pickling error")
        except pickle.PickleError as e:
            print("caught generic pickle error")
        except json.JSONDecodeError as e:
            print("Faild to decode json")
        
    
    def stopFunc(self):
        print("Stop")
        try:
            sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
            jData = json.loads('{"Command":{"Test":false}}')
            bytesToSend= pickle.dumps(jData)            
            sock.sendto(bytesToSend, (host, sendingPort))
            print("Sent -> {}".format(jData))
            
        except pickle.UnpicklingError as e:
            print("caught pickling error")
        except pickle.PickleError as e:
            print("caught generic pickle error")
        except json.JSONDecodeError as e:
            print("Faild to decode json")
        pass

    def clicked(self):
        #print("curve clicked")
        pass

    def closeEvent(self, event):
        self.dataCollection.stop()
        self.updateTimer.stop()
        event.accept()
        app.quit()



#TODO: Add a parameter tree for changing data on the fly
#TODO: add stop/start data buttons
#TODO: add docks for all items with savestates





import logging
if __name__ == '__main__':
    logging.basicConfig(level=logging.DEBUG, format='%(asctime)s - %(name)s - %(levelname)s - %(message)s', filename='testing.log')
    app = qt.QtGui.QApplication([])
    window = MainWindow()
    window.begin()
    window.show()
    window.dataCollection.maxDataSize = 300
    window.dataCollection.saveDict = ['ADC','V']

    app.exec()

