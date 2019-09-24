#!/usr/bin/env python
# -*- coding: utf-8 -*-
# @Time    : 2019/9/24 5:54 下午
# @Author  : kewen
# @File    : ThreadContendHandler.py
from aggregate.GlobalAggregateInfo import GlobalAggregateInfo
from aggregate.handler.BaseHandler import BaseHandler


class ThreadStartHandler(BaseHandler):

    def __init__(self):
        self.threadCount = 0

    def shouldHandle(self, eventName) -> bool:
        return eventName in ["TS"]

    def handle(self, event, globalInfo: GlobalAggregateInfo) -> dict:
        eventName = event.eventName
        self.threadCount += 1
        json = {"timestamp": event.timestamp,
                "time": (event.timestamp - globalInfo.startTimestamp),
                "eventName": eventName,
                "startThreadName": event.startThreadName,
                "curTotalCount": self.threadCount}
        return json
