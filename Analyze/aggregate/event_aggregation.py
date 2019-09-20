#!/usr/bin/env python
# -*- coding: utf-8 -*-
# @Time    : 2019/9/3 11:57 上午
# @Author  : kewen
# @File    : event_aggregation.py
import difflib

from aggregate.AggregateObject import AggregateObject
from event.MonitorContendedEvent import MonitorContendedEvent
from event.ObjectAllocEvent import ObjectAllocEvent
from event.ObjectFreeEvent import ObjectFreeEvent
from event.ThreadStartEvent import ThreadStartEvent
from utils.JVMUtils import convertClassDesc, convertMethodDesc

idCounter = 1000

aggregateObjectList: list = []
objectTagDict: dict = {}

threadCount = 0

monitorObjDict: dict = {}
monitorObjIdCounter = 0
contendThreadDict: dict = {}
contendThreadIdCounter = 0


def aggregateMCEDEvent(event: MonitorContendedEvent) -> (int, int):
    if event.monitorObjHash in monitorObjDict:
        monitorObjId = monitorObjDict[event.monitorObjHash]
    else:
        global monitorObjIdCounter
        monitorObjIdCounter += 1
        monitorObjDict[event.monitorObjHash] = monitorObjIdCounter
        monitorObjId = monitorObjIdCounter

    if event.contendThreadName in contendThreadDict:
        contendThreadId = contendThreadDict[event.contendThreadName]
    else:
        global contendThreadIdCounter
        contendThreadIdCounter += 1
        contendThreadDict[event.contendThreadName] = contendThreadIdCounter
        contendThreadId = contendThreadIdCounter
    return monitorObjId, contendThreadId


def aggregateMCEEvent(event: MonitorContendedEvent) -> (str, str):
    # monitorObjId, contendThreadId = aggregateMCEDEvent(event)
    contendStack = convertNiceStack(event.contendStack)
    ownerStack = convertNiceStack(event.ownerStack)
    return contendStack, ownerStack


def aggregateOFEvent(event: ObjectFreeEvent) -> (int, int, str, int):
    objEvent = objectTagDict[event.tag]
    freeSize = objEvent.objectSize
    marchedObj = None
    for aggObj in aggregateObjectList:
        if event.tag in aggObj.tags:
            aggObj.count -= 1
            aggObj.totalSize -= freeSize
            marchedObj = aggObj
            break
    if marchedObj is None:
        print("!!!!! aggregateOFEvent: not found OFEvent tag: %d" % event.tag)
    return marchedObj.aggId, marchedObj.count, marchedObj.niceStack, marchedObj.totalSize


def aggregateTSEvent(e: ThreadStartEvent) -> int:
    global threadCount
    threadCount += 1
    return threadCount


def aggregateOAEvent(event: ObjectAllocEvent) -> (int, int, str, int):
    """
    根据传入的 ObjectAllocEvent，获得聚合后的信息
    :param event:
    :return : 返回聚合后的信息，包括 聚合 ID、相同 ID 的事件数量、相同 ID 的可读版本栈
    """
    objectTagDict[event.objectTag] = event
    global idCounter
    marchedScore = 0
    marchedAggObj: AggregateObject
    newAggObj = AggregateObject()
    newAggObj.copyFromOAEvent(event)
    for aggObj in aggregateObjectList:
        if aggObj.objectName == newAggObj.objectName and aggObj.threadName == newAggObj.threadName:
            currScore = matcherScoreQuick(aggObj.stackStr, newAggObj.stackStr)
            if currScore > marchedScore:
                marchedScore = currScore
                marchedAggObj = aggObj

    if marchedScore < 0.96:
        idCounter += 1
        newAggObj.aggId = idCounter
        newAggObj.tags.append(event.objectTag)
        newAggObj.niceStack = aggregateOAStack(event)
        aggregateObjectList.append(newAggObj)
    else:
        marchedAggObj.count += 1
        marchedAggObj.totalSize += newAggObj.totalSize
        if event.objectTag in marchedAggObj.tags:
            print("------------!!!! %d" % event.objectTag)
        else:
            marchedAggObj.tags.append(event.objectTag)
        newAggObj = marchedAggObj

    return newAggObj.aggId, newAggObj.count, newAggObj.niceStack, newAggObj.totalSize


def aggregateOAStack(e: ObjectAllocEvent) -> str:
    """
    聚合 Object Alloc Event 的栈信息
    :param e:
    :return:
    """
    header = "%s %s\n" % (convertClassDesc(e.objectName), e.threadName)
    niceStack = convertNiceStack(e.stack)
    # print(header)
    return header + niceStack


def convertNiceStack(stackList: list) -> str:
    """
    将原始 Stack 信息转换成更加可读的样子
    :param stackList:
    :return:
    """
    niceStack = ""
    for line in stackList:
        if "(null)" in line:
            niceStack = line
        else:
            args = line.split(" ")
            if len(args) == 3:
                niceStack += "at    " + convertClassDesc(args[0]) + "." + args[1] + convertMethodDesc(args[2]) + "\n"
            else:
                niceStack += "at    " + line
    return niceStack


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
