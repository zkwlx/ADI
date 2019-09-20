#!/usr/bin/env python
# -*- coding: utf-8 -*-
# @Time    : 2019/9/2 11:24 上午
# @Author  : kewen
# @File    : adi_analyzer.py
import sys

import pymongo
from bokeh.plotting import figure, output_file, show

from PlotLine import PlotLine
from PlotSegment import PlotSegment
from aggregate.aggregate_to_json import aggToJson
from event.Event import Event
from handler.GCHandler import GCHandler
from handler.MonitorContendedHandler import MonitorContendedHandler
from handler.ObjectAllocHandler import ObjectAllocHandler
from handler.ObjectFreeHandler import ObjectFreeHandler
from handler.ThreadStartHandler import ThreadStartHandler
from utils.ColorUtils import get_n_rgb_colors

FILE_NAME = "adi_launch_stack2.log"

mongo = pymongo.MongoClient(host="localhost", port=27017)
db = mongo['adi_analyze']
collection = db[FILE_NAME]

# TODO 在这里配置需要解析的 Event
handlerList = [ObjectAllocHandler(),
               MonitorContendedHandler(),
               ObjectFreeHandler(),
               GCHandler(),
               ThreadStartHandler()]


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
    # print("\n聚合完毕，批量存入到数据库...")
    # collection.insert(eventJsonList)
    print("\n聚合完毕，生成图表...")
    drawMonitorGraph(eventJsonList)


def drawMonitorGraph(jsonList: list):
    output_file(FILE_NAME + ".html")
    threadNames = set()
    monitorObjs = set()
    for json in jsonList:
        if json["eventName"] in ["MCE", "MCED"]:
            threadNames.add(json["contendThreadName"][0:40])
            monitorObjs.add(json["monitorObjHash"])
    colors = get_n_rgb_colors(len(monitorObjs))
    monitorObjColorDict = dict(zip(monitorObjs, colors))

    plotSegments = {}
    for json in jsonList:
        name = json["eventName"]
        if name == "MCE":
            monitorObj = json["monitorObjHash"]
            plotSeg = plotSegments.get(monitorObj, None)
            if plotSeg is None:
                plotSeg = PlotSegment()
                plotSeg.color = monitorObjColorDict[monitorObj]
                plotSeg.legend = str(monitorObj)
                plotSegments[monitorObj] = plotSeg
            plotSeg.x0.append(json["time"])
            contendThread = json["contendThreadName"][0:40]
            plotSeg.y0.append(contendThread)
            plotSeg.y1.append(contendThread)
            plotSeg.ownerThread.append(json["ownerThreadName"][0:40])
            plotSeg.monitorId.append(monitorObj)
            plotSeg.contendStack.append(json["contendStack"])
            plotSeg.ownerStack.append(json["ownerStack"])
        elif name == "MCED":
            monitorObj = json["monitorObjHash"]
            plotSeg = plotSegments.get(monitorObj, None)
            if plotSeg is None:
                print("what!?!?!?!?!??!?!?!")
            else:
                plotSeg.x1.append(json["time"])

    hoverToolHtml = """
    <div>
        <div>
            <span style="font-size: 5px; font-weight: bold;">monitor id:</span>
            <span style="font-size: 6px;">@monitorId</span>
        </div>
        <div>
            <span style="font-size: 5px; font-weight: bold;">contend thread:</span>
            <span style="font-size: 6px;">@y0</span>
        </div>
        <div>
            <span style="font-size: 5px; font-weight: bold;">contend stack:</span>
            <span style="font-size: 6px; white-space: pre-wrap;">\n@contendStack</span>
        </div>
        <div>
            <span style="font-size: 5px; font-weight: bold;">owner thread:</span>
            <span style="font-size: 6px;">@ownerThread</span>
        </div>
        <div>
            <span style="font-size: 5px; font-weight: bold;">owner stack:</span>
            <span style="font-size: 6px; white-space: pre-wrap;">\n@ownerStack</span>
        </div>
    </div>
    """
    graph = figure(plot_width=1200, plot_height=800, y_range=list(threadNames), title=FILE_NAME, x_axis_label="时间 毫秒",
                   y_axis_label="线程名字", tooltips=hoverToolHtml)
    for _, plot in plotSegments.items():
        data = dict(x0=plot.x0, y0=plot.y0, x1=plot.x1, y1=plot.y1, ownerThread=plot.ownerThread,
                    monitorId=plot.monitorId, contendStack=plot.contendStack, ownerStack=plot.ownerStack)
        graph.segment(source=data, x0="x0", x1="x1", y0="y0", y1="y1", line_width=10, color=plot.color)
    show(graph)


def drawGraph(jsonList: list):
    output_file(FILE_NAME + ".html")
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

    graph = figure(title=FILE_NAME, x_axis_label="时间 毫秒", y_axis_label="数量")
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
