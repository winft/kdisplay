/*
 * Copyright (C) 2013  Daniel Vrátil <dvratil@redhat.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 */

#include "qmlscreen.h"
#include "qmloutputcomponent.h"
#include "qmloutput.h"

#include <kscreen/output.h>
#include <kscreen/config.h>

#include <KDebug>

QMLScreen::QMLScreen(QDeclarativeItem *parent):
    QDeclarativeItem(parent),
    m_connectedOutputsCount(0),
    m_enabledOutputsCount(0),
    m_leftmost(0),
    m_topmost(0),
    m_rightmost(0),
    m_bottommost(0)
{
}

QMLScreen::~QMLScreen()
{
}

void QMLScreen::addOutput(QDeclarativeEngine *engine, KScreen::Output *output)
{
    //QDeclarativeItem *container = findChild<QDeclarativeItem*>(QLatin1String("outputContainer"));

    QMLOutputComponent comp(engine, this);
    QMLOutput *qmloutput = comp.createForOutput(output);
    if (!qmloutput) {
        kWarning() << "Failed to create QMLOutput";
        return;
    }

    m_outputMap.insert(output, qmloutput);

    qmloutput->setParentItem(this);

    connect(output, SIGNAL(isConnectedChanged()),
            this, SLOT(outputConnectedChanged()));
    connect(output, SIGNAL(isEnabledChanged()),
            this, SLOT(outputEnabledChanged()));
    connect(output, SIGNAL(isPrimaryChanged()),
            this, SLOT(outputPrimaryChanged()));
    connect(output, SIGNAL(posChanged()),
            this, SLOT(outputPositionChanged()));
    connect(qmloutput, SIGNAL(yChanged()),
            this, SLOT(qmlOutputMoved()));
    connect(qmloutput, SIGNAL(xChanged()),
            this, SLOT(qmlOutputMoved()));
}

int QMLScreen::connectedOutputsCount() const
{
    return m_connectedOutputsCount;
}

int QMLScreen::enabledOutputsCount() const
{
    return m_enabledOutputsCount;
}

QMLOutput *QMLScreen::primaryOutput() const
{
    Q_FOREACH (QMLOutput *qmlOutput, m_outputMap) {
        if (qmlOutput->output()->isPrimary()) {
            return qmlOutput;
        }
    }

    return 0;
}

QSize QMLScreen::maxScreenSize() const
{
    KScreen::Config *config = KScreen::Config::current();
    return config->screen()->maxSize();
}

void QMLScreen::outputConnectedChanged()
{
    int connectedCount = 0;

    Q_FOREACH (KScreen::Output *output, m_outputMap.keys()) {
        if (output->isConnected()) {
            ++connectedCount;
        }
    }

    if (connectedCount != m_connectedOutputsCount) {
        m_connectedOutputsCount = connectedCount;
        Q_EMIT connectedOutputsCountChanged();
    }
}

void QMLScreen::outputEnabledChanged()
{
    /* TODO: Update position of QMLOutput */
    qmlOutputMoved(m_outputMap.value(qobject_cast<KScreen::Output*>(sender())));

    int enabledCount = 0;

    Q_FOREACH (KScreen::Output *output, m_outputMap.keys()) {
        if (output->isEnabled()) {
            ++enabledCount;
        }
    }

    if (enabledCount == m_enabledOutputsCount) {
        m_enabledOutputsCount = enabledCount;
        Q_EMIT enabledOutputsCountChanged();
    }
}

void QMLScreen::outputPrimaryChanged()
{
    QObject *newPrimary = sender();
    Q_FOREACH (KScreen::Output *output, m_outputMap.keys()) {
        if (output == newPrimary) {
            continue;
        }

        output->blockSignals(true);
        output->setPrimary(false);
        output->blockSignals(false);
    }

    Q_EMIT primaryOutputChanged();
}

void QMLScreen::outputPositionChanged()
{
    /* TODO: Reposition the QMLOutputs */
}

void QMLScreen::qmlOutputMoved()
{
    qmlOutputMoved(qobject_cast<QMLOutput*>(sender()));
}

void QMLScreen::qmlOutputMoved(QMLOutput *qmlOutput)
{
    updateCornerOutputs();

    if (m_leftmost) {
        m_leftmost->setOutputX(0);
    }
    if (m_topmost) {
        m_topmost->setOutputY(0);
    }

    if (qmlOutput == m_leftmost) {
        Q_FOREACH (QMLOutput *other, m_outputMap) {
            if (other == m_leftmost) {
                continue;
            }

            if (!other->output()->isConnected() || !other->output()->isEnabled()) {
                continue;
            }

            other->setOutputX(float(other->x() - m_leftmost->x()) / other->displayScale());
        }
    } else if (m_leftmost) {
        qmlOutput->setOutputX(float(qmlOutput->x() - m_leftmost->x()) / qmlOutput->displayScale());
    }

    if (qmlOutput == m_topmost) {
        Q_FOREACH (QMLOutput *other, m_outputMap) {
            if (other == m_topmost) {
                continue;
            }

            if (!other->output()->isConnected() || !other->output()->isEnabled()) {
                continue;
            }

            other->setOutputY(float(other->y() - m_topmost->y()) / other->displayScale());
        }
    } else if (m_topmost) {
        qmlOutput->setOutputY(float(qmlOutput->y() - m_topmost->y()) / qmlOutput->displayScale());
    }
}


void QMLScreen::updateCornerOutputs()
{
    m_leftmost = 0;
    m_topmost = 0;
    m_rightmost = 0;
    m_bottommost = 0;

    Q_FOREACH (QMLOutput *output, m_outputMap) {
        if (!output->output()->isConnected() || !output->output()->isEnabled()) {
            continue;
        }

        QMLOutput *other = m_leftmost;
        if (!other || output->x() < other->x()) {
            m_leftmost = output;
        }

        if (!other || output->y() < other->y()) {
            m_topmost = output;
        }

        if (!other || output->x() + output->width() > other->x() + other->width()) {
            m_rightmost = output;
        }

        if (!other || output->y() + output->height() > other->y() + other->height()) {
            m_bottommost = output;
        }
    }
}


#include "qmlscreen.moc"
