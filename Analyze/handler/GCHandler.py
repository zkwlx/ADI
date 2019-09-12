#!/usr/bin/env python
# -*- coding: utf-8 -*-
# @Time    : 2019/9/2 2:05 下午
# @Author  : kewen
# @File    : ObjectAllocHandler.py
from event.GCEvent import GCEvent
from handler.BaseHandler import BaseHandler

GCStart = "GCS"
GCFinish = "GCF"


class GCHandler(BaseHandler):

    def shouldHandle(self, eventName):
        return eventName == GCStart or eventName == GCFinish

    def handle(self, segmentList):
        event = GCEvent()
        event.eventName = segmentList[0]
        event.timestamp = int(segmentList[1])
        return event
