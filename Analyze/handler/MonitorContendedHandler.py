#!/usr/bin/env python
# -*- coding: utf-8 -*-
# @Time    : 2019/9/2 2:05 下午
# @Author  : kewen
# @File    : ObjectAllocHandler.py
from event.MonitorContendedEvent import MonitorContendedEvent
from handler.BaseHandler import BaseHandler, SEP_POWER, SEP_COMMA

MonitorContended = "MCE"


class MonitorContendedHandler(BaseHandler):

    def shouldHandle(self, eventName):
        return str(eventName).startswith(MonitorContended)

    def handle(self, segmentList):
        """
        一个 MCE 类型事件的例子：
        无调用栈：MCE|1568794380319^^^RxCachedThreadScheduler-10^^^82116050|RxCachedThreadScheduler-10^^^0^^^0^^^0
        有调用栈：
        :param segmentList:
        :return:
        """
        event = MonitorContendedEvent()
        event.eventName = segmentList[0]
        # 收集基本信息
        base = segmentList[1]
        baseList = base.split(SEP_POWER)
        event.timestamp = int(baseList[0])
        event.contendThreadName = baseList[1]
        event.monitorObjHash = int(baseList[2])
        # 只有 MCE 有锁信息，MCED 没有
        if event.eventName == MonitorContended:
            # 收集锁信息
            monitor = segmentList[2]
            monitorList = monitor.split(SEP_POWER)
            event.ownerThreadName = monitorList[0]
            event.entryCount = int(monitorList[1])
            event.waiterCount = int(monitorList[2])
            event.notifyWaiterCount = int(monitorList[3])
            # 收集竞争线程的栈信息
            stack = str(segmentList[3]).replace(SEP_POWER, " ")
            event.contendStack = stack.split(SEP_COMMA)
            # 收集持有锁线程的栈信息
            stack = str(segmentList[4]).replace(SEP_POWER, " ")
            event.ownerStack = stack.split(SEP_COMMA)
        return event
