#!/usr/bin/env python
# -*- coding: utf-8 -*-
# @Time    : 2019/9/10 5:00 下午
# @Author  : kewen
# @File    : aggregate_to_json.py
from aggregate.EventAggregation import aggregateOAEvent, aggregateTSEvent, aggregateOFEvent, aggregateMCEEvent


def aggToJson(event, startTime: int) -> dict:
    eventName = event.eventName
    json = {"timestamp": event.timestamp,
            "time": (event.timestamp - startTime),
            "eventName": eventName}
    if eventName == "OA":
        (aggId, count, niceStack, totalSize) = aggregateOAEvent(event)
        json["threadName"] = event.threadName
        json["objectName"] = event.objectName
        json["objectSize"] = event.objectSize
        json["originStack"] = event.stackStr
        json["aggregateId"] = aggId
        json["count"] = count
        json["aggNiceStack"] = niceStack
        json["totalSize"] = totalSize
        json["objTag"] = event.objectTag
    elif eventName == "OF":
        (aggId, count, niceStack, totalSize) = aggregateOFEvent(event)
        json["objTag"] = event.tag
        json["aggregateId"] = aggId
        json["count"] = count
        json["aggNiceStack"] = niceStack
        json["totalSize"] = totalSize
    elif eventName in "MCE":
        json["contendThreadName"] = event.contendThreadName
        json["monitorObjHash"] = event.monitorObjHash
        json["ownerThreadName"] = event.ownerThreadName
        json["entryCount"] = event.entryCount
        json["waiterCount"] = event.waiterCount
        json["notifyWaiterCount"] = event.notifyWaiterCount
        monitorObjName, contendStack, ownerStack = aggregateMCEEvent(event)
        json["monitorObjName"] = monitorObjName + "@" + str(event.monitorObjHash)
        json["contendStack"] = contendStack
        json["ownerStack"] = ownerStack
    elif eventName in "MCED":
        json["contendThreadName"] = event.contendThreadName
        json["monitorObjHash"] = event.monitorObjHash
    elif eventName == "TS":
        count = aggregateTSEvent(event)
        json["startThreadName"] = event.startThreadName
        json["curTotalCount"] = count
    else:
        json = None
    return json
