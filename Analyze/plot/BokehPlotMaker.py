#!/usr/bin/env python
# -*- coding: utf-8 -*-
# @Time    : 2019/9/20 4:10 下午
# @Author  : kewen
# @File    : BokehPlotMaker.py
from bokeh.plotting import figure, output_file, show

from PlotLine import PlotLine
from PlotSegment import PlotSegment
from plot.BaseMaker import BaseMaker
from utils.ColorUtils import get_n_rgb_colors


class BokehPlotMaker(BaseMaker):

    def __init__(self, logName: str):
        super().__init__()
        self.logName = logName

    def make(self, jsonList: list):
        self.drawMonitorGraph(jsonList)

    def drawMonitorGraph(self, jsonList: list):
        output_file(self.logName + ".html")
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
        graph = figure(plot_width=1200, plot_height=800, y_range=list(threadNames), title=self.logName,
                       x_axis_label="时间 毫秒",
                       y_axis_label="线程名字", tooltips=hoverToolHtml)
        for _, plot in plotSegments.items():
            data = dict(x0=plot.x0, y0=plot.y0, x1=plot.x1, y1=plot.y1, ownerThread=plot.ownerThread,
                        monitorId=plot.monitorId, contendStack=plot.contendStack, ownerStack=plot.ownerStack)
            graph.segment(source=data, x0="x0", x1="x1", y0="y0", y1="y1", line_width=10, color=plot.color)
        show(graph)

    def drawGraph(self, jsonList: list):
        output_file(self.logName + ".html")
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

        graph = figure(title=self.logName, x_axis_label="时间 毫秒", y_axis_label="数量")
        for _, plot in plotLineDict.items():
            graph.line(plot.x, plot.y, legend=plot.legend)

        show(graph)
