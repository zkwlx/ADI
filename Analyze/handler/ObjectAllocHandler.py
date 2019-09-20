#!/usr/bin/env python
# -*- coding: utf-8 -*-
# @Time    : 2019/9/2 2:05 下午
# @Author  : kewen
# @File    : ObjectAllocHandler.py
from event.ObjectAllocEvent import ObjectAllocEvent
from handler.BaseHandler import BaseHandler, SEP_POWER, SEP_COMMA

ObjectAlloc = "OA"


class ObjectAllocHandler(BaseHandler):

    def shouldHandle(self, eventName):
        return eventName == ObjectAlloc

    def handle(self, segmentList):
        """
        一个 OA 类型事件的例子：
        OA|1568794381394^^^RxComputationThreadPool-8^^^[C^^^4872^^^6
        |Ljava/util/Arrays;^^^copyOf^^^([CI)[C,,,Ljava/lang/AbstractStringBuilder;^^^ensureCapacityInternal^^^(I)V,,,Ljava/lang/AbstractStringBuilder;^^^append^^^(Ljava/lang/String;)Ljava/lang/AbstractStringBuilder;,,,Ljava/lang/StringBuilder;^^^append^^^(Ljava/lang/String;)Ljava/lang/StringBuilder;,,,Ljava/lang/StringBuilder;^^^append^^^(Ljava/lang/Object;)Ljava/lang/StringBuilder;,,,Lhu/akarnokd/rxjava2/debug/RxJavaAssemblyException;^^^buildStackTrace^^^()Ljava/lang/String;,,,Lhu/akarnokd/rxjava2/debug/RxJavaAssemblyException;^^^<init>^^^()V,,,Lhu/akarnokd/rxjava2/debug/ObservableOnAssembly;^^^<init>^^^(Lio/reactivex/ObservableSource;)V,,,Lhu/akarnokd/rxjava2/debug/RxJavaAssemblyTracking$3;^^^apply^^^(Lio/reactivex/Observable;)Lio/reactivex/Observable;,,,Lhu/akarnokd/rxjava2/debug/RxJavaAssemblyTracking$3;^^^apply^^^(Ljava/lang/Object;)Ljava/lang/Object;
        :param segmentList:
        :return:
        """
        event = ObjectAllocEvent()
        event.eventName = segmentList[0]
        base = segmentList[1]
        # 收集基本信息
        baseList = base.split(SEP_POWER)
        event.timestamp = int(baseList[0])
        event.threadName = baseList[1]
        event.objectName = baseList[2]
        event.objectSize = int(baseList[3])
        event.objectTag = int(baseList[4])
        # 收集栈信息
        stack = str(segmentList[2]).replace(SEP_POWER, " ")
        event.stack = stack.split(SEP_COMMA)
        event.stackStr = "\n".join(event.stack)
        return event
