# Copyright 2015 CyberTech Labs Ltd.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at

#     http://www.apache.org/licenses/LICENSE-2.0

# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

TEMPLATE = app

CONFIG += console 
QMAKE_CXXFLAGS += -std=c++11

QMAKE_LFLAGS += -Wl,-O1,-rpath,.
QMAKE_LFLAGS += -Wl,-rpath-link,$$DESTDIR

DESTDIR = .

SOURCES += \
	main.cpp \
    inv_mpu.cpp \
    inv_mpu_dmp_motion_driver.cpp \
    mpu.cpp \
    I2Cdev.cpp \
    hovertrik.cpp

TRIK_RUNTIME_DIR = /home/captain/trikRuntime

include(/home/captain/trikRuntime/trikControl/trikControlExport.pri)

HEADERS += \
    I2Cdev.h \
    dmpKey.h \
    dmpmap.h \
    freeram.h \
    inv_mpu.h \
    inv_mpu_dmp_motion_driver.h \
    mpu.h \
    hovertrik.h
