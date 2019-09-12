#!/usr/bin/env python
# -*- coding: utf-8 -*-
# @Time    : 2019/9/2 11:24 上午
# @Author  : kewen
# @File    : adi_analyzer.py
import sys

import pymongo
from bokeh.plotting import figure, output_file, show

from PlotLine import PlotLine
from aggregate.aggregate_to_json import aggToJson
from event.Event import Event
from handler.GCHandler import GCHandler
from handler.ObjectAllocHandler import ObjectAllocHandler
from handler.ObjectFreeHandler import ObjectFreeHandler
from handler.ThreadStartHandler import ThreadStartHandler

FILE_NAME = "adi_1568174079.log"

mongo = pymongo.MongoClient(host="localhost", port=27017)
db = mongo['adi_analyze']
collection = db[FILE_NAME]

# TODO 在这里配置需要解析的 Event
handlerList = [ObjectAllocHandler(), ObjectFreeHandler(), GCHandler(), ThreadStartHandler()]


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
    # 插入到 MongoDB
    print("\n聚合完毕，批量存入到数据库...")
    # collection.insert(eventJsonList)
    drawGraph(eventJsonList)


def drawGraph(jsonList: list):
    output_file(FILE_NAME + ".html")
    graph = figure(title=FILE_NAME, x_axis_label="时间 毫秒", y_axis_label="数量")
    plotLineDict = {}
    for json in jsonList:
        if json["eventName"] in ["OA", "OF"]:
            aggId = json["aggregateId"]
            plotLine = plotLineDict.get(aggId, None)
            if plotLine is None:
                plotLine = PlotLine()
                plotLine.legend = str(aggId)
                plotLineDict[aggId] = plotLine
            plotLine.x.append(json["time"])
            plotLine.y.append(json["count"])
    for _, plot in plotLineDict.items():
        graph.line(plot.x, plot.y, legend=plot.legend)

    show(graph)


def main():
    originEventList = []
    with open(FILE_NAME, "r") as file:
        count = 0
        for line in file:
            count += 1
            event = handleLineFromFile(line)
            if event is not None:
                originEventList.append(event)

    print("原始数据解析完毕，总共 %d 条" % len(originEventList))
    aggregateEvents(originEventList)
    print("解析完成！")


if __name__ == "__main__":
    main()
