#!/usr/bin/env python
# -*- coding: utf-8 -*-
# @Time    : 2019/9/2 2:05 下午
# @Author  : kewen
# @File    : ObjectAllocHandler.py
from event.ObjectFreeEvent import ObjectFreeEvent
from handler.BaseHandler import BaseHandler, SEP_POWER

ObjectFree = "OF"


class ObjectFreeHandler(BaseHandler):

    def shouldHandle(self, eventName):
        return eventName == ObjectFree

    def handle(self, segmentList):
        event = ObjectFreeEvent()
        event.eventName = segmentList[0]
        # 收集基本信息
        base = segmentList[1]
        baseList = base.split(SEP_POWER)
        event.timestamp = int(baseList[0])
        event.tag = int(baseList[1])
        return event
