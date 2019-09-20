#!/usr/bin/env python
# -*- coding: utf-8 -*-
# @Time    : 2019/9/19 3:31 下午
# @Author  : kewen
# @File    : ColorUtils.py
import colorsys
from random import random


def toHex(rgb) -> str:
    str_rgb = "#"
    for c in rgb:
        # 每次转换之后只取0x7b的后两位，拼接到strs中
        str_rgb += str(hex(c))[-2:]
    return str_rgb


def get_n_hls_colors(num) -> list:
    hls_colors = []
    i = 0
    step = 360.0 / num
    while i < 360:
        h = i
        s = 90 + random() * 10
        l = 50 + random() * 10
        _hlsc = [h / 360.0, l / 100.0, s / 100.0]
        hls_colors.append(_hlsc)
        i += step

    return hls_colors


def get_n_rgb_colors(num) -> list:
    rgb_colors = []
    if num < 1:
        return rgb_colors
    hls_colors = get_n_hls_colors(num)
    for hlsc in hls_colors:
        _r, _g, _b = colorsys.hls_to_rgb(hlsc[0], hlsc[1], hlsc[2])
        r, g, b = [int(x * 255.0) for x in (_r, _g, _b)]
        rgb_colors.append(toHex([r, g, b]))

    return rgb_colors
