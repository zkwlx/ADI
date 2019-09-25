#!/usr/bin/env python
# -*- coding: utf-8 -*-
# @Time    : 2019/9/25 11:02 上午
# @Author  : kewen
# @File    : ObjectAllocPlot.py
from bokeh.plotting import figure, output_file

from aggregate.GlobalAggregateInfo import GlobalAggregateInfo
from plot.BaseMaker import BaseMaker
from plot.PlotLineObjectAllocFree import PlotLineObjectAllocFree
from utils.ColorUtils import get_n_rgb_colors


class ObjectAllocPlot(BaseMaker):

    def __init__(self):
        super().__init__()

    def make(self, globalAggInfo: GlobalAggregateInfo, jsonList: list):
        return self.drawObjectAllocGraph(globalAggInfo, jsonList)

    def drawObjectAllocGraph(self, globalAggInfo: GlobalAggregateInfo, jsonList: list):
        output_file(globalAggInfo.fileName + ".html")
        plotLineDict = {}
        for json in jsonList:
            if json["eventName"] in ["OA", "OF"]:
                aggId = json["aggregateId"]
                plotLine = plotLineDict.get(aggId, None)
                if plotLine is None:
                    plotLine = PlotLineObjectAllocFree()
                    plotLine.aggId = aggId
                    plotLineDict[aggId] = plotLine
                plotLine.timeList.append(json["time"])
                plotLine.countList.append(json["count"])
                plotLine.objectSizeList.append(json["totalSize"])
                plotLine.objectNameList.append(json["objectName"])
                plotLine.threadNameList.append(json["threadName"])
                plotLine.aggIdList.append(aggId)
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
            countPlot = self.makeObjectCountPlot("", plotLineDict, idColorDict)
            return countPlot

    def makeObjectCountPlot(self, title: str, plotLineDict: dict, idColorDict: dict):
        hoverToolHtml = """
                <div>
                    <div>
                        <span style="font-size: 5px; font-weight: bold;">aggregate id:</span>
                        <span style="font-size: 6px;">@aggIdList</span>
                    </div>
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
                        <span style="font-size: 5px; font-weight: bold;">aobject size:</span>
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
        graph = figure(plot_width=1200, plot_height=800, title=title,
                       x_axis_label="时间 毫秒",
                       y_axis_label="对象数量", tooltips=hoverToolHtml)

        for _, plot in plotLineDict.items():
            data = dict(x=plot.timeList, y=plot.countList, objectNameList=plot.objectNameList,
                        threadNameList=plot.threadNameList, stackList=plot.stackList,
                        objectSizeList=plot.objectSizeList, eventNameList=plot.eventNameList,
                        aggIdList=plot.aggIdList)
            graph.line(source=data, x="x", y="y", color=idColorDict[plot.aggId], line_width=3)
        return graph
