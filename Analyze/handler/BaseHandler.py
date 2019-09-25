#!/usr/bin/env python
# -*- coding: utf-8 -*-
# @Time    : 2019/9/2 2:01 下午
# @Author  : kewen
# @File    : BaseHandler.py
from event.Event import Event

SEP_POWER = "^^^"
SEP_COMMA = ",,,"


class BaseHandler:

    def shouldHandle(self, eventName) -> bool:
        return False

    def handle(self, segmentList) -> Event:
        pass
