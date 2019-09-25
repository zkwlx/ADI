#!/usr/bin/env python
# -*- coding: utf-8 -*-
# @Time    : 2019/9/2 11:24 上午
# @Author  : kewen
# @File    : adi_analyzer.py
import sys

from aggregate.GlobalAggregateInfo import GlobalAggregateInfo
from aggregate.aggregate_to_json import aggregateToJson
from event.Event import Event
from handler.GCHandler import GCHandler
from handler.MonitorContendedHandler import MonitorContendedHandler
from handler.ObjectAllocHandler import ObjectAllocHandler
from handler.ObjectFreeHandler import ObjectFreeHandler
from handler.ThreadStartHandler import ThreadStartHandler
from plot.BokehPlotMaker import BokehPlotMaker

FILE_NAME = "adi_15.log"

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


def aggregateEvents(originEventList: list) -> (GlobalAggregateInfo, list):
    """
    对原始数据列表进行聚合计算，生成 json 列表
    :param originEventList:
    :return:
    """
    # 对齐时间起点
    startTime = originEventList[0].timestamp
    endTime = originEventList[-1].timestamp
    # 创建全局聚合信息
    globalInfo = GlobalAggregateInfo()
    globalInfo.fileName = FILE_NAME
    globalInfo.startTimestamp = startTime
    globalInfo.endTimestamp = endTime
    globalInfo.totalTime = endTime - startTime
    # 创建聚合事件列表
    eventJsonList = []
    totalCount = len(originEventList)
    currCount = 0
    for event in originEventList:
        json = aggregateToJson(event, globalInfo)
        if json is not None:
            eventJsonList.append(json)
        currCount += 1
        percent = float(currCount / totalCount) * 100
        sys.stdout.write("\r%.2f%% 处理进度" % percent)
        sys.stdout.flush()
    return globalInfo, eventJsonList


def makePlot(globalAggInfo: GlobalAggregateInfo, aggJsonList: list):
    # maker = MongoPlotMaker(FILE_NAME)
    maker = BokehPlotMaker()
    maker.make(globalAggInfo, aggJsonList)


def main():
    originEventList = handleTraceFile()
    print("原始数据解析完毕，总共 %d 条" % len(originEventList))
    globalAggInfo, aggJsonList = aggregateEvents(originEventList)
    print("\n聚合完毕，生成图表...")
    makePlot(globalAggInfo, aggJsonList)
    print("解析完成！")


if __name__ == "__main__":
    main()
