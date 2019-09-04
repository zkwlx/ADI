#!/usr/bin/env python
# -*- coding: utf-8 -*-
# @Time    : 2019/9/2 2:05 下午
# @Author  : kewen
# @File    : ObjectAllocHandler.py
from event.ObjectAllocEvent import ObjectAllocEvent
from handler.BaseHandler import BaseHandler, SEP_POWER, SEP_COMMA

ObjectAlloc = "OA"


class ObjectAllocHandler(BaseHandler):

    def shouldHandle(self, eventName):
        return eventName == ObjectAlloc

    def handle(self, segmentList):
        event = ObjectAllocEvent()
        event.eventName = segmentList[0]
        base = segmentList[1]
        # 收集基本信息
        baseList = base.split(SEP_POWER)
        event.timestamp = int(baseList[0])
        event.threadName = baseList[1]
        event.objectName = baseList[2]
        event.objectSize = int(baseList[3])
        # 收集栈信息
        stack = str(segmentList[2]).replace(SEP_POWER, " ")
        event.stack = stack.split(SEP_COMMA)
        event.stackStr = "\n".join(event.stack)
        return event
