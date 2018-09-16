# -*- coding: utf-8 -*-
"""
Created on Tue Feb 07 13:34:09 2017

@author: joell
"""
import serial # import Serial Library
import serial.tools.list_ports as list_ports
import matplotlib.pyplot as plt #import matplotlib library
from matplotlib.widgets import  Button
import numpy as np
import sys
from PyQt4 import QtGui, QtCore
import time
import csv


class commsPopUp(QtGui.QWidget):
    def __init__(self):
        QtGui.QWidget.__init__(self)
        self.setAttribute(QtCore.Qt.WA_DeleteOnClose)
        self.setWindowTitle("communication device selection")
        self.setWindowModality(QtCore.Qt.ApplicationModal)

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

class savePopUp(QtGui.QWidget):
    def __init__(self):
        global pressure, xdata
        QtGui.QWidget.__init__(self)
        self.setAttribute(QtCore.Qt.WA_DeleteOnClose)
        self.setWindowTitle("communication device selection")
        self.setWindowModality(QtCore.Qt.ApplicationModal)

        fileName = QtGui.QFileDialog.getSaveFileName(self,"Save the File","",'CSV (*.csv);;All Files (*)')
        if fileName == "":
            return
        with open(fileName,'wb') as fo:
            fileWriter = csv.writer(fo)
            fileWriter.writerow(['time (seconds)', 'weight (kg)'])
            for dataPoint in range(len(pressure)):
                fileWriter.writerow(["{}".format(xdata[dataPoint]) , "{}".format(pressure[dataPoint])])


class callbacks:

    def __init__(self, localArduino=None):
        self.ardu = localArduino

    def stop(self,event):
        try:
            self.ardu.write("S");
            self.ardu.flush();
            print "sending stop"
            sys.stdout.flush()
        except Exception as e:
            print "STOP CALLBACK ERROR: "
            print e.message
            sys.stdout.flush()

    def up(self,event):
        try:
            self.ardu.write("U");
            self.ardu.flush();
            print "sending up"
            sys.stdout.flush()
        except Exception as e:
            print "UP CALLBACK ERROR: "
            print e.message
            sys.stdout.flush()

    def down(self,event):
        try:
            self.ardu.write("D");
            self.ardu.flush();
            print "sending down"
            sys.stdout.flush()
        except Exception as e:
            print "DOWN CALLBACK ERROR: "
            print e.message
            sys.stdout.flush()

    def on_press(self,event):
        sys.stdout.flush()

    def reset(self,event):
        global pressure, xdata, base_time, current_frame
        current_frame = 0
        base_time = time.time()
        pressure= np.array([0.0, 0.0])
        xdata = np.array([0.0, 0.0])


    def save_info(self, event):
        global pressure
        print 'key event of: |{}|'.format(event.key)
        if event.key == u'ctrl+s':
            #print "I SHOULD BE SAVING!"
            s = savePopUp()

    @staticmethod
    def leave_everything(event):
        if(arduinoData != None):
            arduinoData.close()
        sys.exit(0)


