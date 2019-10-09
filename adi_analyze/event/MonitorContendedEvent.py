#!/usr/bin/env python
# -*- coding: utf-8 -*-
# @Time    : 2019/9/2 1:34 下午
# @Author  : kewen
# @File    : GCEvent.py
from event.Event import Event


class MonitorContendedEvent(Event):
    """
    JVM 发生锁竞争事件
    """

    def __init__(self):
        super().__init__()
        self.contendThreadName = ""
        self.monitorObjHash = ""
        self.monitorObjName = ""
        self.contendOwnedMonitors = []
        self.contendStack = []
        self.ownerThreadName = ""
        self.entryCount = 0
        self.waiterCount = 0
        self.notifyWaiterCount = 0
        self.ownerStack = []
