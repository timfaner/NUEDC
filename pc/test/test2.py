cfg1={\
    'MONITOR_ERROR':1,\
    'MONITOR_EVENT':2
}
import sys
if sys.version_info[0] < 3:
    #If we're executing with Python 2
    import Tkinter as tk
    import tkMessageBox as messagebox
    import ttk
    import tkFileDialog as filedialog
    import tkColorChooser as colorchooser
else:
    #Otherwise we're using Python 3
    import tkinter as tk
    from tkinter import ttk
 

class frame(ttk.Frame):
    def __init__(self,parent):
            ttk.Frame.__init__(self,parent)
            m1 = Mav()
            
    

class Mav(ttk.Frame):
    def __init__(self):
        b1 = tk.Button("test")
        b1.pack()


class RSA_STATUS:
    def __init__(self):
        self.old_pkg_count = 0      
        self.pkg_cnt = 0
    
    def setStream(self,serial_stream):
        self.stream = serial_stream
        self.__spacepos = self.stream.find(32)
        self.systime_raw = self.stream[0:self.__spacepos]
        self.systime = int((self.systime_raw).decode('ascii'))

        self.old_pkg_count = self.pkg_cnt
        self.pkg_cnt = self.stream[self.__spacepos+1]

        self.type = self.stream[self.__spacepos+2]

        self.data_raw = self.stream[self.__spacepos+3:self.stream.find(10)]
        
        if(self.type == cfg1['MONITOR_EVENT']):
            if(len(self.data_raw)==2):
                self.data = self.data_raw[1]<<8 | self.data_raw[0]
            else:
                self.data = self.data_raw[0]
        elif(self.type == cfg1['MONITOR_ERROR']):
            self.data = self.data_raw
        else:
            self.data = self.data_raw.decode('ascii').split(',')
            for r in self.data[]

class timestart:
    def __init(self)
test = RSA_STATUS()
raw = b'101 a\x02\xff\xff\n'
test.setStream(raw)

test.set





