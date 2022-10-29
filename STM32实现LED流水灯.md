> #         **重庆交通大学信息科学与工程学院**
>
> #             **《嵌入式系统开发》课程**
>
> #                      作业报告（第四周）

**班 级： <span class="underline"> 通信工程2001</span>**

**姓名-学号 ： <span class="underline"> 郭新宇-632007030611 </span>**

**实验项目名称： <span class="underline"> 用寄存器&HAL库完成LED流水灯程序 </span>**

**实验项目性质： <span class="underline"> 设计性 </span>**

**实验所属课程： <span class="underline">《嵌入式系统开发》 </span>**

**实验室(中心)： <span class="underline"> 南岸校区语音大楼 </span>**

**指 导 教 师 ： <span class="underline">娄路 </span>**

**完成时间： <span class="underline"> 2022</span> 年 <span class="underline"> 10</span> 月 <span class="underline"> 12</span> 日**

------

<div STYLE="page-break-after: always;"></div>

<div STYLE="page-break-after: always;"></div>

**一、实验内容和任务**

用寄存器&HAL库完成LED流水灯程序

**二、实验要求**

1\. 分组要求：每个学生独立完成，即1人1组。

2\. 程序及报告文档要求：具有较好的可读性，如叙述准确、标注明确、截图清晰等。

3.项目代码上传github，同时把项目完整打包为zip文件，与实验报告（Markdown源码及PDF文件）、作业博客地址一起提交到学习通。

三. **实验过程介绍** 

@[TOC]

