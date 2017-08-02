'''
The GraphTopLevel is a ttk.Frame class that gets packd into the root window when
the configuration is complete. All the graph updating takes place in updateGraph
'''
tempk = []
import threading
cfg1={\
    'MONITOR_ERROR':1,\
    'MONITOR_EVENT':2,\
    'DATA_OPENMV':  			 97,\
    'DATA_X_OFFSET_RESULT': 	 98,\
    'DATA_Y_OFFSET_RESULT': 	 99,\
    'DATA_PID_X_SPEED': 		 100,\
    'DATA_PID_Y_SPEED':		     101,\
    'DATA_ATTITUDE': 			 102,\
    'DATA_PID_PARAMETERS':	 	 103,\
    'DATA_TASK_NUMBER':          104\
}
cfg_line_to_data = {
    'PID_X':1,\
    'PID_Y':2,\
    'MV_X':3,\
    'MV_Y':4,\
    'Pitch':5,\
    'Yaw':6,\
    'Row':7,\
    '-':'-',\
    0:0
}
cfg_data = {
    'DATA_OPENMV':  			 97,\
    'DATA_X_OFFSET_RESULT': 	 98,\
    'DATA_Y_OFFSET_RESULT': 	 99,\
    'DATA_PID_X_SPEED': 		 100,\
    'DATA_PID_Y_SPEED':		     101,\
    'DATA_ATTITUDE': 			 102,\
    'DATA_PID_PARAMETERS':	 	 103,\
    'DATA_TASK_NUMBER':          104\
}
cfg_event ={\
    'BOOTUP' 			:1,\
    'STARTBUTTON' 		:2,\
    'NOLOCALPOSITION' 	:4,\
    'ARMCHECK' 			:8,\
    'TAKEOFF' 			:16,\
    'OPENMVBOOTUP' 		:32,\
    'TRACKING' 			:64,\
    'LOSTLINE'			:128,\
    'PRELAND' 			:256,\
    'LANDING' 			:512,\
    'LANDED'  			:1024,\
    'OVERFLY' 			:2048,\
    'TASK1' 			:4096,\
    'TASK2' 			:8192,\
    'TASK3' 			:16384,\
    'TASK4' 			:32768\
    }

cfg_error ={\
    'ERROR_TASK_NUMBER':		 65,\
    'ERROR_SPI_DATA': 	  		 66,\
    'ERROR_UNKNOWN_PLACE': 	     67,\
    'ERROR_CANNOT_GET_DATA': 	 68,\
    'ERROR_WRONG_ORDER': 		 69,\
    }

def dictOp(cfg,itemss):
    temp = -1
    for key in cfg:
        if cfg[key] == itemss:
            temp = key
            break
    if(temp == -1):
        return 'Dictnotfound'
    else:
        return temp


from threading import Timer
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
from matplotlib.figure import Figure
import matplotlib.animation as animation


import sys
import serial
import io
import time
import os
import sched

if sys.version_info[0] < 3:
    #If we're executing with Python 2
    import Tkinter as tk
    import ttk
    import tkMessageBox as messagebox
    serialqueue = serial.Serial.inWaiting
    
else:
    #Otherwise we're using Python 3
    import tkinter as tk
    from tkinter import ttk
    from tkinter import messagebox    
    serialqueue = serial.Serial.inWaiting    




