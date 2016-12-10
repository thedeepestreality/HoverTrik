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
#include <cstdlib>


int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
   // HoverTrik worker;

//    if (argc == 5)
//    {
        HoverConfig conf;
//        conf.leftC = atoi(argv[1]);
//        conf.rightC = atoi(argv[2]);
//        conf.leftM = atoi(argv[3]);
//        conf.rightM = atoi(argv[4]);
        conf.fsr = atoi(argv[1]);
//        conf.duration = atoi(argv[6]);
//        conf.imuDuration = atoi(argv[7]);
//        conf.phi_p = atof(argv[8]);
//        conf.phi_i = atof(argv[9]);
//        conf.w_p = atof(argv[10]);
//        conf.w_i = atof(argv[11]);
//        conf.lvl = atof(argv[12]);

//        printf("Started with params: %d %d %d %d %d\r\n",conf.leftM,
//                                                      conf.rightM,
//                                                      conf.fsr,
//                                                      conf.duration,
//                                                      conf.imuDuration);

        HoverTrik worker(conf);
 //   }
//        else
//    {
//        printf("Error args\r\n");
//        return -1;
//    }
    //QObject::connect(&worker,SIGNAL(finish()),&app,SLOT(finish()));
	return app.exec();
}
