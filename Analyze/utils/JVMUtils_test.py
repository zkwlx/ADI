#!/usr/bin/env python
# -*- coding: utf-8 -*-
# @Time    : 2019/9/3 8:38 下午
# @Author  : kewen
# @File    : JVMUtils_test.py.py

import unittest

from utils.JVMUtils import convertBaseType, convertObjectDesc, convertArray, convertClassDesc, convertMethodDesc


class Test(unittest.TestCase):

    def test_convertBaseType(self):
        self.assertEqual(convertBaseType("[[J"), "long[][]", "解析失败！")
        self.assertEqual(convertBaseType("Z"), "bool", "解析失败！")

    def test_convertObjectType(self):
        self.assertEqual(convertObjectDesc("Landroid/content/Context;"), "android.content.Context")
        self.assertEqual(convertObjectDesc("[[Landroid/content/Context;"), "android.content.Context[][]")

    def test_convertArray(self):
        self.assertEqual(convertArray("[[[J"), ("J", 3))
        self.assertEqual(convertArray("Z"), ("Z", 0))
        self.assertEqual(convertArray("[[[Landroid/content/Context;"), ("Landroid/content/Context;", 3))

    def test_convertClassDesc(self):
        self.assertEqual(convertClassDesc("[[[I"), "int[][][]")
        self.assertEqual(convertClassDesc("Landroid/content/Context;"), "android.content.Context")

    def test_convertMethodDesc(self):
        self.assertEqual("(int)", convertMethodDesc("(I)V"))
        self.assertEqual("(char[],int)", convertMethodDesc("([CI)[C"))
        self.assertEqual("()", convertMethodDesc("()[C"))
        self.assertEqual("(java.lang.Object,long)", convertMethodDesc("(Ljava/lang/Object;J)Ljava/lang/Runnable;"))


if __name__ == '__main__':
    unittest.main()
