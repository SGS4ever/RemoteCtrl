/*
* File: Config.h
* Description: This file includes some configuration options
				of the main project
* Author: xr_g
* LastModified: 2020-12-25
*/

#pragma once
#ifndef CONFIG_H
#define CONFIG_H

#define WORKMODE 2					// 工作模式
									//（1：被动连接；2：主动连接）

#define AUTOSTART 0					// 是否添加启动项
									//（0：不添加；1：添加）

#define HIDE_CONSOLE 0				// 是否隐藏控制台窗口
									//（0：不隐藏；1：隐藏）

#define TARGET_IP "192.168.0.102"	// 主动连接模式下的目标IP

#define BUFFER_SIZE 10000			// socket接收缓冲区大小

#define CASCADE_DELETE false		// 删除文件时是否级联删除

#endif