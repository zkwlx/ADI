#!/usr/bin/env python
# -*- coding: utf-8 -*-
# @Time    : 2019/9/19 11:52 上午
# @Author  : kewen
# @File    : PlotSegment.py


class ThreadContendSegment:

    def __init__(self):
        self.x0 = []
        self.y0 = []
        self.x1 = []
        self.y1 = []
        self.durations = []
        self.legend = ""
        self.color = ""
        self.ownerThread = []
        self.monitorObjNames = []
        self.contendStack = []
        self.ownerStack = []
        self.contendOwnedMonitors = []
