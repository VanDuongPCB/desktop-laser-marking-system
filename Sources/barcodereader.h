
#ifndef BARCODEREADER_H
#define BARCODEREADER_H

#include <QString>



class BarcodeReader{
public:
    static bool hasData();
    static bool clear();
    static QString read();
    static bool sendFeedback(bool status);
};

#endif // BARCODEREADER_H
