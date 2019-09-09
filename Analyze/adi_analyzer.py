#!/usr/bin/env python
# -*- coding: utf-8 -*-
# @Time    : 2019/9/2 11:24 上午
# @Author  : kewen
# @File    : adi_analyzer.py

import pymongo

from event.Event import Event
from event_aggregation import aggregateOAEvent, aggregateTSEvent
from handler.GCHandler import GCHandler
from handler.ObjectAllocHandler import ObjectAllocHandler
from handler.ThreadStartHandler import ThreadStartHandler

FILE_NAME = "adi_1568028643.log"

mongo = pymongo.MongoClient(host="localhost", port=27017)
db = mongo['adi_analyze']
collection = db[FILE_NAME]

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
    # 对齐时间起点
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
            eventJsonList.append(json)
        if event.eventName == "TS":
            count = aggregateTSEvent(event)
            json = {
                "timestamp": event.timestamp,
                "time": (event.timestamp - startTime),
                "eventName": event.eventName,
                "startThreadName": event.startThreadName,
                "curTotalCount": count,
            }
            eventJsonList.append(json)

    for json in eventJsonList:
        collection.insert(json)


def main():
    originEventList = []
    with open(FILE_NAME, "r") as file:
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
