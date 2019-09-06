#!/usr/bin/env python
# -*- coding: utf-8 -*-
# @Time    : 2019/9/2 1:34 下午
# @Author  : kewen
# @File    : GCEvent.py
from event.Event import Event


class ThreadStartEvent(Event):
    """
    JVM 启动一个线程的 Event。
    """

    def __init__(self):
        super().__init__()
        self.startThreadName = ""
