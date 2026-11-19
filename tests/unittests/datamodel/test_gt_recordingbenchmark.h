/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */

#ifndef TEST_GT_RECORDINGBENCHMARK_H
#define TEST_GT_RECORDINGBENCHMARK_H

#include "gt_object.h"
#include "gt_doubleproperty.h"

#include <array>
#include <memory>

/// Object with ten registered double properties for the benchmark
class RecordingBenchmarkObject : public GtObject
{
    Q_OBJECT

public:
    Q_INVOKABLE RecordingBenchmarkObject()
    {
        for (std::size_t i = 0; i < props.size(); ++i)
        {
            const QString id = QStringLiteral("p%1").arg(i);
            props[i] = std::make_unique<GtDoubleProperty>(id, id);
            registerProperty(*props[i]);
        }
    }

    std::array<std::unique_ptr<GtDoubleProperty>, 10> props;
};

#endif // TEST_GT_RECORDINGBENCHMARK_H
