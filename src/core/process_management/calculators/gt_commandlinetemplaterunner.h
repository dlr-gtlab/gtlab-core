#ifndef GT_COMMANDLINETEMPLATERUNNER_H
#define GT_COMMANDLINETEMPLATERUNNER_H

#include "gt_calculator.h"
#include "gt_stringproperty.h"

class GtCommandlineTemplateRunner : public GtCalculator
{
    Q_OBJECT
public:
    Q_INVOKABLE GtCommandlineTemplateRunner();

    /**
     * @brief Main run method of the calculator.
     * @return Whether run process was successful or not.
     */
    bool run() override;

    static GtCalculatorData calculatorData();


private:

    GtStringProperty m_executionDir;
};




#endif // GT_COMMANDLINETEMPLATERUNNER_H
