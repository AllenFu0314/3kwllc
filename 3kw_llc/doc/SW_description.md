一.可调参数详解
	1.输出电压调节
		LLC_VOUT_CMD	默认：2.4		对应2.4*22.5 = 54V			备注：LLC最终输出电压调节     
		LLC_B0			默认：0.5   	    							备注：电压环PI调节器的B0参数
		LLC_B1          默认：-0.485									备注：电压环PI调节器的B1参数

	2.LLC频率调节
		LLC_PWM_MAX_FREQ  	默认:10245    	对应400khz					备注:LLC最高频率
		LLC_PWM_MINMUM_FREQ 默认:20490		对应200khz					备注:LLC最低频率
		RESONANT_FREQ       默认:14631		对应280khz					备注:LLC谐振频率

	3.保护参数
		OVP_VALUE          默认2.7          对应2.7*22.5 = 60.75V       	备注:过压保护参数
		OCP_VALUE		   默认2.2          对应2.2*40 = 88A				备注：过流保护参数
		注：OVP_REC_VALUE  触发过压保护后，低于这个值可自动恢复，过流同理

	4.死区时间
		DT_RISING			默认103         对应100ns					备注：LLC上升沿死区时间
		DT_FALLING			默认103		   	对应100ns					备注：LLC下降沿死区时间
		SYNC_REFC_OFFTIME   默认40			对应10ns						备注：同步整流驱动比LLC驱动提前关的时间，LLC关断前10ns																	  关闭同步整流驱动
		SYNC_REFC_ONTIME    默认40			对应10ns						备注：同步整流驱动比LLC驱动延后开的时间，LLC开启后10ns
																			  打开同步整流驱动


二.程序设置
	1.PGOODS：1/0 默认为：1
		描述：如果设置为1，那么当GPIOA3拉高后（源边输入电压正常），LLC驱动才会出来，运行过程中，GPIOA3拉低，LLC和同步整流驱动会关闭
			  如果设置为0 那么上电后LLC驱动和同步整流驱动就会发出

	2.UART_START_ENABLE：1/0 默认为:0
		描述：如果设置为1，那么当上位机发送"openllc"命令后，LLC才会发出驱动
			  如果设置为0，无需上位机的"openllc"命令。

	3.OPEN_LOOP_SS：1/0 默认为:0
		描述：设置为1，开环软启动（不推荐）
			  设置为0，闭环软启动

	4.REPETITION_RATE ：0-256  默认为：5
		描述：控制中断频率,PWM开关频率/REPETITION_RATE+1(不推荐更改)

三.程序文件分布
	1.参数调节文件：globaldefine.h
	2.外设设置和main函数:main.c
	3.程序控制算法:stm32f3xx_it.c