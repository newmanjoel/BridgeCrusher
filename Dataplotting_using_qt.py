
# embedding_in_qt4.py --- Simple Qt4 application embedding matplotlib canvases
#
# Copyright (C) 2005 Florent Rougon
#               2006 Darren Dale
#
# This file is an example program for matplotlib. It may be used and
# modified with no restriction; raw copies as well as modified versions
# may be distributed without limitation.

from __future__ import unicode_literals
import sys
import os
import random
from matplotlib.backends import qt_compat
use_pyside = qt_compat.QT_API == qt_compat.QT_API_PYSIDE
if use_pyside:
    from PySide import QtGui, QtCore
else:
    from PyQt4 import QtGui, QtCore

from matplotlib.backends.backend_qt4agg import FigureCanvasQTAgg as FigureCanvas
from matplotlib.figure import Figure


import serial # import Serial Library
import serial.tools.list_ports as list_ports
import numpy as np
import csv

if(float(serial.VERSION) < 3.0):
    print "You do not have a high enough version of pySerial. You need at least pySerial 3.0, You are currently running version {}".format(serial.VERSION)

    print "try running 'pip install pyserial' or if it says its already installed try 'pip install pyserial --force-reinstall'"

    sys.exit(1)
print 'The version of pySerial that I am using is {}'.format(serial.VERSION)

progname = os.path.basename(sys.argv[0])
progversion = "0.1"

class callbacks:
    def __init__(self, localArduino = None):
        self.ardu = localArduino

    def setArduino(self, localArduino):
        self.ardu = localArduino

    def stop(self,event):
        try:
            self.ardu.write(b"S")
            self.ardu.flush()
            print "sending stop"
            sys.stdout.flush()
        except Exception as e:
            print "STOP CALLBACK ERROR: "
            print e.message
            sys.stdout.flush()

    def up(self,event):
        try:
            self.ardu.write(b"U")
            self.ardu.flush()
            print "sending up"
            sys.stdout.flush()
        except Exception as e:
            print "UP CALLBACK ERROR: "
            print e.message
            sys.stdout.flush()

    def down(self,event):
        try:
            self.ardu.write(b"D")
            self.ardu.flush()
            print "sending down"
            sys.stdout.flush()
        except Exception as e:
            print "DOWN CALLBACK ERROR: "
            print e.message
            sys.stdout.flush()

    def clearData(self,event):
        global pressure
        pressure = None
        pressure = np.array([0,0])

class MyMplCanvas(FigureCanvas):
    """Ultimately, this is a QWidget (as well as a FigureCanvasAgg, etc.)."""

    def __init__(self, parent=None, width=5, height=4, dpi=100):
        fig = Figure(figsize=(width, height), dpi=dpi)
        self.axes = fig.add_subplot(111)

        self.compute_initial_figure()

        FigureCanvas.__init__(self, fig)
        self.setParent(parent)

        FigureCanvas.setSizePolicy(self, QtGui.QSizePolicy.Expanding, QtGui.QSizePolicy.Expanding)
        FigureCanvas.updateGeometry(self)

    def compute_initial_figure(self):
        pass

