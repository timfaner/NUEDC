
import sensor, image, math, pyb, ustruct, utime
from pyb import  SPI, Pin,LED

pin_start = Pin('P4', Pin.IN, Pin.PULL_DOWN)
pin_task1 = Pin('P5', Pin.IN, Pin.PULL_DOWN)
pin_task2 = Pin('P6', Pin.IN, Pin.PULL_DOWN)

spi = SPI(2, SPI.MASTER, baudrate=int(1000000000/66), polarity=0, phase=0,bits=32)
x = 1
GRAYSCALE_THRESHOLD = [(0, 120)]


red_led   = LED(1)
green_led = LED(2)
blue_led  = LED(3)
ir_led    = LED(4)

def led_control(x):
    if   (x&1)==0: red_led.off()
    elif (x&1)==1: red_led.on()
    if   (x&2)==0: green_led.off()
    elif (x&2)==2: green_led.on()
    if   (x&4)==0: blue_led.off()
    elif (x&4)==4: blue_led.on()
    if   (x&8)==0: ir_led.off()
    elif (x&8)==8: ir_led.on()
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

def isaround(x,y):
    if abs(x - 80)<4 and abs(y-60) < 4:
        return True
    else:return False

def sendpackage(*arg):
    Arg = [int(x) for x in arg]
    while(len(Arg)) < 6:
        Arg.append(0)
    check_bit = Arg[0]
    for data in Arg[1:]:
        check_bit = data ^ check_bit
    spi.send(0XD3)
    utime.sleep_us(50)
    for data in Arg:
        spi.send(ustruct.pack("<B",data))
        utime.sleep_us(50)
    spi.send(check_bit)
    utime.sleep_us(50)
    spi.send(0xC8)
    utime.sleep_us(10)


ROIS = [ # [ROI, weight]
        (0, 100, 160, 20, 0.7),
        (0,  75, 160, 20, 0.3),
        (0,  50, 160, 20, 0.1),
        (0,  25, 160, 20, 0.1),
        (0,  0 , 160, 20, 0.1)
       ]

# Compute the weight divisor (we're computing this so you don't have to make weights add to 1).
weight_sum = 0




sensor.reset() # Initialize the camera sensor.
sensor.set_pixformat(sensor.GRAYSCALE) # use grayscale.
sensor.set_framesize(sensor.QQVGA) # use QQVGA for speed.
sensor.skip_frames(time = 2000) # Let new settings take affect.

sensor.set_auto_gain(False) # must be turned off for color tracking
sensor.set_auto_whitebal(False) # must be turned off for color tracking
sensor.set_auto_exposure(False)

while not pin_start.value():
    utime.sleep_ms(1)
    led_control(1)

led_control(0)
#print('Begin')



mav_statu = 0
old_mav_statu = 0
error_flag = 255
statu_number = 0
land_flag = 0

led_x = 0
while(True):
    i = 0
    roi_list = [16 ,8, 4, 2, 1]


    img = sensor.snapshot() # Take a picture and return the image.

    for r in ROIS:
        blobs = img.find_blobs(GRAYSCALE_THRESHOLD, roi=r[0:4], merge=True) # r[0:4] is roi tuple.
        if blobs:
            # Find the blob with the most pixels.
            largest_blob = max(blobs, key=lambda b: b.pixels())
            roi_list[i] = largest_blob
            img.draw_rectangle(largest_blob.rect())
            img.draw_cross(largest_blob.cx(),
                           largest_blob.cy())
        i+=1


    statu_number = 0
    for r in roi_list:
        if isinstance(r,int):statu_number = statu_number|r


    #0:初始状态 1:起飞  2:飞行中/往前飞  3:降落  4:飞过/往后飞 255:出错

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
        mav_statu = 255

    error_statu = 0 #将错误状态置零

    #状态转换判断
    start = pyb.millis()
    if ismatchwith(old_mav_statu,mav_statu,1,2) \
    or ismatchwith(old_mav_statu,mav_statu,2,4) \
    or ismatchwith(old_mav_statu,mav_statu,4,3) \
    or ismatchwith(old_mav_statu,mav_statu,2,3) \
    or ismatchwith(old_mav_statu,mav_statu,2,1) \
    or old_mav_statu == 0 and mav_statu != 0 \
    or ismatchwith(old_mav_statu,mav_statu,1,1) \
    or ismatchwith(old_mav_statu,mav_statu,2,2) \
    or ismatchwith(old_mav_statu,mav_statu,3,3) \
    or ismatchwith(old_mav_statu,mav_statu,4,4):error_flag = 0

    elif ismatchwith(old_mav_statu,mav_statu,0,0): error_flag = 2

    elif old_mav_statu != 255 and mav_statu == 255:       #进入错误状态
        start = pyb.millis()
        error_statu = 1
        error_flag = 0
    elif ismatchwith(old_mav_statu,mav_statu,255,255):      #保持错误状态 并在保持一段时间后触发错误flag
        if(pyb.elapsed_millis(start) >1000) and error_statu == 1:
            error_flag = 1
        error_statu = 1
    elif old_mav_statu == 255 and mav_statu != 255:     #退出错误状态
        error_statu = 0
        error_flag = 0
    else:
        error_flag = 3


    #根据状态判断并输出
    #error_flag :0:正常    1:处于位置状态 2: 读不到色（怕不是曝光有问题） 3:状态转换出错
    #mav_stastus:0:初始状态 1:起飞  2:飞行中/往前飞  3:降落  4:飞过/往后飞 255:出错
    output = [error_flag,mav_statu,80,60,land_flag]

    #{x:, y:, w:, h:, pixels:, cx:, cy:, rotation:, code:1, count:1}


    if(error_flag == 0):
        if mav_statu == 1:
            output[2] = roi_list[3][5]
        elif mav_statu == 2:
           output[2] = roi_list[1][5]
        elif mav_statu == 3:
            if isoneof(statu_number,0b00011):
                if roi_list[2][0] >= roi_list[1][0]:
                    x2 = roi_list[1][5]
                    y2 = roi_list[1][6]
                else:
                    x2 = roi_list[2][5]
                    y2 = roi_list[2][6]
            elif isoneof(statu_number,0b00001):
                if roi_list[2][0] >= roi_list[3][0]:
                    x2 = roi_list[3][5]
                    y2 = roi_list[3][6]
                else :
                    x2 = roi_list[2][5]
                    y2 = roi_list[2][6]
            elif isoneof(statu_number,0b00111,0b00110):
                x2 = roi_list[1][5]
                y2 = roi_list[1][6]
            x1 = roi_list[0][5]
            y1 = roi_list[0][6]
            roi_theta = roi_list[0][7]
            land_point = callinePoint(x1,y1,x2,y2,roi_theta)

            img.draw_cross(int(land_point[0]),int(land_point[1]),size = 20)
            output[2] = int(land_point[0])
            output[3] = int(land_point[1])

        elif mav_statu == 4:
            pass

    if mav_statu == 3 and isaround(output[2],output[3]):
        land_flag = 1


    print(output)
    sendpackage(*output)

    led_control(led_x)
    led_x = led_x+1
    i = 0






