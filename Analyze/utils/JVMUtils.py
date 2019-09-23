#!/usr/bin/env python
# -*- coding: utf-8 -*-
# @Time    : 2019/9/3 8:25 下午
# @Author  : kewen
# @File    : JVMUtils.py


def convertNiceStack(stackList: list) -> str:
    """
    将原始 Stack 信息转换成更加可读的样子
    例如将：\n
    Ljava/util/Arrays; copyOf ([CI)[C\n
    Ljava/lang/AbstractStringBuilder; ensureCapacityInternal (I)V\n
    Ljava/lang/StringBuilder; append()\n
    转换成：\n
    java.util.Arrays.copyOf(char[],int)\n
    java.lang.AbstractStringBuilder.ensureCapacityInternal(int)\n
    java.lang.StringBuilder.append()\n
    :param stackList:
    :return:
    """
    niceStack = ""
    for line in stackList:
        if "(null)" in line:
            niceStack = line
        else:
            args = line.split(" ")
            argsLen = len(args)
            if argsLen == 3:
                niceStack += "at    " + convertClassDesc(args[0]) + "." + args[1] + convertMethodDesc(args[2]) + "\n"
            elif argsLen == 2:
                niceStack += "at    " + convertClassDesc(args[0]) + "." + args[1] + "()\n"
            else:
                niceStack += "at    " + line
    return niceStack


def convertClassDesc(classDesc: str) -> str:
    """
    将 JVM 中类签名字节码转换成可读形式，例如：\n
    Landroid/content/Context; -> android.content.Context
    [[J -> long[][]
    :param classDesc:
    :return:
    """
    if classDesc.startswith("L"):
        return convertObjectDesc(classDesc)
    else:
        return convertBaseType(classDesc)


def convertMethodDesc(methodDesc: str) -> str:
    """
    将 JVM 中方法签名字节码转换成可读形式，例如：\n
    (Landroid/content/Context;[[JZ)Ljava/lang/String; -> (android.content.Context,long[][],bool)
    (JLandroid/content/Context;)J -> (long,android.content.Context)
    :param methodDesc:
    :return:
    """
    newMethodDesc = ""
    isObject = False
    classDesc = ""
    arrayDesc = ""
    for c in methodDesc:
        if isObject:  # 进入对象签名解析
            if c == ";":  # 到对象解析结尾，转换格式
                classDesc += ";"
                newMethodDesc += convertObjectDesc(classDesc) + ","
                classDesc = ""
                isObject = False
            else:
                classDesc += c
        else:
            if c == "(":
                newMethodDesc += c
            elif c == "[":
                arrayDesc += c
            elif c == "L":
                isObject = True
                classDesc = arrayDesc + c
                arrayDesc = ""
            elif c in ['C', 'B', 'D', 'F', 'I', 'J', 'S', 'Z']:
                baseDesc = arrayDesc + c
                newMethodDesc += convertBaseType(baseDesc) + ","
                arrayDesc = ""
            elif c == ")":
                if newMethodDesc.endswith(","):
                    newMethodDesc = newMethodDesc[0:-1]
                newMethodDesc += c
                break
    return newMethodDesc


def convertObjectDesc(objectDesc: str) -> str:
    """
    将 JVM 中对象签名字节码转换成可读形式，例如：\n
    Landroid/content/Context; -> android.content.Context
    [[Landroid/content/Context; -> android.content.Context[][]
    :param objectDesc:
    :return:
    """
    objectDesc, arrayDimension = convertArray(objectDesc)
    desc = objectDesc[1:-1]
    desc = desc.replace("/", ".")
    for i in range(arrayDimension):
        desc += "[]"

    return desc


def convertBaseType(base: str) -> str:
    """
    将 JVM 中基础类型字节码转换成可读形式，例如：\n
    [[J -> long[][]
    Z -> bool
    """
    base, arrayDimension = convertArray(base)
    humanDesc = ""
    if base == 'C':
        humanDesc = "char"
    elif base == 'B':
        humanDesc = "byte"
    elif base == 'D':
        humanDesc = "double"
    elif base == 'F':
        humanDesc = "float"
    elif base == 'I':
        humanDesc = "int"
    elif base == 'J':
        humanDesc = "long"
    elif base == 'S':
        humanDesc = "short"
    elif base == 'Z':
        humanDesc = "bool"

    if humanDesc != "":
        for i in range(arrayDimension):
            humanDesc += "[]"

    return humanDesc


def convertArray(anyArray: str) -> (str, int):
    """
    将 JVM 中的数组类型签名转换成（数组类型、数组维度）例如：\n
    [[[Landroid/content/Context; -> (Landroid/content/Context;, 3)
    Z -> (Z, 0)
    :param anyArray:
    :return:
    """
    arrayDimension = 0
    for c in anyArray:
        if c == "[":
            arrayDimension += 1
        else:
            break

    return anyArray[arrayDimension:], arrayDimension
