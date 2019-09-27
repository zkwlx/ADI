#!/usr/bin/env python
# -*- coding: utf-8 -*-
# @Time    : 2019/9/19 3:37 下午
# @Author  : kewen
# @File    : ColorUtils_test.py

import unittest

from utils.ColorUtils import toHex, get_n_rgb_colors


class Test(unittest.TestCase):

    def test_toHex(self):
        rgb = [244, 255, 196]
        self.assertEqual(toHex(rgb), "#f4ffc4")

    def test_get_n_rgb(self):
        get_n_rgb_colors(3)


if __name__ == '__main__':
    unittest.main()
