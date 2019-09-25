#!/usr/bin/env python
# -*- coding: utf-8 -*-
# @Time    : 2019/9/20 4:10 下午
# @Author  : kewen
# @File    : BokehPlotMaker.py
from bokeh.layouts import column
from bokeh.plotting import show

from aggregate.GlobalAggregateInfo import GlobalAggregateInfo
from plot.BaseMaker import BaseMaker
from plot.maker.ObjectAllocPlot import ObjectAllocPlot
from plot.maker.ThreadContendPlot import ThreadContendPlot


class BokehPlotMaker(BaseMaker):

    def __init__(self, ):
        super().__init__()
        self.globalInfo = None
        self.makerList = [ThreadContendPlot(), ObjectAllocPlot()]

    def make(self, globalAggInfo: GlobalAggregateInfo, jsonList: list):
        self.globalInfo = globalAggInfo
        plotList = []
        for maker in self.makerList:
            plot = maker.make(globalAggInfo, jsonList)
            if plot:
                plotList.append(maker.make(globalAggInfo, jsonList))
        # TODO 展示所有 Plot
        show(column(plotList))
