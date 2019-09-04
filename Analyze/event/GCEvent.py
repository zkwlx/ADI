#!/usr/bin/env python
# -*- coding: utf-8 -*-
# @Time    : 2019/9/2 1:34 下午
# @Author  : kewen
# @File    : GCEvent.py

from event.Event import Event


class GCEvent(Event):
    """
    JVM 触发垃圾回收的 Event。包括 GC 开始和 GC 结束
    """

    def __init__(self):
        super().__init__()
