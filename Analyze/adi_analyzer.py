#!/usr/bin/env python
# -*- coding: utf-8 -*-
# @Time    : 2019/9/2 11:24 上午
# @Author  : kewen
# @File    : adi_analyzer.py
import sys

from aggregate.aggregate_to_json import aggToJson
from event.Event import Event
from handler.GCHandler import GCHandler
from handler.MonitorContendedHandler import MonitorContendedHandler
from handler.ObjectAllocHandler import ObjectAllocHandler
from handler.ObjectFreeHandler import ObjectFreeHandler
from handler.ThreadStartHandler import ThreadStartHandler
from plot.BokehPlotMaker import BokehPlotMaker

FILE_NAME = "adi_launch_stack2.log"

# TODO 在这里配置需要解析的 Event
handlerList = [ObjectAllocHandler(),
               MonitorContendedHandler(),
               ObjectFreeHandler(),
               GCHandler(),
               ThreadStartHandler()]


def handleTraceFile() -> list:
    """
    从原始 trace 文件中解析 Event 对象列表
    :return:
    """
    originEventList = []
    with open(FILE_NAME, "r") as file:
        count = 0
        for line in file:
            count += 1
            event = handleLineFromFile(line)
            if event is not None:
                originEventList.append(event)
    return originEventList


def handleLineFromFile(line: str) -> Event:
    """
    将一行原始数据解析成 Event 对象
    :param line:
    :return:
    """
    segmentList = line.split("|")
    eventName = segmentList[0]
    for h in handlerList:
        if h.shouldHandle(eventName):
            return h.handle(segmentList)
    print("------>没找到 %s" % eventName)


def aggregateEvents(originEventList: list) -> list:
    """

    :param originEventList:
    :return:
    """
    # 对齐时间起点
    startTime = originEventList[0].timestamp
    eventJsonList = []
    totalCount = len(originEventList)
    currCount = 0
    for event in originEventList:
        json = aggToJson(event, startTime)
        if json is not None:
            eventJsonList.append(json)
        currCount += 1
        percent = float(currCount / totalCount) * 100
        sys.stdout.write("\r%.2f%% 处理进度" % percent)
        sys.stdout.flush()
    return eventJsonList


def makePlot(aggJsonList: list):
    # maker = MongoPlotMaker(FILE_NAME)
    maker = BokehPlotMaker(FILE_NAME)
    maker.make(aggJsonList)


def main():
    originEventList = handleTraceFile()
    print("原始数据解析完毕，总共 %d 条" % len(originEventList))
    aggJsonList = aggregateEvents(originEventList)
    print("\n聚合完毕，生成图表...")
    makePlot(aggJsonList)
    print("解析完成！")


if __name__ == "__main__":
    main()
