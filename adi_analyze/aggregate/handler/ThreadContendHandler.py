#!/usr/bin/env python
# -*- coding: utf-8 -*-
# @Time    : 2019/9/24 5:54 下午
# @Author  : kewen
# @File    : ThreadContendHandler.py
from aggregate.GlobalAggregateInfo import GlobalAggregateInfo
from aggregate.handler.BaseHandler import BaseHandler
from event.MonitorContendedEvent import MonitorContendedEvent
from utils.JVMUtils import convertClassDesc, convertNiceStack


def aggregateMCEEvent(event: MonitorContendedEvent) -> (str, str, str):
    # monitorObjId, contendThreadId = aggregateMCEDEvent(event)
    monitorObjName = convertClassDesc(event.monitorObjName)
    contendStack = convertNiceStack(event.contendStack)
    ownerStack = convertNiceStack(event.ownerStack)
    return monitorObjName, contendStack, ownerStack


class ThreadContendHandler(BaseHandler):

    def __init__(self):
        pass

    def shouldHandle(self, eventName) -> bool:
        return eventName in ["MCE", "MCED"]

    def handle(self, event, globalInfo: GlobalAggregateInfo) -> dict:
        eventName = event.eventName
        json = {"timestamp": event.timestamp,
                "time": (event.timestamp - globalInfo.startTimestamp),
                "eventName": eventName}
        if eventName in "MCE":
            json["contendThreadName"] = event.contendThreadName
            json["monitorObjHash"] = event.monitorObjHash
            json["ownerThreadName"] = event.ownerThreadName
            json["entryCount"] = event.entryCount
            json["waiterCount"] = event.waiterCount
            json["notifyWaiterCount"] = event.notifyWaiterCount
            json["contendOwnedMonitors"] = "[" + ", ".join(event.contendOwnedMonitors) + "]"
            monitorObjName, contendStack, ownerStack = aggregateMCEEvent(event)
            json["monitorObjName"] = monitorObjName + "@" + event.monitorObjHash
            json["contendStack"] = contendStack
            json["ownerStack"] = ownerStack
        elif eventName in "MCED":
            json["contendThreadName"] = event.contendThreadName
            json["monitorObjHash"] = event.monitorObjHash
        return json