class GraphTopLevel:
    """
    Configures root directory for graphing - deletes config frame, adds all
    widgets necessary for graphing
    """
    def __init__(self, root):
        
        #This is the tk.Tk() root
        self.root = root
        
        #Since we're done with the serial configuration, we want to get rid of 
        #the configuration stuff. Destroy the frame.
        self.root.CfgFrm.destroy()

        #Configure the root directory to reflect the fact that it's graphing now
        self.root.title('Serial Data Live Graph')
        self.root.resizable(width=True, height=True)
        self.root.unbind("<Return>")
       

        #Change the geometry so that if the window is un-maximized, it's good
        #Move to the center of the screen
        self.root.withdraw()                
        self.root.update()
        scrwidth = self.root.winfo_screenwidth()
        scrheight = self.root.winfo_screenheight()
        winwidth = 950
        winheight = 500
        
        winposx = int(round(scrwidth/2 - winwidth/2))
        winposy = int(round(scrheight/2 - winheight/2))
        self.root.geometry('{}x{}+{}+{}'.format(winwidth, winheight, winposx, winposy))
        self.root.deiconify()
        self.root.config(background = 'white')
        if self.root.variables['startmax'] == 'yes':        
            #Maximize the window by default
            self.root.state('zoomed')  

        #Add the GraphFrame, which contains all the widgets we want
        self.GraphFrm = GraphFrame(self.root, self.root)
        #self.GraphFrm.pack(fill='both', expand=True) 
        self.root.resizable(width = False,height = False)
        
        #Change the function of the X button on the upper right hand corner - 
        #Make it call a function which closes the serial port and then closes
        #the program
        self.root.wm_protocol("WM_DELETE_WINDOW", self.GraphFrm.close_prog)        

    
