/* GTlab - Gas Turbine laboratory
 * copyright 2009-2022 by DLR
 *
 *  Created on: 9.9.2022
 *  Author: Marius Bröcker (AT-TWK)
 *  E-Mail: marius.broecker@dlr.de
 */

#include "test_crashonexecutecalculator.h"

#include <stdexcept>

TestCrashOnExecuteCalculator::TestCrashOnExecuteCalculator()
{
    setObjectName("Crash Execution");
}

bool
TestCrashOnExecuteCalculator::run()
{
    gtWarning() << "*** Crashing GTlab ***";

    throw std::runtime_error{"do_crash"};
}