def update():
    global pressure, arduinoData, xdata, base_time, current_frame
    display_frames = 500


    while True: # While loop that loops forever
        if (arduinoData.in_waiting==0):                 #Wait here until there is data
            ax.cla()                                           #clear everything in the figure
            ax.plot(xdata[-display_frames:-1], pressure[-display_frames:-1], 'r', label='Pressure')         #plot the temperature
            #ax.legend(loc='upper left')                        #plot the legend
            #if len(pressure!=0):
            maxThing = np.argmax(pressure)
            maxDisplay = np.max(pressure[-display_frames:-1])
            (s,top) = plt.ylim()
            (s,otherSide) = plt.xlim()
            top = top*1.1
            otherSide = otherSide*.25

            totalString = "{:^30}{:^30}\nMax:{:^24}{:^30}\nCurrent:{:^22}{:^30}".format('kg','lbs',"{:.3f}".format(pressure[maxThing]),"{:.3f}".format(pressure[maxThing]*2.20462),"{:.3f}".format(pressure[len(pressure)-1]),"{:.3f}".format(pressure[len(pressure)-1]*2.20462))
            #print "totalString"
            #print totalString
            #ax.text(otherSide,top, totalString,fontsize = 20)

            #contestantString = 'Bridge Number %d'%(eventNumber)
            #ax.text(-1,top,contestantString)

                #if(pressure[len(pressure)-1]*2.20462 > 55):
                #    callback.stop('something')
                #    callback.down('something')
            #ax.annotate(pressure[maxDisplay], xy=(maxDisplay, pressure[maxDisplay]), xytext=(maxDisplay+5, pressure[maxDisplay]+5),arrowprops=dict(facecolor='black', shrink=0.05),)

            plt.ylim(ymax=maxDisplay+maxDisplay*0.1)
            if current_frame< display_frames:
                plt.xlim(xmax=xdata[-1]+1)
            else:
                plt.xlim(xdata[-display_frames]-1, xdata[-1]+1)
            ax.grid(True)
            ax.set_ylabel('Weight in Kg', fontsize=20)
            ax.set_xlabel('Time in Seconds', fontsize = 20)
            ax.set_title(totalString, fontsize = 40)
            plt.pause(0.000001)                                 #Pause Briefly. Important to keep the figure from crashing

        else:
            arduinoString = arduinoData.readline()              #read the line (returns a string)
            if 'V:' in arduinoString:
                try:
                    arduinoString = arduinoString.replace('V:','')
                    press = float( arduinoString)                     #turn the string into a float
                    pressure = np.append(pressure,press)
                    xdata = np.append(xdata,time.time()-base_time)
                    #if(current_frame>=display_frames):                                         #If you have 50 or more points, delete the first one from the array
                    #    pressure = np.delete(pressure,0)                #This allows us to just see the last 50 data points
                    #else:
                    current_frame=current_frame+1
                except Exception as e:
                    print 'Could not read the weight from arduino -> %s'%(arduinoString)
            #elif 'Min' in arduinoString:
            #    callback.up('something')
            elif 'MOTOR' in arduinoString:
                pass #do nothing, we just dont want to print it
            else:
                print 'ARDUINO ->' + arduinoString

def getArduinoComms():
    qApp = QtGui.QApplication(sys.argv)
    popUp = commsPopUp()
    popUp.show()
    qApp.exec_()



if __name__ == "__main__":

    arduinoData = None
    getArduinoComms()

    fig, ax = plt.subplots()
    plt.subplots_adjust(bottom=0.2)
    eventNumber = 0

    fig.canvas.set_window_title('This is the test window title')

    current_frame= 0
    base_time = time.time()
    pressure= np.array([0.0, 0.0])
    xdata = np.array([0.0, 0.0])


    callback = callbacks(arduinoData)
    axstop = plt.axes([0.7, 0.05, 0.1, 0.075])
    axup = plt.axes([0.81, 0.05, 0.1, 0.075])
    axdown = plt.axes([0.11, 0.05, 0.1, 0.075])
    axreset = plt.axes([0.22, 0.05, 0.1, 0.075])
    axleave = plt.axes([0.5,0.05,0.1,0.075])

    # disable saving png with control s
    plt.rcParams['keymap.save'] = ''
    fig.canvas.mpl_connect('key_press_event', callback.save_info)

    bstart = Button(axup, 'START', color='#96f97b', hovercolor='#15b01a')
    bstart.on_clicked(callback.up)

    bstop = Button(axstop, 'STOP', color='#ffd1df', hovercolor='#e50000')
    bstop.on_clicked(callback.stop)

    bhome = Button(axdown, 'HOME', color='#95d0fc', hovercolor='#0343df')
    bhome.on_clicked(callback.down)

    breset = Button(axreset, 'RESET')
    breset.on_clicked(callback.reset)

    bleave = Button(axleave, 'EXIT')
    bleave.on_clicked(callbacks.leave_everything)
    #fig.canvas.mpl_connect('button_press_event', callback.on_press)


    h1, = ax.plot(xdata,pressure)
    plt.ion()

    print "I have created the figure, starting the method"
    sys.stdout.flush()
    ax.plot(pressure, 'r', label='Pressure')         #plot the temperature

    try:
        update()
    except Exception as e:
        print e.message
    finally:
        print 'closing arduino'
        arduinoData.close()
    print 'everything is done now'