class GraphFrame(ttk.Frame):
    def __init__(self, parent, root):
        ttk.Frame.__init__(self, parent)
        self.parent = parent
        self.root = root
        
        #Create the variables for the statusbar
        self.root.variables.update({'buffsize':tk.StringVar()})
        self.root.variables.update({'lastline':tk.StringVar(value='Nothing Recieved')})
        self.root.variables.update({'refreshrate':tk.StringVar(value=0)})
        self.root.count = 0     #the number of lines we've recieved
        self.root.starttime = 0 #time when we started      
        
        
        self.root.package_error = tk.StringVar(self,value = 'Null')
        self.rsa = RSA_STATUS(self.root)
        self.root.line_y = []
        self.root.line_x = []
        for line in range(9):
            self.root.line_y.append([])
            self.root.line_x.append([])
        
        self.init_time = 0
        self.root.rsa_data = tk.StringVar(self,value = 'Null')
        self.root.rsa_type = tk.StringVar(self,value = 'Null')
        self.root.rsa_time = tk.IntVar(self,value = 0)
        self.root.time_for_graph = tk.IntVar(self,value = 0)
        self.root.rsa_event = tk.IntVar(self,value = 0)
        self.root.rsa_params = []
        self.root.rsa_params = [tk.StringVar(self,value = 'p'),tk.StringVar(self,value = 'i'),\
                                tk.StringVar(self,value = 'd'),tk.StringVar(self,value = 'task')]
        self.root.rsa_error =  tk.StringVar(self,value = 'No Error')
        self.root.linenumber = tk.IntVar(self,value = -1)
        self.root.rsa_interval =tk.StringVar(self,value = 'Null')
        self.root.graph_data_ready =0
        self.root.graphstr_y = tk.DoubleVar(self,value = 0.0)
        self.root.package_statu = tk.StringVar(self,value = 'NoPackage')
        self.root.count = 0
        self.templog = ''


        statrbtn = ttk.Button(self.root,text = 'Start',command = self.startpro)
        stopbtn = ttk.Button(self.root,text = "Stop",command = self.stoppro)

        

        
        #tk.grid_rowconfigure(1,minisize = 10)
        statrbtn.grid(row = 0,column = 0) 
        stopbtn.grid(row = 0,column = 1) 
        MAVStatus(self.root, self.root,text = 'Mav_staus',height = 800).grid(row = 1,columnspan =2)
        Graph(self.root, self.root).grid(row = 0,column = 2,sticky = 's',rowspan = 2) 
        StatusBar(self.root, self.root).grid(row = 2,column = 0,columnspan = 3,sticky = 's')

        self.root.bind('<Return>',self.test)
        
        
        self.creatAndOpenSerial()
        self.root.ser.readline()
        self.root.ser.close()
        self.first_time = 1
        if self.root.variables['log2file'] == 'on': 
                self.root.logfile = open(self.root.variables['filename'], 'a') 
                
        
    def test(self,*arg):
        print('test')
        print( self.root.ser.readline())

        
    def startpro(self):
        
        
        
        
        try:
            if(self.root.ser.isOpen()):
        
                pass
            else:
                self.root.ser.open()

            if float(serial.VERSION[0:3]) < 3:
            #If we're executing with pySerial 2.x
                serial.Serial.flushInput(self.root.ser)
                serial.Serial.flushOutput(self.root.ser)
            else:
            #Otherwise we're using pySerial 3.x
                serial.Serial.reset_input_buffer(self.root.ser)
                serial.Serial.reset_output_buffer(self.root.ser)
        except Exception as e:
            self.first_time = 1
            messagebox.showerror(message=('Serial Fault:', e))
            self.stop_flag = 1
            
        self.stop_flag = 0
        
        if(1):  #不想退格了。。
            try:
                ret = self.root.ser.readline()
                if(ret == b''): #若读取失败，则跳过
                    messagebox.showerror(message=('串口超时'))
                    self.stop_flag = 1
                    self.first_time = 1
                else:
                    #更新数据接收率
                    self.root.count +=1
                    self.root.variables['refreshrate'].set(\
                    value='{:.1f}'.format(round(self.root.count/time.clock(), 1)))
                    #获取buff长度
                    bufflen = serialqueue(self.root.ser)
                    self.root.variables['buffsize'].set(value=str(bufflen)) 
                    
                    self.rsa.setStream(ret)
                    
                    
                    
                    self.root.rsa_time.set(int(self.rsa.getTime()))
                    if(self.first_time == 1):
                        self.root.line_y = []
                        self.root.line_x = []
                        for line in range(9):
                            self.root.line_y.append([])
                            self.root.line_x.append([])
                        self.last_time = time.time()
                        self.init_time = self.root.rsa_time.get()
                        try:
                            if self.root.logfile.closed:
                                self.root.logfile  = open(self.root.variables['filename'], 'a')
                        except :
                            pass
                        self.first_time = 0

                    self.root.time_for_graph.set(int(self.rsa.getTime())-self.init_time)
                    if self.rsa.getType() == cfg1['MONITOR_EVENT']:
                        self.root.rsa_event.set(self.rsa.getData())
                    elif self.rsa.getType() == cfg1['MONITOR_ERROR']:
                        self.root.rsa_error.set(dictOp(cfg_error,self.rsa.getData()))
                    else:
                        self.root.rsa_data.set(self.rsa.getData())
                        p1 = p2 = p3 = p4 = p5 = p6 = p7 = '0'
                        
                        if( dictOp(cfg_data,self.rsa.getType()) == 'DATA_PID_X_SPEED'):
                            p1 = self.rsa.getData()
                            self.root.graphstr_y.set(p1)
                            self.root.linenumber.set(0)
                            self.root.graph_data_ready = 1
                            
                        elif( dictOp(cfg_data,self.rsa.getType()) == 'DATA_PID_Y_SPEED'):
                            p2 = self.rsa.getData()
                            self.root.graphstr_y.set(p2)
                            self.root.linenumber.set(1)
                            self.root.graph_data_ready = 1
                            
                        elif( dictOp(cfg_data,self.rsa.getType()) == 'DATA_X_OFFSET_RESULT'):
                            p3 = self.rsa.getData()
                            self.root.graphstr_y.set(p3)
                            self.root.linenumber.set(2)
                            self.root.graph_data_ready = 1
                            
                        elif( dictOp(cfg_data,self.rsa.getType()) == 'DATA_Y_OFFSET_RESULT'):
                            p4 = self.rsa.getData()
                            self.root.graphstr_y.set(p4)
                            self.root.linenumber.set(3)
                            self.root.graph_data_ready = 1
                            
                        elif( dictOp(cfg_data,self.rsa.getType()) == 'DATA_ATTITUDE'):
                            #pitch
                            p5 = self.rsa.getData()[0]
                            self.root.graphstr_y.set(p5)
                            self.root.linenumber.set(4)
                            self.root.graph_data_ready = 1
                            #yaw
                            p6 = self.rsa.getData()[1]
                            self.root.graphstr_y.set(p6)
                            self.root.linenumber.set(5)
                            self.root.graph_data_ready = 1
                            #row
                            p7 = self.rsa.getData()[2]
                            self.root.graphstr_y.set(p7)
                            self.root.linenumber.set(6)
                            self.root.graph_data_ready = 1
                            
                        elif( dictOp(cfg_data,self.rsa.getType()) == 'DATA_PID_PARAMETERS'):
                            self.root.rsa_params[0].set(self.rsa.getData()[0])
                            self.root.rsa_params[1].set(self.rsa.getData()[1])
                            self.root.rsa_params[2].set(self.rsa.getData()[2])
                    
                        
                        elif( dictOp(cfg_data,self.rsa.getType()) == 'DATA_TASK_NUMBER'):
                            self.root.rsa_params[3].set(self.rsa.getData())
                    if self.root.variables['log2file'] == 'on' :
                        self.templog += str(self.rsa.getType()) +' '+str(self.root.rsa_time.get())+' '\
                        +  str(self.rsa.getData()) + '\n'

                        if time.time() >self.last_time+1:
                            self.root.logfile.write(self.templog)
                            self.templog = ''
                            self.last_time = time.time()
                        
                            
                    self.root.count = 0
            except Exception as e:
                self.first_time = 1
                messagebox.showerror(message=('Start Failed:', e))
                self.root.ser.close()
                self.stop_flag = 1
                
                
        if(self.stop_flag != 1):    
            self.serial_timer = Timer(0.01,self.timetriger)
            self.serial_timer.start()
            print("tick")
        else:
            if self.root.ser:
                self.root.ser.close()
            print("tock")
            self.first_time = 1

    def timetriger(self):
        self.startpro()

    def stoppro(self):
        self.first_time = 1
        self.stop_flag = 1
        if self.serial_timer:
            self.serial_timer.cancel()

        
        self.stop_flag = 1
        self.first_time = 1
        if self.root.variables['log2file'] == 'on': 
            
            if not self.root.logfile:
                self.root.logfile.write(self.templog)
                self.templog
                self.root.logfile.close()

    def creatAndOpenSerial(self):
        #Set up the relationship between what the user enters and what the API calls for
        bytedic = {'5':serial.FIVEBITS,
                   '6':serial.SIXBITS,
                   '7':serial.SEVENBITS,
                   '8':serial.EIGHTBITS}
        bytesize = bytedic[str(self.root.variables['databits'])]
        
        paritydict = {'None':serial.PARITY_NONE,
                      'Even':serial.PARITY_EVEN,
                      'Odd' :serial.PARITY_ODD,
                      'Mark':serial.PARITY_MARK,
                      'Space':serial.PARITY_SPACE}
        parity=paritydict[self.root.variables['parity']]
        
        stopbitsdict = {'1':serial.STOPBITS_ONE,
                        '2':serial.STOPBITS_TWO}
        stopbits = stopbitsdict[str(self.root.variables['stopbits'])]
            
        #Open the serial port given the settings, store under the root
        if os.name == 'nt':
            port = self.root.variables['COMport'][0:5].strip()
            self.root.ser = serial.Serial(\
                port=port,\
                baudrate=str(self.root.variables['baud']),\
                bytesize=bytesize, parity=parity, stopbits=stopbits, timeout=2) 
        else:
            first_space = self.root.variables['COMport'].index(' ')
            port = self.root.variables['COMport'][0:first_space].strip()
            # Parameters necessary due to https:github.com/pyserial/pyserial/issues/59
            self.root.ser = serial.Serial(\
                port=port,\
                baudrate=str(self.root.variables['baud']),\
                bytesize=bytesize, parity=parity, stopbits=stopbits, timeout=2, rtscts=True, dsrdtr=True) 
        io.DEFAULT_BUFFER_SIZE = 5000
            
        #Purge the buffer of any previous data
        if float(serial.VERSION[0:3]) < 3:
            #If we're executing with pySerial 2.x
            serial.Serial.flushInput(self.root.ser)
            serial.Serial.flushOutput(self.root.ser)
        else:
            #Otherwise we're using pySerial 3.x
            serial.Serial.reset_input_buffer(self.root.ser)
            serial.Serial.reset_output_buffer(self.root.ser)
            
            
    def close_prog(self):
        #Close the serial port
        self.root.ser.close()
        
        #Close the log file if it's open
        if self.root.variables['log2file'] == 'on': 
            if not self.root.logfile:
                self.root.logfile.write(self.templog)
                self.root.logfile.close()
            
        #Destroy the root
        self.root.destroy()

