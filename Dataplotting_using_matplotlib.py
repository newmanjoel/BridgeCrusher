# -*- coding: utf-8 -*-
"""
Created on Tue Feb 07 13:34:09 2017

@author: joell
"""
import serial
import serial.tools.list_ports as list_ports
import matplotlib.pyplot as plt
from matplotlib.widgets import Button
import numpy as np
import sys
from PyQt4 import QtGui, QtCore
import time
import csv


class commsPopUp(QtGui.QWidget):
    '''
    This is the small window that pops up when you start the program to select the port that the
    arduino is on.

    When this window is closed or a valid communication port is selected the program will
    continue loading everything.
    '''

    def __init__(self):
        ''' init all of the things '''
        QtGui.QWidget.__init__(self)

        # window properties
        self.setAttribute(QtCore.Qt.WA_DeleteOnClose)
        self.setWindowTitle("communication device selection")
        self.setWindowModality(QtCore.Qt.ApplicationModal)

        # define the layout of the window
        layout = QtGui.QVBoxLayout(self)

        # setup the combo box and populate it with the current COM ports
        self.comboBox = QtGui.QComboBox(self)
        self.getComms()
        self.comboBox.setSizeAdjustPolicy(self.comboBox.AdjustToContents)
        self.comboBox.activated[str].connect(self.commsChoice)
        self.comboBox.setStyleSheet('QComboBox QAbstractItemView {min-width: 150px;}')

        # add the combobox to the window and a label
        layout.addWidget(QtGui.QLabel("Please Select the communication device"))
        layout.addWidget(self.comboBox)

    def commsChoice(self, text):
        ''' Tries to connect to the selected com port. If sussesful then will close the window '''
        global arduinoData

        # make sure that the arduinoData object is clear (probably overkill)
        arduinoData = None

        # display the text that we received (mostly for debugging) then try to connect
        print "Recieved the text: {}".format(text)
        for (port, name, _) in self.ports:
            if text == "{} -> {}".format(name, port):
                print "trying to connnect to {}".format(name)
                arduinoData = serial.Serial(port, 115200)
                print "connnected to {}".format(name)
                self.close()

        if arduinoData is None:
            print 'Failed to create arduinoData object.'

    def getComms(self):
        ''' Loads all of the COM ports and puts them into the combo box. Some string formatting '''
        global arduinoData

        self.ports = list(list_ports.comports())
        for (port, name, _) in self.ports:
            self.comboBox.addItem("{} -> {}".format(name, port))


class savePopUp(QtGui.QWidget):
    ''' Creates a popup used for saving the data that is in the main screen '''
    def __init__(self):
        ''' initialize everything making sure we have this window has maximum priority '''
        global pressure, xdata
        QtGui.QWidget.__init__(self)
        self.setAttribute(QtCore.Qt.WA_DeleteOnClose)
        self.setWindowTitle("communication device selection")
        self.setWindowModality(QtCore.Qt.ApplicationModal)

        # use the built in savefile name thing
        fileName = QtGui.QFileDialog.getSaveFileName(self, "Save the File", "", 'CSV (*.csv);;All Files (*)')

        # if the user decides not to save then do nothing
        if fileName == "":
            return

        # try and save the file in CSV format
        with open(fileName, 'wb') as fo:
            fileWriter = csv.writer(fo)
            fileWriter.writerow(['time (seconds)', 'weight (kg)'])
            for dataPoint in range(len(pressure)):
                fileWriter.writerow(["{}".format(xdata[dataPoint]), "{}".format(pressure[dataPoint])])


class callbacks:
    '''
    Main class used for callbacks of all sort.

    Used for closing the main application as well as sending data to the arduino.
    '''

    def __init__(self, localArduino=None):
        ''' Making sure that we always have a local arduino in the class even its its none '''
        self.ardu = localArduino

    def stop(self, event):
        ''' Send the stop command to the arduino '''
        try:
            self.ardu.write("S")
            self.ardu.flush()
            print "sending stop"
            sys.stdout.flush()
        except Exception as e:
            print "STOP CALLBACK ERROR: "
            print e.message
            sys.stdout.flush()

    def up(self, event):
        ''' Send the command to make the system crush something '''
        try:
            self.ardu.write("U")
            self.ardu.flush()
            print "sending up"
            sys.stdout.flush()
        except Exception as e:
            print "UP CALLBACK ERROR: "
            print e.message
            sys.stdout.flush()

    def down(self, event):
        ''' Send the command to make the system return to home (non crushing location) '''
        try:
            self.ardu.write("D")
            self.ardu.flush()
            print "sending down"
            sys.stdout.flush()
        except Exception as e:
            print "DOWN CALLBACK ERROR: "
            print e.message
            sys.stdout.flush()

    def on_press(self, event):
        ''' not used, for completeness '''
        sys.stdout.flush()

    def reset(self, event):
        ''' reset all of the data '''
        global pressure, xdata, base_time, current_frame
        current_frame = 0
        base_time = time.time()
        pressure = np.array([0.0, 0.0])
        xdata = np.array([0.0, 0.0])

    def save_info(self, event):
        ''' Handing all of the key_events. This saves and stops the crusher using ctrl+s and space '''
        global pressure
        print 'key event of: |{}|'.format(event.key)
        if event.key == u'ctrl+s':
            # print "I SHOULD BE SAVING!"
            savePopUp()
        elif event.key == ' ':
            # print ' I should be stopping because space was hit'
            self.stop()

    @staticmethod
    def leave_everything(event):
        ''' Closing everything and exiting the program '''
        if(arduinoData is not None):
            arduinoData.close()
        sys.exit(0)


