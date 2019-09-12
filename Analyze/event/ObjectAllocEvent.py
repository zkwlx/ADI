#!/usr/bin/env python
# -*- coding: utf-8 -*-
# @Time    : 2019/9/2 1:09 下午
# @Author  : kewen
# @File    : ObjectAllocEvent.py
from event.StackEvent import StackEvent


class ObjectAllocEvent(StackEvent):
    """
    JVM 分配对象的 Event。保存分配对象时的线程名字、对象类名、调用栈等详细信息
    """

    def __init__(self):
        super().__init__()
        self.threadName = ""
        self.objectName = ""
        self.objectSize = 0
        self.objectTag = 0


