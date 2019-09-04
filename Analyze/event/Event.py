#!/usr/bin/env python
# -*- coding: utf-8 -*-
# @Time    : 2019/9/2 11:54 上午
# @Author  : kewen
# @File    : Event.py


class Event:
    """
    对应日志中的一行事件，保存事件所有信息
    """

    def __init__(self):
        self.eventName = ""
        self.timestamp = 0
