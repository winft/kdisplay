/*
    SPDX-FileCopyrightText: 2014-2016 Sebastian Kügler <sebas@kde.org>
    SPDX-FileCopyrightText: 2022 David Redondo <kde@david-redondo.de>

 SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "../../plasma-integration/osd/osdaction.h"
#include "osdservice_interface.h"

#include <QCoreApplication>
#include <QDBusConnection>

int main(int argc, char** argv)
{
    QCoreApplication app(argc, argv);

    QString const name = QStringLiteral("org.kwinft.kdisplay.osdService");
    QString const path = QStringLiteral("/org/kwinft/kdisplay/osdService");
    auto osdService
        = new OrgKwinftKdisplayOsdServiceInterface(name, path, QDBusConnection::sessionBus());

    auto reply = osdService->showActionSelector();

    // TODO(romangg): Currently does not start the actual service on the bus. Abort here.
    return 0;

    if (!reply.isValid()) {
        qDebug() << "Error calling osdService:";
        qDebug() << reply.error();
        return 1;
    }

    auto actionEnum = QMetaEnum::fromType<KDisplay::OsdAction::Action>();
    auto const value = actionEnum.valueToKey(reply.value());
    if (!value) {
        qDebug() << "Got invalid action" << reply.value();
        return 1;
    }
    qDebug() << "Selected Action" << value;
    return 0;
}
