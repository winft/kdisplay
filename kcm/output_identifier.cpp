/********************************************************************
Copyright © 2019 Roman Gilg <subdiff@gmail.com>

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*********************************************************************/
#include "output_identifier.h"

#include "../common/utils.h"

#include <disman/output.h>

#include <QQuickItem>
#include <QQuickView>
#include <QStandardPaths>
#include <QSurfaceFormat>
#include <QTimer>

#define QML_PATH "kpackage/kcms/kcm_kdisplay/contents/ui/"

OutputIdentifier::OutputIdentifier(Disman::ConfigPtr config, QObject* parent)
    : QObject(parent)
{
    QQuickWindow::setDefaultAlphaBuffer(true);
    const QString qmlPath = QStandardPaths::locate(QStandardPaths::GenericDataLocation,
                                                   QStringLiteral(QML_PATH "OutputIdentifier.qml"));

    for (const auto& output : config->outputs()) {
        if (!output->auto_mode()) {
            continue;
        }

        auto const mode = output->auto_mode();

        auto view = new QQuickView;
        QSurfaceFormat format;
        format.setAlphaBufferSize(8);
        view->setFormat(format);
        view->setColor(QColor(0, 0, 0, 0));
        view->setFlags(Qt::X11BypassWindowManagerHint | Qt::FramelessWindowHint);
        view->setSource(QUrl::fromLocalFile(qmlPath));

        view->installEventFilter(this);

        auto rootObj = view->rootObject();
        if (!rootObj) {
            delete view;
            continue;
        }

        QSize deviceSize;
        QSizeF logicalSize;
        if (output->horizontal()) {
            deviceSize = mode->size();
        } else {
            deviceSize = QSize(mode->size().height(), mode->size().width());
        }
        if (config->supported_features() & Disman::Config::Feature::PerOutputScaling) {
            // Scale adjustment is not needed on Wayland, we use logical size.
            logicalSize = output->geometry().size();
        } else {
            logicalSize = deviceSize / view->effectiveDevicePixelRatio();
        }
        rootObj->setProperty("outputName", Utils::outputName(output));
        rootObj->setProperty("modeName", Utils::sizeToString(deviceSize));
        view->setProperty("screenSize", QRectF(output->position(), logicalSize).toRect());
        m_views << view;
    }

    for (auto* view : m_views) {
        view->show();
    }
    QTimer::singleShot(2500, this, &OutputIdentifier::identifiersFinished);
}

OutputIdentifier::~OutputIdentifier()
{
    qDeleteAll(m_views);
}

bool OutputIdentifier::eventFilter(QObject* object, QEvent* event)
{
    if (event->type() == QEvent::Resize) {
        if (m_views.contains(qobject_cast<QQuickView*>(object))) {
            QResizeEvent* e = static_cast<QResizeEvent*>(event);
            const QRect screenSize = object->property("screenSize").toRect();
            QRect geometry(QPoint(0, 0), e->size());
            geometry.moveCenter(screenSize.center());
            static_cast<QQuickView*>(object)->setGeometry(geometry);
        }
    }
    return QObject::eventFilter(object, event);
}
