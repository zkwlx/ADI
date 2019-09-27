#!/usr/bin/env python
# -*- coding: utf-8 -*-
# @Time    : 2019/9/10 5:00 下午
# @Author  : kewen
# @File    : aggregate_to_json.py
from aggregate.GlobalAggregateInfo import GlobalAggregateInfo
from aggregate.handler.ObjectAllocAndFreeHandler import ObjectAllocAndFreeHandler
from aggregate.handler.ThreadContendHandler import ThreadContendHandler
from aggregate.handler.ThreadStartHandler import ThreadStartHandler

handlerList = [ObjectAllocAndFreeHandler(),
               ThreadContendHandler(),
               ThreadStartHandler()]


def aggregateToJson(event, globalInfo: GlobalAggregateInfo) -> dict:
    for handler in handlerList:
        if handler.shouldHandle(event.eventName):
            return handler.handle(event, globalInfo)