class MAVStatus(ttk.LabelFrame):
    def __init__(self, parent, root,**kw):
        ttk.LabelFrame.__init__(self, parent,**kw)
        self.parent = parent
        self.root = root  
        #timelb = tk.Label(self,textvariable = str(self.root.rsa_time))
        lf1 = Event(self,self.root,text = 'Event',)
        lf2 = Data(self,self.root,text = 'Data')
        lf3 = Error(self,self.root,text = 'Error')
        #timelb.grid(row = 0,column = 0,sticky = 'n',padx = 7,pady = 10)
        lf1.grid(row = 1,column = 0,sticky = 'n',padx = 7,pady = 10)
        lf2.grid(row = 2,column = 0,sticky = 'n',padx = 7,pady = 10)
        lf3.grid(row = 3,column = 0,sticky = 'n',padx = 7,pady = 10)
        
        
class Event(ttk.LabelFrame):
    def __init__(self, parent, root,**kw):
        ttk.LabelFrame.__init__(self, parent,**kw)
        self.parent = parent
        self.root = root
        
        self.new_event_list = tk.StringVar(self,value = 'Standby')
        self.old_event_list = tk.StringVar(self,value = 'Standby')
        self.l1 = tk.LabelFrame(self,text = 'Current',width = 70)
        self.l2 = tk.LabelFrame(self,text = 'Past',width = 70)
        self.l1.pack()
        self.l2.pack()
        self.m1 = tk.Label(self.l1,textvariable = self.new_event_list)
        self.m2 = tk.Label(self.l2,textvariable = self.old_event_list)
        self.m1.pack()
        self.m2.pack()
        
        self.root.rsa_event.trace('w',self.eventcallback)
        self.cc = 0
        
    def eventcallback(self,*arg):
        self.evnentDataHandle(self.root.rsa_event.get())
    def evnentDataHandle(self,number):
        if(self.cc == 0):
            self.old_event = self.new_event = []
            self.old_event_number = self.new_event_number = 0
            self.cc = self.cc+1

        self.old_event_number = self.new_event_number
        
        self.old_event = self.new_event
        self.new_event_number = number
        
        diff = self.old_event_number^self.new_event_number
        for bit in range(16):
            a = diff & pow(2,bit)
            if(a != 0):
                self.new_event = []
                self.new_event.append(dictOp(cfg_event,a))
        event =self.new_event
        self.new_event_list.set(self.new_event)
        self.old_event_list.set(self.old_event)
        if len(self.new_event) > 1:
            self.root.rsa_error.set('Event Queue Wrong')
