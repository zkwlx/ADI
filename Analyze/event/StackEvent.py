#!/usr/bin/env python
# -*- coding: utf-8 -*-
# @Time    : 2019/9/2 11:54 上午
# @Author  : kewen
# @File    : Event.py
from event.Event import Event


class StackEvent(Event):
    """
    对应日志中的一行事件，保存事件所有信息
    """

    def __init__(self):
        super().__init__()
        self.stack = [""]
        self.stackStr = ""
