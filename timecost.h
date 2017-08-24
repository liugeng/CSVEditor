#ifndef TIMECOST_H
#define TIMECOST_H

#include <QTime>

#define TIME_INIT(MSG) \
    QTime time;\
    time.start();\
    QString __msg = MSG;\
    qDebug("[%s]", MSG);\
    int __t0 = time.elapsed();\
    int __t1 = __t0;\
    int __total = 0;

#define TIME_PRINT \
    __t1 = time.elapsed();\
    if (__t1 > __t0) {\
        __total += __t1 - __t0;\
        qDebug("    [%s:%d] %d", qPrintable(__msg), __LINE__, __t1-__t0);\
        __t0 = time.elapsed();\
    }

#define TIME_TOTAL \
    qDebug("[%s] total: %d", qPrintable(__msg), __total);

#endif // TIMECOST_H