class Data(ttk.LabelFrame):
    def __init__(self, parent, root,**kw):
        ttk.LabelFrame.__init__(self, parent,**kw)
        self.parent = parent
        self.root = root
        self.l1 = tk.LabelFrame(self,text = 'Task Numeber')
        self.l1.pack(pady = 6,padx = 5)
        self.l2 = tk.Label(self.l1,textvariable =self.root.rsa_params[3])
        self.l2.pack(fill = 'both')
        self.pidlb = tk.LabelFrame(self,text = 'pid params')
        self.pidlb.pack(pady = 6,padx = 5)
        self.n1 = tk.Label(self.pidlb,textvariable = self.root.rsa_params[0])
        self.n2 = tk.Label(self.pidlb,textvariable = self.root.rsa_params[1])
        self.n3 = tk.Label(self.pidlb,textvariable = self.root.rsa_params[2])
        self.n1.grid(row = 0,padx = 5,pady = 5)
        self.n2.grid(row = 0,column  = 1,padx = 5,pady = 5)
        self.n3.grid(row = 0,column  = 2,padx = 5,pady = 5)
        

class Error(ttk.LabelFrame):
    def __init__(self, parent, root,**kw):
        ttk.LabelFrame.__init__(self, parent,**kw)
        self.count = 0
        self.parent = parent
        self.root = root     
        self.root.rsa_error.trace('w',self.labelBlink)
        self.l2 = tk.Label(self,textvariable = self.root.rsa_error)
        self.l2.pack(fill ='both')
        self.l2.bind('<Button-1>',self.reseeet)
        
    def labelBlink(self,*arg):
        
        self.l2.config(background = 'red')
        if self.count < 2:
            self.timer = Timer(0.1,self.labelBlink1)
            self.timer.start()
        self.count = self.count+1
    def labelBlink1(self,*arg):
        
        self.l2.config(background = 'yellow')
        self.timer = Timer(0.1,self.labelBlink)
        self.timer.start()
    def reseeet(self,*arg):
        self.l2.config(background = 'white')



