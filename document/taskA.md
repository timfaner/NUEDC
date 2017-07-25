#基本任务 一 数据包
##包顺序
	[error_flag,mav_statu,x,y,land_flag]
##各参数含义
###`error_flag`: 错误状态flag  
应该检查这个参数是否为0 不为零的话 先暂留吧  
###`mav_stastu`: 飞机现在应该处在的飞行状态  
摄像头判断出来的此时的飞行状态 值3的时候相当于是降落指令
###`x`:不同`mav_statu`值输出对应目标在屏幕中的x值  
###`y`:不同`mav_statu`值输出对应目标在屏幕中的y值            
##各参数取值
`error_flag` 0:正常 1:处于未知的位置  2: 读不到色  3:飞行顺序出错  
`mav_stastu` 0:初始状态 1:起飞 `takeoff` 2:飞行中/往前飞 `flying` 3:降落 `preland` 4:飞过/往后飞 `overfly` 255:出错 `error` 
`x`:   `mav_statu == 2` 巡线时线的x坐标   `mav_statu == 3` 降落时点的x坐标    `mav_statu为其他` 该数据无用  
`y`: `mav_statu == 3`   降落时原点的x坐标 `mav_statu为其他` 该数据无用