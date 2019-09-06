#!/usr/bin/env python
# -*- coding: utf-8 -*-
# @Time    : 2019/9/3 11:57 上午
# @Author  : kewen
# @File    : event_aggregation.py
import copy
import difflib

from event.ObjectAllocEvent import ObjectAllocEvent
from event.StackEvent import StackEvent
from event.ThreadStartEvent import ThreadStartEvent
from utils.JVMUtils import convertClassDesc, convertMethodDesc

idCounter = 1000
aggregateEvent = []

threadCount = 0


def aggregateTSEvent(e: ThreadStartEvent) -> int:
    global threadCount
    threadCount += 1
    return threadCount


def aggregateOAEvent(e: ObjectAllocEvent) -> (int, int, str):
    """
    根据传入的 ObjectAllocEvent，获得聚合后的信息
    :param e:
    :return : 返回聚合后的信息，包括 聚合 ID、相同 ID 的事件数量、相同 ID 的可读版本栈
    """
    global idCounter
    score = 0
    event = copy.deepcopy(e)
    aggEvent = None
    for ev in aggregateEvent:
        if ev.objectName == event.objectName and ev.threadName == event.threadName:
            currScore = matcherScore(ev.stackStr, event.stackStr)
            if 0.85 < currScore < 0.92:
                # print("score: %f, agg: \n%s, \ne: \n%s" % (currScore, ev.stackStr, event.stackStr))
                pass
            if currScore > score:
                score = currScore
                event.aggId = ev.aggId
                event.niceStack = ev.niceStack
                aggEvent = ev

    if score < 0.92:
        idCounter += 1
        event.aggId = idCounter
        event.count = 1
        event.niceStack = aggregateOAStack(event)
        aggregateEvent.append(event)
    else:
        aggEvent.count += 1
        event.count = aggEvent.count

    return event.aggId, event.count, event.niceStack


def aggregateOAStack(e: ObjectAllocEvent) -> str:
    """
    聚合 Object Alloc Event 的栈信息
    :param e:
    :return:
    """
    header = "%s %s\n" % (convertClassDesc(e.objectName), e.threadName)
    niceStack = convertNiceStack(e)
    print(header + niceStack)
    return header + niceStack


def convertNiceStack(e: StackEvent) -> str:
    """
    将原始 Stack 信息转换成更加可读的样子
    :param e:
    :return:
    """
    niceStack = ""
    for line in e.stack:
        if "(null)" in line:
            niceStack = line
        else:
            args = line.split(" ")
            if len(args) == 3:
                niceStack += "at    " + convertClassDesc(args[0]) + "." + args[1] + convertMethodDesc(args[2]) + "\n"
            else:
                niceStack += "at    " + line
    return niceStack


def matcherScore(str1, str2):
    """
    获取字符串的匹配度
    :param str1:
    :param str2:
    :return: 匹配度 [0,1]
    """
    return difflib.SequenceMatcher(None, str1, str2).ratio()
