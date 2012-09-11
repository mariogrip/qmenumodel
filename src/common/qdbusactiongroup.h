/*
 * Copyright 2012 Canonical Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Authors:
 *      Renato Araujo Oliveira Filho <renato@canonical.com>
 */

#ifndef QDBUSACTIONGROUP_H
#define QDBUSACTIONGROUP_H

#include "qdbusobject.h"

#include <QObject>
#include <QAction>
#include <QSet>

class QDBusActionGroup : public QObject, public QDBusObject
{
    Q_OBJECT
    Q_PROPERTY(int busType READ busType WRITE setIntBusType NOTIFY busTypeChanged)
    Q_PROPERTY(QString busName READ busName WRITE setBusName NOTIFY busNameChanged)
    Q_PROPERTY(QString objectPath READ objectPath WRITE setObjectPath NOTIFY objectPathChanged)
    Q_PROPERTY(int status READ status NOTIFY statusChanged)

public:
    QDBusActionGroup(QObject *parent=0);
    ~QDBusActionGroup();

Q_SIGNALS:
    void busTypeChanged();
    void busNameChanged();
    void objectPathChanged();
    void statusChanged();

public Q_SLOTS:
    void start();
    void stop();
    QAction *getAction(const QString &actionName);

protected:
    virtual void serviceAppear(GDBusConnection *connection);
    virtual void serviceVanish(GDBusConnection *connection);

    virtual void busTypeChanged(BusType type);
    virtual void busNameChanged(const QString &busNameChanged);
    virtual void objectPathChanged(const QString &objectPath);
    virtual void statusChanged(ConnectionStatus status);

private:
    GActionGroup *m_actionGroup;
    QSet<QAction*> m_actions;
    int m_signalActionAddId;
    int m_signalActionRemovedId;

    // workaround to support int as bustType
    void setIntBusType(int busType);

    void setActionGroup(GDBusActionGroup *ag);
    void addAction(const char *actionName);
    void removeAction(const char *actionName);
    void updateAction(const char *actionName, GVariant *state);
    void clear();

    // glib slots
    static void onActionAdded(GDBusActionGroup *ag, gchar *actionName, gpointer data);
    static void onActionRemoved(GDBusActionGroup *ag, gchar *actionName, gpointer data);
    static void onActionStateChanged(GDBusActionGroup *ag, gchar *actionName, GVariant *value, gpointer data);
};

#endif