class StatusBar(ttk.Frame):
    def __init__(self, parent, root,**kw):
        ttk.Frame.__init__(self, parent,**kw)
        self.parent = parent
        self.root = root
        self['borderwidth'] = 2
        self['relief'] = 'sunken'
        
        COMlabel = ttk.Label(self, text= \
            self.root.variables['COMport'][0:5].strip() + ':')
        baudLabel = ttk.Label(self, text= \
            str(self.root.variables['baud'].strip()))
        COMlabel.pack(side='left', padx=0)
        baudLabel.pack(side='left', padx=0) 
 
        ttk.Separator(self, orient='vertical').pack(side='left', fill='y', padx=5)       
        
        buffLabel = ttk.Label(self, text='Serial Buffer:')
        buffLabel.pack(side='left', padx=0)
        
        buffBar = ttk.Progressbar(self, orient='horizontal', length=50,\
            mode='determinate', variable=self.root.variables['buffsize'],\
            maximum=io.DEFAULT_BUFFER_SIZE)
        buffBar.pack(side='left')
        
        ttk.Separator(self, orient='vertical').pack(side='left', fill='y', padx=5)
        
        
        
        updateLabel = ttk.Label(self, text='Data Recieved at: ')
        updateLabel.pack(side='left')
        updateRate = ttk.Label(self, textvariable=self.root.variables['refreshrate'])
        updateRate.pack(side='left')
        ttk.Label(self, text='Hz (Est)').pack(side='left')
        
        ttk.Separator(self, orient='vertical').pack(side='left', fill='y', padx=5)  
             
        
        package_errorLabel = ttk.Label(self, text='Package Error:')
        package_errorLabel.pack(side='left')
        package_error = ttk.Label(self, textvariable=self.root.package_error)
        package_error.pack(side='left')
        if self.root.variables['log2file'] == 'on':
            self.root.toggleLogButton = ttk.Button(self, text='Turn Logging Off', command = self.toggleLog)
            self.root.toggleLogButton.pack(side='left')
        
    def toggleLog(self):
        if self.root.variables['log2file'] == 'on':
            #If it's on, turn it off and write a \n to seperate different logs
            self.root.variables['log2file'] = 'off'
            self.root.logfile = open(self.root.variables['filename'], 'a')
            self.root.logfile.write('\n\n')
            self.root.logfile.close()       
            self.root.toggleLogButton['text'] = 'Turn Logging On'            
        else:
            self.root.variables['log2file'] = 'on'
            self.root.logfile = open(self.root.variables['filename'], 'a')
            self.root.toggleLogButton['text'] = 'Turn Logging Off'            
        
    def debugbutton(self):       
        width = 8
        msg = ''        
        for row in range(len(self.root.data[0])):
            for column in range(len(self.root.data)):
                element = str(self.root.data[column][row])
                if len(element) < width:
                    element += ' ' * (width - len(element))
                msg += element
                if column == len(self.root.data) - 1:
                    msg += '\n'
        #messagebox.showinfo(message=msg)
        #messagebox.showinfo(message=str(serial.Serial.inWaiting(self.root.ser)))
        #messagebox.showinfo(message=self.root.ani)
        


