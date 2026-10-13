/* GTlab - Gas Turbine laboratory
 *
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 * SPDX-License-Identifier: MPL-2.0+
 */

#include <QCoreApplication>
#include <QPluginLoader>
#include <QJsonDocument>
#include <QTextStream>

int
main(int argc, char* argv[])
{
    QCoreApplication app(argc, argv);
    if (app.arguments().size() != 2)
    {
        return 2;
    }

    QPluginLoader loader(app.arguments().at(1));
    const auto metadata = loader.metaData();
    if (metadata.isEmpty())
    {
        QTextStream(stderr) << "No plugin metadata: " << loader.errorString()
                            << " (" << loader.fileName() << ")\n";
        return 1;
    }

    const auto output = QJsonDocument(metadata).toJson(QJsonDocument::Compact);
    fwrite(output.constData(), 1, size_t(output.size()), stdout);
    fputc('\n', stdout);
    return 0;
}
