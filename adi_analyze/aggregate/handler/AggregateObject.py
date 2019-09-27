#!/usr/bin/env python
# -*- coding: utf-8 -*-
# @Time    : 2019/9/10 4:03 下午
# @Author  : kewen
# @File    : AggregateObject.py
from event.ObjectAllocEvent import ObjectAllocEvent


class AggregateObject:

    def __init__(self):
        self.objectName = ""
        self.threadName = ""
        self.stackStr = ""
        self.niceStack = ""
        self.niceObjectName = ""
        self.aggId = 0
        self.count = 1
        self.totalSize = 0
        self.tags = []

    def copyFromOAEvent(self, event: ObjectAllocEvent):
        self.objectName = event.objectName
        self.threadName = event.threadName
        self.stackStr = event.stackStr
        self.totalSize = event.objectSize
