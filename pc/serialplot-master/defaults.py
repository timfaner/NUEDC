'''
Be careful when modifying these values - if they aren't set correctly, 
the program won't run. As a precaution if you modify it, it's a good idea to 
save a copy first. serialplot just looks for a file in the same directory 
called 'defaults.py'
 
The format for graphXlineX properties is:
[datalabel,
datapos,
linecolor,
dashed,
multiplier,
offset]
'''

defaults = {'COMport': '',
 'baud': 115200,
 'databits': 8,
 'timelength': 5000,
 'datalength': 7,
 'filename': 'C:/Users/Victor/Desktop/RsaLog.csv',
 'g1ylims': [-10, 10],
 'g2ylims': [-10, 10],
 'g3ylims': [-10, 10],
 'graph1line1': ['line1', 'PID_X', '#FF0000', '-', 1, 0],
 'graph1line2': ['line2', 'PID_Y', '#ff0000', '-', 1, 0],
 'graph1line3': ['line3', '-', '#00FF00', '-', 1, 0],
 'graph2line1': ['line1', 'MV_X', '#0000FF', '-', 1, 0],
 'graph2line2': ['line2', 'MV_Y', '#0000FF', '-', 1, 0],
 'graph2line3': ['line3', '-', '#00FF00', '-', 1, 0],
 'graph3line1': ['line1', 'Pitch', '#FF0000', '-', 1, 0],
 'graph3line2': ['line2', 'Yaw', '#0000FF', '-', 1, 0],
 'graph3line3': ['line3', 'Row', '#00FF00', '-', 1, 0],
 'log2file': 'off',
 'maxlength': 500,
 'numgraphs': 1,
 'parity': 'None',
 'refreshfreq': 20,
 'startmax': 'no',
 'stopbits': 1,
 'stsbrwdth': 7,
 'timeout':0.1}