class PlottingArduinoData(MyMplCanvas):
    """A canvas that updates itself every 50ms with a new plot."""

    def __init__(self, *args, **kwargs):
        MyMplCanvas.__init__(self, *args, **kwargs)
        timer = QtCore.QTimer(self)
        timer.timeout.connect(self.update_figure)
        timer.start(1)

    def compute_initial_figure(self):
        self.axes.plot([0, 1, 2, 3], [1, 2, 0, 4], 'r')

    def update_figure(self):
        global arduinoData
        global pressure
        global eventNumber
        global cnt, n
        global firstTime

        self.axes.cla()
        self.axes.plot(pressure, 'r', label='Pressure')
        if arduinoData is None:
            return

        if arduinoData.inWaiting() == 0:
            #this is to reduce CPU overhead
            return

        if firstTime:
            arduinoData.reset_input_buffer()
            firstTime = False

        maxThing = np.argmax(pressure)
        (s,top) = self.axes.get_ylim()
        (s,otherSide) = self.axes.get_xlim()
        top = top*1.0
        otherSide = otherSide*.25

        totalString = "        kg                lbs\nMax:%06.3f        %06.3f\nCurrent:%06.3f        %06.3f"%(pressure[maxThing],pressure[maxThing]*2.20462,pressure[len(pressure)-1],pressure[len(pressure)-1]*2.20462)
        self.axes.text(otherSide,top, totalString,fontsize = 20)

        contestantString = 'Bridge Number %d'%(eventNumber)

        self.axes.text(-1,top,contestantString)

        self.axes.grid(True)


        arduinoString = arduinoData.readline()
        arduinoString = arduinoString.replace('\n','')

        if 'V:' in arduinoString:
            try:
                arduinoString = arduinoString.replace('V:','')
                press = float(arduinoString)
                pressure = np.append(pressure,press)

                if(cnt>=n):
                    pressure = np.delete(pressure,0)
                else:
                    cnt=cnt+1
            except Exception as e:
                print 'Could not read the weight from arduino -> %s'%(arduinoString)
        #elif 'Min' in arduinoString:
        #    callback.up('something')
        elif 'MOTOR' in arduinoString:
            pass #do nothing, we just dont want to print it
        else:
            print 'ARDUINO ->' + arduinoString

        self.draw()

class crusherPopUp(QtGui.QWidget):
    def __init__(self):
        QtGui.QWidget.__init__(self)
        self.setAttribute(QtCore.Qt.WA_DeleteOnClose)
        self.setWindowTitle("Crusher Settings")

        l = QtGui.QVBoxLayout(self)

        l.addWidget(QtGui.QLabel("This is a test to see the position"))
        l.addWidget(QtGui.QLabel("This is a test to see the position"))


class commsPopUp(QtGui.QWidget):
    def __init__(self):
        QtGui.QWidget.__init__(self)
        self.setAttribute(QtCore.Qt.WA_DeleteOnClose)
        self.setWindowTitle("communication device selection")

        l = QtGui.QVBoxLayout(self)

        self.comboBox = QtGui.QComboBox(self)
        self.getComms()
        self.comboBox.setSizeAdjustPolicy(self.comboBox.AdjustToContents)
        self.comboBox.activated[str].connect(self.commsChoice)
        self.comboBox.setStyleSheet('QComboBox QAbstractItemView {min-width: 150px;}')

        l.addWidget(QtGui.QLabel("Please Select the communication device"))
        l.addWidget(self.comboBox)

    def commsChoice(self, text):
        global arduinoData
        global firstTime
        global buttonCallbacks

        arduinoData = None
        print "Recieved the text: {}".format(text)
        for (port, name, PID) in self.ports:
            if text == "{} -> {}".format(name, port):
                print "trying to connnect to {}".format(name)
                arduinoData = serial.Serial(port, 115200)
                buttonCallbacks.setArduino(arduinoData)
                firstTime = True
                print "connnected to {}".format(name)
                #self.statusBar().showMessage("connected to: {}".format(name), 2000)

        if arduinoData is None:
            print 'Failed to create arduinoData object.'

    def getComms(self):
        global arduinoData
        #self.statusBar().showMessage("Getting all of the available ports", 1000)
        #print "Getting all of the available ports"
        self.ports = list(list_ports.comports())
        for (port, name, PID) in self.ports:
            #self.statusBar().showMessage("Adding {} which is port: {}".format(name, port), 500)
            print "Testing {} which is port: {}".format(name, port)
            self.comboBox.addItem("{} -> {}".format(name, port))

