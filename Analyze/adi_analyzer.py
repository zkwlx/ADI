#!/usr/bin/env python
# -*- coding: utf-8 -*-
# @Time    : 2019/9/2 11:24 上午
# @Author  : kewen
# @File    : adi_analyzer.py

import pymongo

from event.Event import Event
from event_aggregation import aggregateOAEvent
from handler.GCHandler import GCHandler
from handler.ObjectAllocHandler import ObjectAllocHandler
from handler.ThreadStartHandler import ThreadStartHandler

mongo = pymongo.MongoClient(host="localhost", port=27017)
db = mongo['adi_analyze']
collection = db['test']

# TODO 在这里配置需要解析的 Event
handlerList = [ObjectAllocHandler(), GCHandler(), ThreadStartHandler()]


def handleLineFromFile(line: str) -> Event:
    segmentList = line.split("|")
    eventName = segmentList[0]
    for h in handlerList:
        if h.shouldHandle(eventName):
            return h.handle(segmentList)
    print("------>没找到 %s" % eventName)


def aggregateEvents(originEventList: list):
    startTime = originEventList[0].timestamp
    eventJsonList = []
    for event in originEventList:
        if event.eventName == "OA":
            aggId, count, aggStack = aggregateOAEvent(event)
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
                "aggNiceStack": aggStack,
            }
            collection.insert(json)


def main():
    originEventList = []
    with open("adi.log", "r") as file:
        count = 0
        for line in file:
            count += 1
            event = handleLineFromFile(line)
            if event is not None:
                originEventList.append(event)
    print("总共 %d 行，成功识别 %d 行" % (count, len(originEventList)))
    aggregateEvents(originEventList)


if __name__ == "__main__":
    main()
