/*
    Copyright (C) 2012  Dan Vratil <dvratil@redhat.com>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/


#ifndef MONITORVIEW_H
#define MONITORVIEW_H

#include <QDeclarativeItem>

#define QML_PATH "kcm_displayconfiguration/qml/"

class /*KScreen::*/Output;
class QMLOutput;
class QDeclarativeContext;

class QMLOutputView : public QDeclarativeItem
{
    Q_OBJECT

    Q_PROPERTY(QList<QMLOutput*> outputs READ outputs NOTIFY outputsChanged);
public:
    QMLOutputView();
    virtual ~QMLOutputView();

    void addOutput(QDeclarativeEngine* engine, /*KScreen::*/Output* output);

    QList<QMLOutput*> outputs() const;

Q_SIGNALS:
    void outputsChanged();

private Q_SLOTS:
    void outputMoved();
    void outputClicked();

private:
    QDeclarativeContext * context() const;

    QList<QMLOutput*> m_outputs;

};

#endif // MONITORVIEW_H
