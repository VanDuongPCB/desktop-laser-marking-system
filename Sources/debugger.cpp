//#include "debugger.h"
//#include <QFile>
//#include <QCoreApplication>
//Debugger::Debugger()
//{

//}

//Debugger::~Debugger()
//{

//}

//void Debugger::write(QString sender, QString sepa, QString content){
//    QFile fileWriter(QCoreApplication::applicationDirPath() + "/debug.txt");
//    if(!fileWriter.exists()) return;
//    if(fileWriter.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)){
//        QString dat = sender + " " + sepa + " " + content + "\n";
//        fileWriter.write(dat.toUtf8());
//        fileWriter.close();
//    }
//}