def update():
    '''
    Main function to get data from the arduino and refresh the graph.
    All the fun stuff lives in this function.
    '''
    global pressure, arduinoData, xdata, base_time, current_frame
    display_frames = 500

    # While loop that loops forever
    while True:
        # Test to see if there is new data or not
        if (arduinoData.in_waiting == 0):
            # clear everything in the figure
            ax.cla()
            # plot the current pressure
            ax.plot(xdata[-display_frames:-1], pressure[-display_frames:-1], 'r', label='Pressure')
            # plot the legend (uncomment to display)
            # ax.legend(loc='upper left')
            # find the location of the mamimum value in the array (even for things offscreen)
            maxThing = np.argmax(pressure)
            # find the max value that is shown in the display
            maxDisplay = np.max(pressure[-display_frames:-1])

            # getting the limits for display purposes
            (s, top) = plt.ylim()
            (s, otherSide) = plt.xlim()
            top = top*1.1
            otherSide = otherSide*.25

            # form the string that is displayed above the graph
            totalString = "{:^30}{:^30}\nMax:{:^24}{:^30}\nCurrent:{:^22}{:^30}".format(
                'kg',
                'lbs',
                "{:.3f}".format(pressure[maxThing]),
                "{:.3f}".format(pressure[maxThing]*2.20462),
                "{:.3f}".format(pressure[len(pressure)-1]),
                "{:.3f}".format(pressure[len(pressure)-1]*2.20462))

            plt.ylim(ymax=maxDisplay+maxDisplay*0.1)
            if current_frame < display_frames:
                plt.xlim(xmax=xdata[-1]+1)
            else:
                plt.xlim(xdata[-display_frames]-1, xdata[-1]+1)

            '''
            set up the formatting of the graph and surrounding area
            if you want to adjust the size of the text for anything, that is done here
            '''

            ax.grid(True)
            ax.set_ylabel('Weight in Kg', fontsize=20)
            ax.set_xlabel('Time in Seconds', fontsize=20)
            ax.set_title(totalString, fontsize=40)

            # Pause Briefly. Important to keep the figure from crashing
            plt.pause(0.000001)
        # This is when there is new data
        else:
            # read the line (returns a string)
            arduinoString = arduinoData.readline()
            if 'V:' in arduinoString:
                try:
                    # remove the identifier
                    arduinoString = arduinoString.replace('V:', '')
                    # turn the string into a float
                    press = float(arduinoString)
                    # add the current pressure to the main array for processing
                    pressure = np.append(pressure, press)
                    # use relative time
                    xdata = np.append(xdata, time.time()-base_time)
                    # keep track of how many datapoints we have (could also be found from len(xdata))
                    current_frame = current_frame + 1
                except Exception:
                    print 'Could not read the weight from arduino -> {}'.format(arduinoString)
            elif 'MOTOR' in arduinoString:
                # do nothing, we just dont want to print it
                pass
            else:
                # if we receive a non-standard message, print it out
                print 'ARDUINO -> '.format(arduinoString)


def getArduinoComms():
    ''' start the QT setup, show the popup selector, then close the QT system '''
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

    # initialize some of the import data
    current_frame = 0
    base_time = time.time()
    pressure = np.array([0.0, 0.0])
    xdata = np.array([0.0, 0.0])

    # setup the callbacks and the button locations
    callback = callbacks(arduinoData)
    buttonBottom = 0.05
    buttonWidth = 0.1
    buttonHeight = 0.075
    axstop = plt.axes([0.7, buttonBottom, buttonWidth, buttonHeight])
    axup = plt.axes([0.81, buttonBottom, buttonWidth, buttonHeight])
    axdown = plt.axes([0.11, buttonBottom, buttonWidth, buttonHeight])
    axreset = plt.axes([0.22, buttonBottom, buttonWidth, buttonHeight])
    axleave = plt.axes([0.5, buttonBottom, buttonWidth, buttonHeight])

    # disable saving png with ctrl+s and remap it to our function
    plt.rcParams['keymap.save'] = ''
    fig.canvas.mpl_connect('key_press_event', callback.save_info)

    # setup the start button
    bstart = Button(axup, 'START', color='#96f97b', hovercolor='#15b01a')
    bstart.on_clicked(callback.up)

    # setup the stop button
    bstop = Button(axstop, 'STOP', color='#ffd1df', hovercolor='#e50000')
    bstop.on_clicked(callback.stop)

    # setup the home button
    bhome = Button(axdown, 'HOME', color='#95d0fc', hovercolor='#0343df')
    bhome.on_clicked(callback.down)

    # setup the reset button
    breset = Button(axreset, 'RESET')
    breset.on_clicked(callback.reset)

    # setup the exit button
    bleave = Button(axleave, 'EXIT')
    bleave.on_clicked(callbacks.leave_everything)

    # make sure that if you use the 'x' button that it does the same thing as the exit button
    fig.canvas.mpl_connect('close_event', callbacks.leave_everything)

    # make sure there is some data at the start or it will look funny
    h1, = ax.plot(xdata, pressure)
    plt.ion()

    print "I have created the figure, starting the method"
    sys.stdout.flush()
    # used for creating the label
    ax.plot(pressure, 'r', label='Pressure')

    try:
        update()
    except Exception as e:
        print e.message
    finally:
        print 'closing arduino'
        if arduinoData is not None:
            arduinoData.close()
    print 'everything is done now'
