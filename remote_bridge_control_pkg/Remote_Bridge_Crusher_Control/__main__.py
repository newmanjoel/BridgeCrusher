# -*- coding: utf-8 -*-

#import Remote_Bridge_Crusher_Control as rbcc
import webserver.run_webserver as server
from Hardware_interface.HMIBox import HMI as Hmi
import threading
import logging

print("this is in the main.")
logging.basicConfig(level=logging.WARNING, format='%(asctime)s - %(name)s - %(levelname)s - %(message)s')

if __name__ == '__main__':
    try:
        hmi = Hmi()
        hmi.port = 'COM8'
        hmi.history_length = 5000
        #t1 = threading.Thread(target=run_webserver.start_data_collection)
        t2 = threading.Thread(target=server.start_web_server)
        #t1.start()
        t2.start()
    except Exception as error:
        print ("error: unable to start a thread due to :"+error)
