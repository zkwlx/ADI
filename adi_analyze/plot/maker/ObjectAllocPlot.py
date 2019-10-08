#!/usr/bin/env python
# -*- coding: utf-8 -*-
# @Time    : 2019/9/25 11:02 上午
# @Author  : kewen
# @File    : ObjectAllocPlot.py
from bokeh.plotting import figure

from aggregate.GlobalAggregateInfo import GlobalAggregateInfo
from plot.BaseMaker import BaseMaker
from plot.maker.ObjectAllocLine import ObjectAllocLine
from utils.ColorUtils import get_n_rgb_colors

OBJECT_COUNT_PLOT = 1
OBJECT_SIZE_PLOT = 2


class ObjectAllocPlot(BaseMaker):

    def __init__(self):
        super().__init__()

    def make(self, globalAggInfo: GlobalAggregateInfo, jsonList: list):
        return self.drawObjectAllocGraph(globalAggInfo, jsonList)

    def drawObjectAllocGraph(self, globalAggInfo: GlobalAggregateInfo, jsonList: list):
        plotLineDict = {}
        for json in jsonList:
            if json["eventName"] in ["OA", "OF"]:
                aggId = json["aggregateId"]
                plotLine = plotLineDict.get(aggId, None)
                if plotLine is None:
                    plotLine = ObjectAllocLine()
                    plotLine.aggId = aggId
                    plotLineDict[aggId] = plotLine
                plotLine.timeList.append(json["time"])
                plotLine.countList.append(json["count"])
                plotLine.objectSizeList.append(json["totalSize"])
                plotLine.objectNameList.append(json["objectName"])
                plotLine.threadNameList.append(json["threadName"])
                if json["eventName"] == "OF":
                    plotLine.eventNameList.append("Free")
                    plotLine.stackList.append("")
                else:
                    plotLine.eventNameList.append("Alloc")
                    plotLine.stackList.append(json["aggNiceStack"])

        if not plotLineDict:
            return None
        else:
            ids = plotLineDict.keys()
            colors = get_n_rgb_colors(len(ids))
            idColorDict = dict(zip(ids, colors))
            countTitle = "对象分配数量报表，总时长：%d 毫秒，日志文件：%s" % (globalAggInfo.totalTime, globalAggInfo.fileName)
            sizeTitle = "对象分配大小报表，总时长：%d 毫秒，日志文件：%s" % (globalAggInfo.totalTime, globalAggInfo.fileName)
            countPlot = self.makeObjectPlot(OBJECT_COUNT_PLOT, countTitle, plotLineDict, idColorDict)
            sizePlot = self.makeObjectPlot(OBJECT_SIZE_PLOT, sizeTitle, plotLineDict, idColorDict)
            return countPlot, sizePlot

    def makeObjectPlot(self, plotType: int, title: str, plotLineDict: dict, idColorDict: dict):
        hoverToolHtml = """
                <div>
                    <div>
                        <span style="font-size: 6px;">object </span>
                        <span style="font-size: 5px; font-weight: bold;">@eventNameList</span>
                    </div>
                    <div>
                        <span style="font-size: 5px; font-weight: bold;">object name:</span>
                        <span style="font-size: 6px;">@objectNameList</span>
                    </div>
                    <div>
                        <span style="font-size: 5px; font-weight: bold;">thread:</span>
                        <span style="font-size: 6px;">@threadNameList</span>
                    </div>
                    <div>
                        <span style="font-size: 5px; font-weight: bold;">object size:</span>
                        <span style="font-size: 6px;">@objectSizeList Byte</span>
                    </div>
                    <div>
                        <span style="font-size: 5px; font-weight: bold;">object count:</span>
                        <span style="font-size: 6px;">@y</span>
                    </div>
                    <div>
                        <span style="font-size: 5px; font-weight: bold;">timestamp:</span>
                        <span style="font-size: 6px;">@x ms</span>
                    </div>
                    <div>
                        <span style="font-size: 5px; font-weight: bold;">alloc thread stack:</span>
                        <span style="font-size: 6px; white-space: pre-wrap;">\n@stackList</span>
                    </div>
                </div>
                """
        if plotType == OBJECT_SIZE_PLOT:
            y_label = "对象大小 Byte"
        else:
            y_label = "对象数量"
        graph = figure(plot_width=1200, plot_height=800, title=title,
                       x_axis_label="时间 毫秒",
                       y_axis_label=y_label, tooltips=hoverToolHtml)

        for _, plot in plotLineDict.items():
            if plotType == OBJECT_SIZE_PLOT:
                plotY = plot.objectSizeList
            else:
                plotY = plot.countList

            data = dict(x=plot.timeList, y=plotY, objectNameList=plot.objectNameList,
                        threadNameList=plot.threadNameList, stackList=plot.stackList,
                        objectSizeList=plot.objectSizeList, eventNameList=plot.eventNameList)
            graph.line(source=data, x="x", y="y", color=idColorDict[plot.aggId], line_width=2)

        return graph
