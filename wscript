## -*- Mode: python; py-indent-offset: 4; indent-tabs-mode: nil; coding: utf-8; -*-

def build(bld):
    obj = bld.create_ns3_module('Kcl-Haptic-Sim', ['internet'])
    obj.source = [
       'model/haptic-file-sensor.cc',
       'model/sensor-data-sample.cc',
       'model/haptic-header.cc',
       'model/haptic-operator.cc',
       'model/data-reduction-algorithm.cc',
       'model/no-data-reduction-algorithm.cc',
       'helper/haptic-operator-helper.cc',
       'helper/position-to-velocity-conversion.cc'
        ]
    obj.cxxflags = ['-std=c++11']

    module_test = bld.create_ns3_module_test_library('Kcl-Haptic-Sim')
    module_test.source = [
        'test/haptic-file-sensor-test-suite.cc',
        'test/haptic-header-test-suite.cc',
        'test/sensor-data-sample-test-suite.cc',
        'test/haptic-operator-test-suite.cc',
        'test/position-to-velocity-conversion-test-suite.cc'
        ]

    headers = bld(features='ns3header')
    headers.module = 'Kcl-Haptic-Sim'
    headers.source = [
       'model/haptic-file-sensor.h',
       'model/sensor-data-sample.h',
       'model/haptic-header.h',
       'model/haptic-operator.h',
       'model/data-reduction-algorithm.h',
       'model/no-data-reduction-algorithm.h',
       'helper/haptic-operator-helper.h',
       'helper/position-to-velocity-conversion.h'
        ]

    
