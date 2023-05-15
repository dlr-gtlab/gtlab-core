#include "gt_objectmemento.h"
#include "gt_objectfactory.h"

#include <iostream>

int main()
{
    std::cout << "RUNNING DATAPROCESSOR TESTS\n\n";

    auto xmlStr = R"(
        <object class="TestSpecialGtObject" name="Group" uuid="{a-uid}">
         <propertylist name="emptyStringList" type="QString"></propertylist>
         <propertylist name="stringList" type="QString">hello;world</propertylist>
         <propertylist name="threeStrings" type="QString">;;</propertylist>
         <property name="invalidVariant" type=""/>
        </object>
        )";

    auto memento = GtObjectMemento(xmlStr);

    auto dummy = memento.toObject(*gtObjectFactory);
    std::cout << "Object name: " << dummy->objectName().toStdString() << "\n\n";
    std::cout << "XML:\n" << memento.toByteArray().toStdString() << "\n";
    return 0;
}
