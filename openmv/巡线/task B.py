
import sensor, image, math,pyb,ustruct,utime
from pyb import  SPI

spi = SPI(2, SPI.MASTER, baudrate=int(1000000000/66), polarity=0, phase=0,bits=32)


x = 1


GRAYSCALE_THRESHOLD = [(0, 140)]

#计算两直线交点
def callinePoint(x1 = 1,y1 = 1,x2 = 1,y2 = 1,theta = 1):
    if theta ==0: theta+=1
    x = (math.tan(theta)*x1 - 1/math.tan(theta)*x2 +y2 - y1)/(math.tan(theta) - 1/math.tan(theta))
    y = (math.tan(theta)*y2 - 1/math.tan(theta)*y1 +x1 - x2)/(math.tan(theta) - 1/math.tan(theta))
    return(x,y);

def isoneof(statu,*s):
    sr = 0
    for r in s:
                if statu == r: sr = 1
    if sr ==1:
        return True;
    else:
        return False;

def ismatchwith(a,b,c,d):
    if a == c and b == d:
        return True;
    else: return False;



ROIS = [(0,0,10,120),\
        (10,0,140,10),\
        (150,0,10,120),\
        (10,110,140,10)]
# Compute the weight divisor (we're computing this so you don't have to make weights add to 1).
weight_sum = 0
#for r in ROIS: weight_sum += r[4] # r[4] is the roi weight.

# Camera setup...
sensor.reset() # Initialize the camera sensor.
sensor.set_pixformat(sensor.GRAYSCALE) # use grayscale.
sensor.set_framesize(sensor.QQVGA) # use QQVGA for speed.
sensor.skip_frames(time = 2000) # Let new settings take affect.

sensor.set_auto_gain(False) # must be turned off for color tracking
sensor.set_auto_whitebal(False) # must be turned off for color tracking
sensor.set_auto_exposure(False)



mav_statu = 0
old_mav_statu = 0
error_flag = -1
statu_number = 0
while(True):
    i = 0
    roi_list = [8, 4, 2, 1]


    img = sensor.snapshot() # Take a picture and return the image.

    for r in ROIS:
        blobs = img.find_blobs(GRAYSCALE_THRESHOLD, roi=r[0:4], merge=True) # r[0:4] is roi tuple.
        if blobs:
            # Find the blob with the most pixels.
            largest_blob = max(blobs, key=lambda b: b.pixels())
            for r in blobs:
                img.draw_rectangle(r.rect())
                img.draw_cross(r.cx(),
                           r.cy())
        i+=1


    statu_number = 0
    for r in roi_list:
        if isinstance(r,int):statu_number = statu_number|r


    #0:初始状态 1:起飞  2:飞行中/往前飞  3:降落  4:飞过/往后飞 －1:出错

    old_mav_statu = mav_statu

    if isoneof(statu_number,0b11000,0b11100,0b01100,0b10000,0b01000):
        mav_statu = 1
    elif isoneof(statu_number,0b00000):
        mav_statu = 2
    elif isoneof(statu_number,0b00001,0b00011,0b00111,0b00110):
        mav_statu = 3
    elif isoneof(statu_number,0b01110,0b01111,0b01101,0b01100):
        mav_statu = 4
    else:
        mav_statu = -1

    error_statu = 0 #将错误状态置零

