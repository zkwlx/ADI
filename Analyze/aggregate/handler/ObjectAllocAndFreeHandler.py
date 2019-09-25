#!/usr/bin/env python
# -*- coding: utf-8 -*-
# @Time    : 2019/9/24 5:25 下午
# @Author  : kewen
# @File    : ObjectAllocAndFreeHandler.py
import difflib

from aggregate.GlobalAggregateInfo import GlobalAggregateInfo
from aggregate.handler.AggregateObject import AggregateObject
from aggregate.handler.BaseHandler import BaseHandler
from event.ObjectAllocEvent import ObjectAllocEvent
from event.ObjectFreeEvent import ObjectFreeEvent
from utils.JVMUtils import convertClassDesc, convertNiceStack


def matcherScoreQuick(str1, str2):
    """
    获取字符串的匹配度
    :param str1:
    :param str2:
    :return: 匹配度 [0,1]
    """
    return difflib.SequenceMatcher(None, str1, str2).quick_ratio()


def matcherScore(str1, str2):
    """
    获取字符串的匹配度
    :param str1:
    :param str2:
    :return: 匹配度 [0,1]
    """
    return difflib.SequenceMatcher(None, str1, str2).ratio()


class ObjectAllocAndFreeHandler(BaseHandler):

    def __init__(self):
        self.idCounter = 1000
        self.aggregateObjectList: list = []
        self.objectTagDict: dict = {}

    def shouldHandle(self, eventName) -> bool:
        return eventName in ["OA", "OF"]

    def handle(self, event, globalInfo: GlobalAggregateInfo) -> dict:
        eventName = event.eventName
        json = {"timestamp": event.timestamp,
                "time": (event.timestamp - globalInfo.startTimestamp),
                "eventName": eventName}
        if eventName == "OA":
            (aggId, count, niceStack, totalSize, niceObjectName) = self.aggregateOAEvent(event)
            json["threadName"] = event.threadName
            json["objectName"] = niceObjectName
            json["objectSize"] = event.objectSize
            json["originStack"] = event.stackStr
            json["aggregateId"] = aggId
            json["count"] = count
            json["aggNiceStack"] = niceStack
            json["totalSize"] = totalSize
            json["objTag"] = event.objectTag
        elif eventName == "OF":
            (aggId, count, niceStack, totalSize, niceObjectName, threadName) = self.aggregateOFEvent(event)
            json["objTag"] = event.tag
            json["aggregateId"] = aggId
            json["count"] = count
            json["aggNiceStack"] = niceStack
            json["totalSize"] = totalSize
            json["threadName"] = threadName
            json["objectName"] = niceObjectName
        return json

    def aggregateOAEvent(self, event: ObjectAllocEvent) -> (int, int, str, int):
        """
        根据传入的 ObjectAllocEvent，获得聚合后的信息
        :param event:
        :return : 返回聚合后的信息，包括 聚合 ID、相同 ID 的事件数量、相同 ID 的可读版本栈
        """
        self.objectTagDict[event.objectTag] = event
        marchedScore = 0
        marchedAggObj: AggregateObject
        newAggObj = AggregateObject()
        newAggObj.copyFromOAEvent(event)
        for aggObj in self.aggregateObjectList:
            if aggObj.objectName == newAggObj.objectName and aggObj.threadName == newAggObj.threadName:
                currScore = matcherScoreQuick(aggObj.stackStr, newAggObj.stackStr)
                if currScore > marchedScore:
                    marchedScore = currScore
                    marchedAggObj = aggObj

        if marchedScore < 0.96:
            self.idCounter += 1
            newAggObj.aggId = self.idCounter
            newAggObj.tags.append(event.objectTag)
            newAggObj.niceStack = convertNiceStack(event.stack)
            newAggObj.niceObjectName = convertClassDesc(event.objectName)
            self.aggregateObjectList.append(newAggObj)
        else:
            marchedAggObj.count += 1
            marchedAggObj.totalSize += newAggObj.totalSize
            if event.objectTag in marchedAggObj.tags:
                print("------------!!!! %d" % event.objectTag)
            else:
                marchedAggObj.tags.append(event.objectTag)
            newAggObj = marchedAggObj

        return newAggObj.aggId, newAggObj.count, newAggObj.niceStack, newAggObj.totalSize, newAggObj.niceObjectName

    def aggregateOFEvent(self, event: ObjectFreeEvent) -> (int, int, str, int, str, str):
        objEvent = self.objectTagDict[event.tag]
        freeSize = objEvent.objectSize
        marchedObj = None
        for aggObj in self.aggregateObjectList:
            if event.tag in aggObj.tags:
                aggObj.count -= 1
                aggObj.totalSize -= freeSize
                marchedObj = aggObj
                break
        if marchedObj is None:
            print("!!!!! aggregateOFEvent: not found OFEvent tag: %d" % event.tag)
        return marchedObj.aggId, marchedObj.count, marchedObj.niceStack, marchedObj.totalSize, \
               marchedObj.niceObjectName, marchedObj.threadName
