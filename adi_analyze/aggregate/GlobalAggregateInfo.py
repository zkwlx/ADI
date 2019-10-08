#!/usr/bin/env python
# -*- coding: utf-8 -*-
# @Time    : 2019/9/23 2:08 下午
# @Author  : kewen
# @File    : GlobalAggregateInfo.py


class GlobalAggregateInfo:
    """
    一份日志的全局信息，和日志文件是一对一关系
    """

    def __init__(self):
        # 日志文件名
        self.fileName = ""
        # 日志路径
        self.filePath = ""
        # 日志开始事件时间戳
        self.startTimestamp = 0
        # 日志最后事件的时间戳
        self.endTimestamp = 0
        # 日志事件的总时长
        self.totalTime = 0
