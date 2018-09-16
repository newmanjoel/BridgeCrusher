# -*- coding: utf-8 -*-
"""
Created on Tue Feb 07 13:34:09 2017

@author: joell
"""
import serial # import Serial Library
import serial.tools.list_ports as list_ports
import matplotlib.pyplot as plt #import matplotlib library
from matplotlib.animation import FuncAnimation
import numpy as np
from matplotlib.widgets import  Button
import sys
from PyQt4 import QtGui, QtCore

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
        for i in range(len(pressure)):
            pressure[i] = 0;
        print  "reseting the graph"

    def save_info(self,event):
        global eventNumber
        print 'Trying to save the data'
        with open("test.txt", "a") as myfile:
            writeString = 'Number: %d '%(eventNumber)
            #numpy.savetxt(fname, X, fmt='%.18e', delimiter=' ', newline='\n', header='', footer='', comments='# ')
            np.savetxt(myfile,pressure,fmt=', %0.3f',delimiter=' , ',newline='', header=writeString,footer='\n',comments='')
            #myfile.write(pressure)
        eventNumber +=1
        sys.stdout.flush()

def update():
    global pressure, arduinoData
    cnt= 0
    n = 2000
    while True: # While loop that loops forever
        if (arduinoData.in_waiting==0):                 #Wait here until there is data
            ax.cla()                                           #clear everything in the figure
            ax.plot(pressure, 'r', label='Pressure')         #plot the temperature
            #ax.legend(loc='upper left')                        #plot the legend
            if len(pressure!=0):
                maxThing = np.argmax(pressure)
                (s,top) = ax.get_ylim()
                (s,otherSide) = ax.get_xlim()
                top = top*1.0
                otherSide = otherSide*.25

                totalString = "        kg                lbs\nMax:%06.3f        %06.3f\nCurrent:%06.3f        %06.3f"%(pressure[maxThing],pressure[maxThing]*2.20462,pressure[len(pressure)-1],pressure[len(pressure)-1]*2.20462)
                ax.text(otherSide,top, totalString,fontsize = 20)

                contestantString = 'Bridge Number %d'%(eventNumber)
                ax.text(-1,top,contestantString)

                #if(pressure[len(pressure)-1]*2.20462 > 55):
                #    callback.stop('something')
                #    callback.down('something')
                #ax.annotate(pressure[maxThing], xy=(maxThing, pressure[maxThing]), xytext=(maxThing+5, pressure[maxThing]+5),arrowprops=dict(facecolor='black', shrink=0.05),)

            #plt.ylim(0,1024)                                    #Set y min and max values
            #plt.xlim(0,n)
            #ax.title('My Live Streaming Sensor Data')          #Plot the title
            ax.grid(True)                                      #Turn the grid on
            #ax.ylabel('Weight in Kg')                              #Set ylabels

            plt.pause(0.000001)                                 #Pause Briefly. Important to keep the figure from crashing

        else:
            arduinoString = arduinoData.readline()              #read the line (returns a string)
            if 'V:' in arduinoString:
                try:
                    arduinoString = arduinoString.replace('V:','')
                    press = float( arduinoString)                     #turn the string into a float
                    pressure = np.append(pressure,press)                        #append the latest datapoint to the pressure array

                    if(cnt>=n):                                         #If you have 50 or more points, delete the first one from the array
                        pressure = np.delete(pressure,0)                #This allows us to just see the last 50 data points
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

def getArduinoComms():
#    print "Getting all of the available ports"
#    ports = list(list_ports.comports())
#    arduinoData=None
#    for (port,name,PID) in ports:
#        print "Testing %s which is port: %s"%(name,port)
#        sys.stdout.flush()
#        if "Arduino" in name:
#            print "found the ardunio. opening comms"
#            sys.stdout.flush()
#            arduinoData = serial.Serial(port, 115200) #Creating our serial object named arduinoData
#
#    if arduinoData is None:
#        print 'Failed to create arduinoData object. Exiting'
#        sys.exit(-1)
    print "this is before the show"
    qApp = QtGui.QApplication(sys.argv)
    c = commsPopUp()
    c.show()
    qApp.exec_()
    print "this is after the show"


def init():
    ax.set_xlim(0,2000)
    callback = callbacks(arduinoData)
    axstop = plt.axes([0.7, 0.05, 0.1, 0.075])
    axup = plt.axes([0.81, 0.05, 0.1, 0.075])
    axdown = plt.axes([0.11, 0.05, 0.1, 0.075])
    axreset = plt.axes([0.22, 0.05, 0.1, 0.075])
    axsave = plt.axes([0.5,0.05,0.1,0.075])

    bstart = Button(axup, 'START')
    bstart.on_clicked(callback.up)

    bstop = Button(axstop, 'STOP')
    bstop.on_clicked(callback.stop)

    bhome = Button(axdown, 'HOME')
    bhome.on_clicked(callback.down)

    breset = Button(axreset, 'RESET')
    breset.on_clicked(callback.reset)

    bsave = Button(axsave, 'SAVE')
    bsave.on_clicked(callback.save_info)
    #fig.canvas.mpl_connect('button_press_event', callback.on_press)
    # ax.set_xlim(0, 2*np.pi)
    ax.set_ylim(-1, 1)
    return ln,

def update_frame(frame):
    global pressure, frameNum, arduinoData, xdata
    arduinoString = arduinoData.readline()
    if 'V:' in arduinoString:
        try:
            arduinoString = arduinoString.replace('V:','')
            press = float(arduinoString)                     #turn the string into a float
            pressure = np.append(pressure,press)                        #append the latest datapoint to the pressure array
            frameNum=frameNum+1
            xdata = np.append(xdata,frameNum)
        except Exception as e:
            print "Cought the error {}".format(e.message)

    #print "the length of xdata is {} and the length of pressure is {}".format(len(xdata),len(pressure))
    ln.set_data(xdata, pressure)
    return ln,

if __name__ == "__main__":
    print "this is the main!"
    arduinoData = None
    getArduinoComms()

    fig, ax = plt.subplots()
    plt.subplots_adjust(bottom=0.2)
    eventNumber = 0
    frameNum = 0

    pressure= np.array([])

    xdata = np.array([])
    ln, = plt.plot(xdata, pressure, 'ro', animated=True)

    ani = FuncAnimation(fig, update_frame, frames=arduinoData, init_func=init, blit=True)
    plt.show()
    print "this is the end of __main__"






    #h1, = ax.plot(np.asarray(range(len(pressure))),pressure)                                   # create an empty plot
    #plt.ion()                                               #Tell matplotlib you want interactive mode to plot live data

    #print "I have created the figure, starting the method"
    #sys.stdout.flush()
    #ax.plot(pressure, 'r', label='Pressure')         #plot the temperature

    #try:
    #    update()
    #except Exception as e:
    #    print e.message
    #finally:
    #    print 'closing arduino'
    #    arduinoData.close()
    #print 'everything is done now'
