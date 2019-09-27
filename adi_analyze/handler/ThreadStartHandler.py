#!/usr/bin/env python
# -*- coding: utf-8 -*-
# @Time    : 2019/9/2 2:05 下午
# @Author  : kewen
# @File    : ObjectAllocHandler.py
from event.ThreadStartEvent import ThreadStartEvent
from handler.BaseHandler import BaseHandler, SEP_POWER, SEP_COMMA

ThreadStart = "TS"


class ThreadStartHandler(BaseHandler):

    def shouldHandle(self, eventName):
        return eventName == ThreadStart

    def handle(self, segmentList):
        event = ThreadStartEvent()
        event.eventName = segmentList[0]
        # 收集基本信息
        base = segmentList[1]
        baseList = base.split(SEP_POWER)
        event.timestamp = int(baseList[0])
        event.startThreadName = baseList[1]
        return event
