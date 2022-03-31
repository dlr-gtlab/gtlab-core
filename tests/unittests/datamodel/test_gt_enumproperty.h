#ifndef TEST_GT_ENUMPROPERTY_H
#define TEST_GT_ENUMPROPERTY_H

#include <QObject>

class EnumContainer : public QObject
{
    Q_OBJECT
public:
    enum TestEnum {
        A = 0,
        B = 3,
        C
    };
    Q_ENUM(TestEnum)
};

#endif // TEST_GT_ENUMPROPERTY_H