class ApplicationWindow(QtGui.QMainWindow):
    def __init__(self):
        global buttonCallbacks

        QtGui.QMainWindow.__init__(self)

        buttonCallbacks = callbacks()
        self.setAttribute(QtCore.Qt.WA_DeleteOnClose)
        self.setWindowTitle("application main window")

        self.file_menu = QtGui.QMenu('&File', self)
        self.file_menu.addAction('&Save', self.saveFile, QtCore.Qt.CTRL + QtCore.Qt.Key_S)
        self.file_menu.addAction('&Quit', self.fileQuit, QtCore.Qt.CTRL + QtCore.Qt.Key_Q)
        self.menuBar().addMenu(self.file_menu)

        self.settingsMenu = QtGui.QMenu('&Settings', self)
        self.settingsMenu.addAction('&Communication', self.commSettings)
        self.settingsMenu.addAction('&Crusher Settings', self.crusherSettings)
        self.menuBar().addMenu(self.settingsMenu)

        self.help_menu = QtGui.QMenu('&Help', self)
        self.menuBar().addSeparator()
        self.menuBar().addMenu(self.help_menu)

        self.help_menu.addAction('&About', self.about)
        self.menuBar().setNativeMenuBar(True)

        self.main_widget = QtGui.QWidget()
        l = QtGui.QVBoxLayout(self.main_widget)

        self.textLabel = QtGui.QLabel("This is a test to check the order of opperations")

        clearButton = QtGui.QPushButton("Clear")
        startButton = QtGui.QPushButton("Start")
        stopButton = QtGui.QPushButton("Stop")
        homeButton = QtGui.QPushButton("Home")
        saveButton = QtGui.QPushButton("Save")

        clearButton.clicked.connect(buttonCallbacks.clearData)
        startButton.clicked.connect(buttonCallbacks.down)
        stopButton.clicked.connect(buttonCallbacks.stop)
        homeButton.clicked.connect(buttonCallbacks.up)
        saveButton.clicked.connect(self.saveFile)

        startButton.setStyleSheet("background-color: rgb(200, 255 ,200)")
        stopButton.setStyleSheet("background-color: rgb(255, 200 ,200)")
        homeButton.setStyleSheet("background-color: rgb(200, 200 ,255)")

        hbox = QtGui.QHBoxLayout()
        hbox.addStretch(1)
        hbox.addWidget(startButton)
        hbox.addWidget(stopButton)
        hbox.addWidget(homeButton)
        hbox.addWidget(saveButton)
        hbox.addWidget(clearButton)

        #sc = MyStaticMplCanvas(self.main_widget, width=5, height=4, dpi=100)
        #dc = MyDynamicMplCanvas(self.main_widget, width=5, height=4, dpi=100)
        myData = PlottingArduinoData(self.main_widget, width=5, height=4, dpi=100)
        #l.addWidget(sc)
        l.addWidget(self.textLabel)
        l.addWidget(myData)
        l.addLayout(hbox)

        self.main_widget.setFocus()
        self.setCentralWidget(self.main_widget)

    def saveFile(self):
        fileName = QtGui.QFileDialog.getSaveFileName(self,"Save the File","",'CSV (*.csv);;All Files (*)')
        if fileName == "":
            return
        with open(fileName,'wb' ) as fo:
            fileWriter = csv.writer(fo)
            for dataPoint in pressure:
                fileWriter.writerow(["{}".format(dataPoint)])

    def commSettings(self):
        self.c = commsPopUp()
        self.c.show()

    def crusherSettings(self):
        print "This is the crusherSettings callback"
        self.cs = crusherPopUp()
        self.cs.show()

    def fileQuit(self):
        self.close()

    def closeEvent(self, ce):
        self.fileQuit()

    def about(self):
        QtGui.QMessageBox.about(self, "About",
"""

This program is used for the Bridge Crusher that was designed and built by.
Joel Newman
Mat McCann
Abdul Fourteia


""")


""" Global Varables """

pressure= np.array([0,0])
arduinoData=None
eventNumber = 0
cnt = 1
n = 10000
firstTime = True
buttonCallbacks = None

qApp = QtGui.QApplication(sys.argv)
aw = ApplicationWindow()
aw.setWindowTitle("%s" % progname)
aw.show()

#sys.exit(qApp.exec_())
qApp.exec_()
