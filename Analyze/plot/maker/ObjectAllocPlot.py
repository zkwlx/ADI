#!/usr/bin/env python
# -*- coding: utf-8 -*-
# @Time    : 2019/9/25 11:02 上午
# @Author  : kewen
# @File    : ObjectAllocPlot.py
from bokeh.plotting import figure, output_file

from aggregate.GlobalAggregateInfo import GlobalAggregateInfo
from plot.BaseMaker import BaseMaker
from plot.PlotLine import PlotLine


class ObjectAllocPlot(BaseMaker):

    def __init__(self):
        super().__init__()

    def make(self, globalAggInfo: GlobalAggregateInfo, jsonList: list):
        return self.drawObjectAllocGraph(globalAggInfo, jsonList)

    @staticmethod
    def drawObjectAllocGraph(globalAggInfo: GlobalAggregateInfo, jsonList: list):
        output_file(globalAggInfo.fileName + ".html")
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

        if not plotLineDict:
            return None
        else:
            graph = figure(title=globalAggInfo.fileName, x_axis_label="时间 毫秒", y_axis_label="数量")
            for _, plot in plotLineDict.items():
                graph.line(plot.x, plot.y, legend=plot.legend)
            return graph
