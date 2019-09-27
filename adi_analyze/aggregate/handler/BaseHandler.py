#!/usr/bin/env python
# -*- coding: utf-8 -*-
# @Time    : 2019/9/2 2:01 下午
# @Author  : kewen
# @File    : BaseHandler.py
from aggregate.GlobalAggregateInfo import GlobalAggregateInfo


class BaseHandler:

    def shouldHandle(self, eventName) -> bool:
        return False

    def handle(self, event, globalInfo: GlobalAggregateInfo) -> dict:
        pass