class RSA_STATUS:
    def __init__(self,root):
        self.old_pkg_count = 0      
        self.pkg_cnt = 0
        self.systime = 0
        self.systime_old = 0
        self.root = root
    def setStream(self,serial_stream):
        self.stream = serial_stream
        try:
            self.__spacepos = self.stream.find(32)
            self.systime_raw = self.stream[0:self.__spacepos]

            self.systime_old = self.systime
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
                if self.data_raw.decode('ascii').find(',') == -1 :#数据中没有‘，’
                    self.data = float(self.data_raw.decode('ascii'))
                else:
                    self.data = self.data_raw.decode('ascii').split(',')
                    for r in range(len(self.data)):
                        self.data[r] = float(self.data[r])
        except Exception as e:
            #self.root.package_error.set("something wrong")
            pass
    def check(self):
        if(self.old_pkg_count == self.pkg_cnt) or (self.old_pkg_count == self.pkg_cnt-254):
            return True
        else:
            return False

    def getInterval(self):
        return  self.systime -self.systime_old

    def getData(self):
        return self.data
    def getType(self):
        return self.type
    def getTime(self):
        return  self.systime

#If this script is executed, just run the main script


class Graph(ttk.Frame):

    def __init__(self, parent, root,**kw):
        self.cccount = 2
        ttk.Frame.__init__(self, parent,**kw)
        self.parent = parent
        self.root = root                
        #Create the figure object to put all the axes in        
        self.root.f = Figure(facecolor='white')
        
        #Create an empty dictionary to store the axis handles in
        self.root.ax = {}
        numgraphs = int(self.root.variables['numgraphs'])
        
        #Based on the number of graphs we want, create the axis objects
        for ax in range(1,numgraphs + 1):
            #This creates a dictionary, with the keys being character axis
            #number (ie '1', '2', etc)
        
            key = 'g'+str(ax)+'ylims'
            timelength = int(self.root.variables['timelength'])
            
            for lim in range(len(self.root.variables[key])):
                    self.root.variables[key][lim] = int(self.root.variables[key][lim])
                    
            lims = self.root.variables[key]
            
            self.root.ax[str(ax)] = self.root.f.add_subplot(numgraphs, 1, ax)
            self.root.ax[str(ax)].set_ylim(lims)
            self.root.ax[str(ax)].set_xlim([-timelength, 0])
            self.root.ax[str(ax)].grid(True)
            self.root.ax[str(ax)].tick_params(axis='both', labelsize=16)
            
        #Create an empty list to store line objects in. The position in the list
        #is also the line number
        self.root.lines = []
        
        #Create an empty list to store the data position for each line
        self.root.lineDataPos = []

        #Create a list of multiplier and offsets, with position corrilating to 
        #the position of the data it will be applied to. e.g. [data1, data2]
        #would have [[multiplier1, offset,], [multiplier2, offset2]]
        self.root.dataMultOff = []
        
        #Create a list to store all the data
        self.root.data = []
        for column in range(0, int(self.root.variables['datalength'])):
            self.root.data.append([])
            #Default to a multiplier of 1, offset of zero
            self.root.dataMultOff.append([1, 0])
        #Set up the multiplier and offsets
        for data in range(len(self.root.data)):
            #Find if a line uses this data
            for key in self.root.variables:
                #If it does, set the position in the dataMultOff equal to
                #The multipliers in the variable       
                if key[0:5] == 'graph' and cfg_line_to_data[self.root.variables[key][1]] != '-':
                    position = int(cfg_line_to_data[self.root.variables[key][1]]) - 1
                else:
                    position = -1
                    
                if position == data:
                    multiplier = float(self.root.variables[key][4])
                    offset = float(self.root.variables[key][5])
                    datalbl = self.root.variables[key][0]
                    self.root.dataMultOff[data] = [multiplier, offset, datalbl]
            
        #Sort they keys, so we are always ascending
        keylst = sorted(list(self.root.variables.keys()))
        
        #Run through the variables and add only the lines and datapositions
        #we're using
        for key in keylst:
            if (key[0:5] == 'graph') and (cfg_line_to_data[self.root.variables[key][1]] != '-'):
                #If the key is graph info AND there is a data position there, store it
                #in the arrays we created above
                graph = int(key[5])
                #linenum = int(key[10])
                datapos = int(cfg_line_to_data[self.root.variables[key][1]])   
                
                #Add a line only if we have this number of graphs
                if int(self.root.variables['numgraphs']) >= graph:                   
                    self.root.lineDataPos.append(datapos)
                
                    #Add an empty line to the graph
                    l, = self.root.ax[str(graph)].plot([], [],\
                        color=      self.root.variables[key][2],
                        linestyle=  self.root.variables[key][3],
                        label=      self.root.variables[key][0],)
                    self.root.lines.append(l)   

        #Create the legends
        for ax in range(1,numgraphs + 1):
            handles, labels = self.root.ax[str(ax)].get_legend_handles_labels()
            self.root.ax[str(ax)].legend(handles=handles, loc='upper left')
            
        #Configure and open the serial port
        
        self.currenttime = 0
        self.delta_currenttime = 0
        #TODO: For some unknown reason, when running serialplot.py on windows
        #with python3, the program freezes at canvas.show(). However,
        #(This is the weird part) if the graphwindow.py or configwindow.py is
        #run first, which in turn calls serialplot.py, there is no issue and it
        #runs fine. Odd.
   
        #Create the initial axis objects and show the figure
        canvas = FigureCanvasTkAgg(self.root.f, master=self)  
        canvas.show()
        canvas.get_tk_widget().pack(side='top', fill='both', expand=True)
      
        #Start the timer for estimated update frequency
            

        
                    
            #write headers so we know what the data is

        #Set up the graph update routine
        updatefreq = float(self.root.variables['refreshfreq']) #Hz 
        interval = int((1/updatefreq)*1000)
        self.root.graph_data_ready =0
        self.root.linenumber.trace('w',self.updataGraph_xy)
        
        try:
            self.root.ani = animation.FuncAnimation(self.root.f, self.updateGraph,\
                init_func=self.init_func, interval=interval, blit=True)
        except:
            messagebox.showerror(message='Issue starting animation')

        #Apply tight layout to reduce wasted space        
        self.root.f.tight_layout()

        #If this line isn't in here the graph shows up choppy for some reason
        #TODO Figure out why this shows up so choppy unless the window is resized 
        #This line fixed the problem previously, but after updating matplotlib
        #It ceased to fix it
        self.root.f.canvas.show()        
        
        
    
    def init_func(self):
        for line in self.root.lines:
            line.set_data([], [])     
        return self.root.lines

    
    def updataGraph_xy(self,*arg):
        
        l_num = self.root.linenumber.get()
        if l_num == 5 or l_num == 6:
            pass
        else:
            self.currenttime_past = self.currenttime
            self.currenttime = self.root.time_for_graph.get()
            self.delta_currenttime = self.currenttime - self.currenttime_past
        
        for k in range(len(self.root.line_x[l_num])):
            self.root.line_x[l_num][k] = self.root.line_x[l_num][k] - self.delta_currenttime
            
        self.root.line_x[l_num].append(0)
        self.root.line_y[l_num].append(self.root.graphstr_y.get())

        #temp1 = self.root.line_x[l_num]
        #temp2 = self.root.line_y[l_num]
        temp3 = len(self.root.line_x[l_num])
        for k in range(temp3):
            
            if(self.root.line_x[l_num][temp3-k-1] < -int(self.root.variables['timelength'])): #最早的点与现在的点相距大于8s则删掉大于8s的点
                self.root.line_x[l_num] = self.root.line_x[l_num][temp3-k-1:]
                self.root.line_y[l_num] = self.root.line_y[l_num][temp3-k-1:]
                break
        #self.root.line_x[l_num] = temp1
        #self.root.line_y[l_num] = temp2
        
    

    
        
    
    
    def updateGraph(self, frameNum, *args, **kwargs):
        
        if self.root.graph_data_ready == 1:
            for line in range(len(self.root.lines)):
                for i in range(len(self.root.line_x)):
                    if(self.root.lineDataPos[line]-1 == i):
                        ydata = self.root.line_y[i]
                        xdata = self.root.line_x[i]
                        self.root.lines[line].set_data(xdata, ydata)
            self.root.graph_data_ready = 0
        
        #xdata = [0,-1000,-2000,-3000,-4000]
        #ydata = [0,-5,5,-3,4]
        return self.root.lines

if __name__ == '__main__':
    os.system("serialplot.py")
