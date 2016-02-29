/* Copyright 2015 CyberTech Labs Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * This file was modified by Yurii Litvinov to make it comply with the requirements of trikRuntime
 * project. See git revision history for detailed changes. */

#include <QtCore/qglobal.h>

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
    #include <QtGui/QApplication>
#else
    #include <QtWidgets/QApplication>
#endif

//#include "I2Cdev.h"
#include "hovertrik.h"
#include <QObject>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    HoverTrik worker;
    //QObject::connect(&worker,SIGNAL(finish()),&app,SLOT(finish()));
	return app.exec();
}
