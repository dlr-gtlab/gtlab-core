/* GTlab - Gas Turbine laboratory
 * copyright 2009-2023 by DLR
 *
 *  Created on: 27.1.2023
 *  Author: Marius Bröcker (AT-TWK)
 *  E-Mail: marius.broecker@dlr.de
 */

#include "gtest/gtest.h"

#include "gt_functionalinterface.h"
#include "gt_logging.h"

class TestMpl : public testing::Test {};

// the signature is compiler dependent (for example gcc will conver 'bool*' to 'b*' etc).
// Therefore this test is enabled for MSVC only
#ifdef _MSVC_LANG

TEST_F(TestMpl, functionSignature)
{
    int myFunction(QString const str, bool* ok = nullptr);

    QString signature1 = gt::interface::getFunctionSignature<decltype(myFunction)>();
    gtDebug() << "Signature 1:" << signature1;
    EXPECT_EQ(signature1, "int (QString, bool*)");

    void myOtherFunction();

    QString signature2 = gt::interface::getFunctionSignature<decltype(myOtherFunction)>();
    gtDebug() << "Signature 2_1:" << signature2;
    EXPECT_EQ(signature2, "void ()");

    auto myLambda= [](float const&, QString*, double){ return QChar{}; };

    QString lambdaSig = gt::interface::getFunctionSignature<decltype(myLambda)>();
    gtDebug() << "Signature Lambda:" << lambdaSig;
    EXPECT_EQ(lambdaSig, "QChar (float const&, QString*, double)");

    struct CustomFunctor
    {
        bool operator()(QString const&, int const*, char const);
    };

    QString customFunctor = gt::interface::getFunctionSignature<CustomFunctor>();
    gtDebug() << "Signature Functor:" << customFunctor;
    EXPECT_EQ(customFunctor, "bool (QString const&, int const*, char" /*'const' is not matched somehow*/ ")");
}
#endif
