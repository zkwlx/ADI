#!/usr/bin/env python
# -*- coding: utf-8 -*-
# @Time    : 2019/9/10 5:00 下午
# @Author  : kewen
# @File    : aggregate_to_json.py
from aggregate.event_aggregation import aggregateOAEvent, aggregateTSEvent, aggregateOFEvent


def aggToJson(event, startTime: int) -> dict:
    json = None
    if event.eventName == "OA":
        (aggId, count, niceStack, totalSize) = aggregateOAEvent(event)
        json = {
            "timestamp": event.timestamp,
            "time": (event.timestamp - startTime),
            "eventName": event.eventName,
            "threadName": event.threadName,
            "objectName": event.objectName,
            "objectSize": event.objectSize,
            "originStack": event.stackStr,
            "aggregateId": aggId,
            "count": count,
            "aggNiceStack": niceStack,
            "totalSize": totalSize,
            "objTag": event.objectTag,
        }
    if event.eventName == "OF":
        (aggId, count, niceStack, totalSize) = aggregateOFEvent(event)
        json = {
            "timestamp": event.timestamp,
            "time": (event.timestamp - startTime),
            "eventName": event.eventName,
            "objTag": event.tag,
            "aggregateId": aggId,
            "count": count,
            "aggNiceStack": niceStack,
            "totalSize": totalSize,
        }
    if event.eventName == "TS":
        count = aggregateTSEvent(event)
        json = {
            "timestamp": event.timestamp,
            "time": (event.timestamp - startTime),
            "eventName": event.eventName,
            "startThreadName": event.startThreadName,
            "curTotalCount": count,
        }

    return json
