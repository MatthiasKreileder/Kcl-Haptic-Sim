## -*- Mode: python; py-indent-offset: 4; indent-tabs-mode: nil; coding: utf-8; -*-

REQUIRED_BOOST_LIBS = ['system', 'signals', 'filesystem', 'thread']

def required_boost_libs(conf):
    conf.env['REQUIRED_BOOST_LIBS'] += REQUIRED_BOOST_LIBS

def build(bld):
    obj = bld.create_ns3_module('Kcl-Haptic-Sim', ['internet'])
    obj.source = [
       'model/haptic-file-sensor.cc',
       'model/sensor-data-sample.cc',
       'model/haptic-header.cc',
       'model/haptic-operator.cc',
       'model/data-reduction-algorithm.cc',
       'model/no-data-reduction-algorithm.cc',
       'model/haptic-tele-operator.cc',
       'model/chai3d-server.cc',
       'model/named-pipe-handler.cc',
       'model/phantom-agent.cc',
       'model/haptic-data-reduction-algorithm.cc',
       'model/shared-memory-handler.cc',
       'helper/haptic-operator-helper.cc',
       'helper/haptic-tele-operator-helper.cc',
       'helper/position-to-velocity-conversion.cc',
       'helper/chai3d-server-helper.cc',
       'helper/phantom-agent-helper.cc'
        ]
    obj.cxxflags = ['-std=c++11',
    				'-I /home/matthias/Development/install_dir/boost_install_dir/include/',
    				'-L /home/matthias/Development/install_dir/boost_install_dir/lib',
    				'-l boost_system',
    				'-l boost_thread',
    				'-L /lib64',
    				'-l pthread']

    module_test = bld.create_ns3_module_test_library('Kcl-Haptic-Sim')
    module_test.source = [
        'test/haptic-file-sensor-test-suite.cc',
        'test/haptic-header-test-suite.cc',
        'test/sensor-data-sample-test-suite.cc',
        'test/haptic-operator-test-suite.cc',
        'test/haptic-tele-operator-test-suite.cc',
        'test/position-to-velocity-conversion-test-suite.cc',
        'test/chai3d-server-test-suite.cc',
        'test/haptic-data-reduction-algorithm-test-suite.cc'
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
       'model/haptic-tele-operator.h',
       'model/chai3d-server.h',
       'model/named-pipe-handler.h',
       'model/phantom-agent.h',
       'model/haptic-data-reduction-algorithm.h',
       'model/shared-memory-handler.h',
       'helper/haptic-operator-helper.h',
       'helper/haptic-tele-operator-helper.h',
       'helper/position-to-velocity-conversion.h',
       'helper/chai3d-server-helper.h',
       'helper/phantom-agent-helper.h'
        ]

    if bld.env.ENABLE_EXAMPLES:
        bld.recurse('examples')    
