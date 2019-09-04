#!/usr/bin/env python
# -*- coding: utf-8 -*-
# @Time    : 2019/9/3 11:57 上午
# @Author  : kewen
# @File    : event_aggregation.py
import copy
import difflib

from event.ObjectAllocEvent import ObjectAllocEvent
from utils.JVMUtils import convertClassDesc, convertMethodDesc

idCounter = 1000
aggregateEvent = []


def matcherScore(str1, str2):
    """
    获取字符串的匹配度
    :param str1:
    :param str2:
    :return: 匹配度 [0,1]
    """
    return difflib.SequenceMatcher(None, str1, str2).ratio()


def aggregateOAEvent(e: ObjectAllocEvent) -> (int, int, str):
    """
    根据传入的 ObjectAllocEvent，获取匹配的唯一 id
    :param e:
    :return:
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
        # 用 timestamp 字段存放 id
        event.aggId = idCounter
        event.count = 1
        event.niceStack = aggregateNiceStack(event)
        aggregateEvent.append(event)
    else:
        aggEvent.count += 1
        event.count = aggEvent.count

    return event.aggId, event.count, event.niceStack


def aggregateNiceStack(e: ObjectAllocEvent) -> str:
    header = "%s %s\n" % (convertClassDesc(e.objectName), e.threadName)
    niceStack = ""
    for line in e.stack:
        print(line)
        if "(null)" in line:
            niceStack = line
        else:
            args = line.split(" ")
            niceStack += "    " + convertClassDesc(args[0]) + "." + args[1] + convertMethodDesc(args[2]) + "\n"
    print(header + niceStack)
    return header + niceStack