# 一、实验原理
## 1、STM32F103C8T6简介
STM32F103C8T6是一款由意法半导体公司（ST）推出的基于Cortex-M3内核的32位微控制器，硬件采用LQFP48封装，属于ST公司微控制器中的STM32系列。
![在这里插入图片描述](https://img-blog.csdnimg.cn/8b9079f7e53c43b681dc5b20ad898ecc.png)
## 2、地址映射
1.M3存储器映射
![在这里插入图片描述](https://img-blog.csdnimg.cn/64e4305979c846cc90bf0c80b6613df2.png)
LED灯程序中，宏定义：

```
#define GPIOC_BASE (APB2PERIPH_BASE + 0x1000)
#define APB2PERIPH_BASE (PERIPH_BASE + 0x10000)
#define PERIPH_BASE ((uint32_t)0x40000000)
```
注：PERIPH_BASE 外设基地址：因为stm32是32位的，宏展开为0x40000000并转化为 uint32_t
APB2PERIPH_BASE 总线基地址：宏展开为PERIPH_BASE加上偏移地址 0x10000
2.寄存器寻址
GPIOB基址：
GPIOB相关的寄存器，都住在0x4001 0C00到0x4001 0FFF范围内。
![在这里插入图片描述](https://img-blog.csdnimg.cn/7756defeed8b4375971a581723d258be.png)
端口输入寄存器地址偏移：
存储数据的位置：0X40010C00+0X0x =
地址为： GPIOC_BASE +0x0x
![在这里插入图片描述](https://img-blog.csdnimg.cn/9b00764108564cdd95f13cf68b6c49c1.png)
数据
![在这里插入图片描述](https://img-blog.csdnimg.cn/29eabb60c2704f48990ed23c0c7c6b26.png)
3.地址映射

```
GPIO_TypeDef * GPIOx; //定义一个 GPIO_TypeDef 型结构体指针 GPIOx
GPIOx = GPIOA; //把指针地址设置为宏 GPIOA 地址
GPIOx->CRL = 0xffffffff; //通过指针访问并修改 GPIOA_CRL 寄存器
```
## 3、寄存器映射
给已分配好地址(通过存储器映射实现)的有特定功能的内存单元取别名的过程就叫寄存器映射。
![在这里插入图片描述](https://img-blog.csdnimg.cn/07cfe17313a64791bf2554bc774ab2fb.png)
会有GPIOA->CRL=0x0000 0000这种写法，表示将16进制数0赋值给GPIOA的CRL寄存器所在的存储单元

```
#define PERIPH_BASE      ((uint32_t)0x40000000) 
```
1.这里属于存储器级别的映射，将外设基地址映射到0x40000000，可对应图2

```
#define APB2PERIPH_BASE       (PERIPH_BASE + 0x10000)
```
2.这里对外设基地址进行偏移量为0x10000的地址偏移，偏移到APB2总线对应外设区。

```
#define GPIOA_BASE            (APB2PERIPH_BASE + 0x0800)
```
3.这里对APB2外设基地址进行偏移量为0x0800的地址偏移，偏移到GPIOA对应区域。
## 4、GPIO端口初始化设置
### 1.时钟配置
本次实验采用GPIOA、B、C三个端口。该三个端口都属于APB2总线
1.找到时钟使能寄存器映射基地址
![在这里插入图片描述](https://img-blog.csdnimg.cn/51fa3caeef9f400d8f9d30c37ff9bd5d.png)
2.找到端口偏移地址以及对应端口所在位置
![在这里插入图片描述](https://img-blog.csdnimg.cn/c88962a85ae44af79b81b589f6ca2925.png)
3.使能对应端口时钟

```
//----------------APB2使能时钟寄存器 ---------------------
#define RCC_APB2ENR		*((unsigned volatile int*)0x40021018)

	RCC_APB2ENR|=1<<2|1<<3|1<<4;			//APB2-GPIOA、GPIOB、GPIOC外设时钟使能	
```
### 2.输入输出模式和输出速率设置
本次实验采用通用推挽输出模式，最高输出时钟频率2Mhz。分别用到A4、B5、C14三个引脚。其中A4、B5属于端口配置低寄存器偏移地址为0x00，C13属于端口配置高寄存器偏移地址为0x04。
![在这里插入图片描述](https://img-blog.csdnimg.cn/db7aadc9859047a985f94abb1218884d.png)
![在这里插入图片描述](https://img-blog.csdnimg.cn/190525c3237045f0aa468ee747909376.png)
1.找到GPIOx端口基地址
![在这里插入图片描述](https://img-blog.csdnimg.cn/d1e5b766cd454447a4eb834e7b24fe67.png)
2.配置对应引脚寄存器，基地址+偏移量
```
//----------------GPIOA配置寄存器 -----------------------
#define GPIOA_CRL		*((unsigned volatile int*)0x40010800)
//----------------GPIOB配置寄存器 -----------------------
#define GPIOB_CRL		*((unsigned volatile int*)0x40010C00)
//----------------GPIOC配置寄存器 -----------------------
#define GPIOC_CRH		*((unsigned volatile int*)0x40011004)
```
3.设置输出模式为推挽输出，输出速度为2Mhz
```
	GPIOA_CRL&=0xFFF0FFFF;		//设置位 清零	
	GPIOA_CRL|=0x00020000;		//PA4推挽输出，把第19、18、17、16位变为0010
	
	GPIOB_CRL&=0xFF0FFFFF;		//设置位 清零	
	GPIOB_CRL|=0x00200000;		//PB5推挽输出，把第23、22、21、20变为0010
	 
	GPIOC_CRH&=0xFF0FFFFF;		//设置位 清零	
	GPIOC_CRH|=0x00200000;		//PC14推挽输出，把第23、22、21、20变为0010
```
# 二、C语言实现LED流水灯
## 1.流水灯原理
本次实验采用三个灯实现，亮灯状态用1表示,灭灯状态用0表示。
初始状态为0 0 0，
状态一为1 0 0
状态二为0 1 0
状态三为0 0 1
状态三结束后继续进入状态一，一直循环达到流水灯效果。
## 2.前期准备
1.创建项目
![在这里插入图片描述](https://img-blog.csdnimg.cn/3edf422ad938433e954ebcd7db0d94b7.png)
2.选择STM32F103C8开发板
![在这里插入图片描述](https://img-blog.csdnimg.cn/f880e1cb6840449a98c662f09b789433.png)
注：创建项目出现弹窗，不勾选setup项，只勾选core项
![在这里插入图片描述](https://img-blog.csdnimg.cn/41cfdf5036404b94ae9064ab273fe2cd.png)
3.在output里选择create hex file
![在这里插入图片描述](https://img-blog.csdnimg.cn/d83f5c7c2404468f82ef7b81d3ee6654.png)
![在这里插入图片描述](https://img-blog.csdnimg.cn/88136d3f42f14676ace68ab457e09cd4.png)
添加驱动文件
![在这里插入图片描述](https://img-blog.csdnimg.cn/cf5653434c0141d0881cf0d49fe1534f.png)
拷贝到工程目录下
![在这里插入图片描述](https://img-blog.csdnimg.cn/71ce35f146414c61af5667d3c510bde6.png)
右击文件夹，选择Add Existing Files to Group Source Group 1

![在这里插入图片描述](https://img-blog.csdnimg.cn/d2136b560890442089d3b2b61b958e6b.png)
选择All FIles，选择刚刚添加的启动文件，Add，Add之后Close
![在这里插入图片描述](https://img-blog.csdnimg.cn/f15e11de23a344ac8dfbff5b8ef4b702.png)
4.线路连接
对于USB转TTL模块和stm32f103c8t6连接

![在这里插入图片描述](https://img-blog.csdnimg.cn/6c1d2e69a1e24673bb3819da898f3b1d.png)
![在这里插入图片描述](https://img-blog.csdnimg.cn/35029aa5a1ad44008785354f354ba2dc.png)
## 3.C语言实现
代码如下

```
#include "stm32f10x.h"
//----------------APB2使能时钟寄存器 ---------------------
#define RCC_APB2ENR		*((unsigned volatile int*)0x40021018)
//----------------GPIOA配置寄存器 -----------------------
#define GPIOA_CRL		*((unsigned volatile int*)0x40010800)
#define	GPIOA_ODR		*((unsigned volatile int*)0x4001080C)
//----------------GPIOB配置寄存器 -----------------------
#define GPIOB_CRL		*((unsigned volatile int*)0x40010C00)
#define	GPIOB_ODR		*((unsigned volatile int*)0x40010C0C)
//----------------GPIOC配置寄存器 -----------------------
#define GPIOC_CRH		*((unsigned volatile int*)0x40011004)
#define	GPIOC_ODR		*((unsigned volatile int*)0x4001100C)


//延时函数
 void Delay()
 {
   u32 i=0;
   for(;i<5000000;i++);
 }
 int main(void)
 {	
	RCC_APB2ENR|=1<<2|1<<3|1<<4;			//APB2-GPIOA、GPIOB、GPIOC外设时钟使能	
	
	GPIOA_CRL&=0xFFF0FFFF;		//设置位 清零	
	GPIOA_CRL|=0x00020000;		//PB5推挽输出
	GPIOA_ODR&=~(1<<4);			//设置初始灯为灭
	
	GPIOB_CRL&=0xFF0FFFFF;		//设置位 清零	
	GPIOB_CRL|=0x00200000;		//PB5推挽输出
	GPIOB_ODR&=~(1<<5);			//设置初始灯为灭
	 
	GPIOC_CRH&=0xF0FFFFFF;		//设置位 清零	
	GPIOC_CRH|=0x02000000;		//PB5推挽输出
	GPIOC_ODR&=~(1<<14);			//设置初始灯为灭		
	 

	 

	while(1){
		//A灯
		GPIOA_ODR|=1<<4;		//PB5高电平
	 	Delay();
		GPIOA_ODR&=~(1<<4);		//PB5低电平,因为是置0，所以用按位与


		
		//B灯
		GPIOB_ODR|=1<<5;		//PB5高电平
	 	Delay();
		GPIOB_ODR&=~(1<<5);		//PB5低电平,因为是置0，所以用按位与


		
		
		//C灯
		GPIOC_ODR|=1<<14;		//PB5高电平
	 	Delay();
		GPIOC_ODR&=~(1<<14);		//PB5低电平,因为是置0，所以用按位与


		
		}
}
```
## 4.烧录程序
生成hex文件
![在这里插入图片描述](https://img-blog.csdnimg.cn/9659970d329942e0908d720ad210c806.png)
![在这里插入图片描述](https://img-blog.csdnimg.cn/ee65307995ce413cb6da4231231ce686.png)
使用驱动进行烧录操作
连接到电脑，打开mcuisp，上传HEX文件到stm32f103c8t6上：
![在这里插入图片描述](https://img-blog.csdnimg.cn/c4982e8ae5eb4382a685241a7579932e.png)
# 三、效果
![在这里插入图片描述](https://img-blog.csdnimg.cn/cc33342f50eb457fb00e2b1cf84d2cc1.png)
![在这里插入图片描述](https://img-blog.csdnimg.cn/c5c9ec51a99045278accffce4a5a1760.png)
![在这里插入图片描述](https://img-blog.csdnimg.cn/daf483d7b13b422db252cc283bf891b8.png)
# 四、总结

通过本次实验，我学会了了如何对GPIO进行初始化（时钟配置、输入输出模式设置、最大速率设置），同时也学习和理解了STM32F103系列芯片的地址映射和寄存器映射原理。
# 五、参考资料
[stm32寄存器实现流水灯](https://blog.csdn.net/qq_47281915/article/details/120812867)
[stm32寄存器实现流水灯](https://blog.csdn.net/qq_47281915/article/details/120812867)
[STM32寄存器的简介、地址查找，与直接操作寄存器](https://blog.csdn.net/geek_monkey/article/details/86291377)
[STM32从地址到寄存器](https://blog.csdn.net/geek_monkey/article/details/86293880)



