#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# @Time    : 2019/10/12 5:41 下午
# @Author  : kewen
# @File    : setup.py.py

from setuptools import setup, find_packages

setup(
    name='adi_analyzer',
    version='0.9.3',
    author='zkwlx',
    author_email='zkwlixue@163.com',
    url='https://github.com/zkwlx/ADI',
    description='A powerful tool to help Android engineers optimize their performance',
    packages=find_packages().append("adi_analyzer.py"),
    install_requires=['bokeh'],
    entry_points={
        'console_scripts': [
            'adi_analyzer=adi_analyzer:main'
        ]
    }
)
