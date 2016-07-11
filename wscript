## -*- Mode: python; py-indent-offset: 4; indent-tabs-mode: nil; coding: utf-8; -*-

def build(bld):
    obj = bld.create_ns3_module('Kcl-Haptic-Sim', ['internet'])
    obj.source = [
       'model/haptic-file-sensor.cc',
       'model/position-data-sample.cc',
       'model/haptic-header.cc'
        ]
    obj.cxxflags = ['-std=c++11']

    module_test = bld.create_ns3_module_test_library('Kcl-Haptic-Sim')
    module_test.source = [
        'test/haptic-file-sensor-test-suite.cc',
        'test/haptic-header-test-suite.cc'
        ]

    headers = bld(features='ns3header')
    headers.module = 'Kcl-Haptic-Sim'
    headers.source = [
       'model/haptic-file-sensor.h',
       'model/position-data-sample.h',
       'model/haptic-header.h'
        ]

    