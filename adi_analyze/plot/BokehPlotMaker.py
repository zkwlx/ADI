#!/usr/bin/env python
# -*- coding: utf-8 -*-
# @Time    : 2019/9/20 4:10 下午
# @Author  : kewen
# @File    : BokehPlotMaker.py
from bokeh.layouts import column
from bokeh.plotting import show, output_file

from aggregate.GlobalAggregateInfo import GlobalAggregateInfo
from plot.BaseMaker import BaseMaker
from plot.maker.ObjectAllocPlot import ObjectAllocPlot
from plot.maker.ThreadContendPlot import ThreadContendPlot


class BokehPlotMaker(BaseMaker):

    def __init__(self, ):
        super().__init__()
        self.makerList = [ThreadContendPlot(), ObjectAllocPlot()]

    def make(self, globalAggInfo: GlobalAggregateInfo, jsonList: list):
        output_file(globalAggInfo.filePath + ".html")
        plotList = []
        for maker in self.makerList:
            plot = maker.make(globalAggInfo, jsonList)
            if plot:
                if type(plot) is tuple:
                    for p in plot:
                        plotList.append(p)
                else:
                    plotList.append(plot)

        # 展示所有 Plot
        show(column(plotList))
