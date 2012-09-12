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

#include "qdbusmenumodel.h"
#include "qdbusactiongroup.h"
#include "dbusmenuscript.h"

#include <QObject>
#include <QSignalSpy>
#include <QtTestGui>
#include <QDebug>

class ActionGroupTest : public QObject
{
    Q_OBJECT
private:
    DBusMenuScript m_script;
    QDBusMenuModel m_model;
    QDBusActionGroup m_actionGroup;

private Q_SLOTS:
    void initTestCase()
    {
        Q_ASSERT(m_script.connect());
    }

    void cleanupTestCase()
    {
        m_script.quit();
    }

    void init()
    {
        m_model.stop();
        m_model.setBusType(QDBusObject::SessionBus);
        m_model.setBusName(MENU_SERVICE_NAME);
        m_model.setObjectPath(MENU_OBJECT_PATH);

        m_actionGroup.stop();
        m_actionGroup.setBusType(QDBusObject::SessionBus);
        m_actionGroup.setBusName(MENU_SERVICE_NAME);
        m_actionGroup.setObjectPath(MENU_OBJECT_PATH);
    }

    void cleanup()
    {
        m_script.unpublishMenu();
    }

    void testBusTypeProperty()
    {
        m_actionGroup.setProperty("busType", 1);
        QCOMPARE(m_actionGroup.busType(), QDBusObject::SessionBus);

        m_actionGroup.setProperty("busType", 2);
        QCOMPARE(m_actionGroup.busType(), QDBusObject::SystemBus);

        m_actionGroup.setProperty("busType", 0);
        QCOMPARE(m_actionGroup.busType(), QDBusObject::SystemBus);

        m_actionGroup.setProperty("busType", 10);
        QCOMPARE(m_actionGroup.busType(), QDBusObject::SystemBus);
    }

    void testServiceAppear()
    {
        m_model.start();
        m_actionGroup.start();
        QCOMPARE(m_actionGroup.status(), QDBusObject::Connecting);
        QCOMPARE(m_actionGroup.count(), 0);

        // Make menu available
        m_script.publishMenu();

        QCOMPARE(m_actionGroup.status(), QDBusObject::Connected);
        QCOMPARE(m_actionGroup.count(), 0);
    }

    void testServiceDisappear()
    {
        m_model.start();
        m_actionGroup.start();
        QCOMPARE(m_actionGroup.count(), 0);

        // Make menu available
        m_script.publishMenu();
        QCOMPARE(m_actionGroup.status(), QDBusObject::Connected);

        // Append menus
        m_script.walk(2);
        QCOMPARE(m_actionGroup.count(), 2);

        // Remove menu from dbus
        m_script.unpublishMenu();
        QCOMPARE(m_actionGroup.status(), QDBusObject::Connecting);
        QCOMPARE(m_actionGroup.count(), 0);

        m_actionGroup.stop();
        QCOMPARE(m_actionGroup.status(), QDBusObject::Disconnected);
    }

    void testActiveAction()
    {
        // start model
        m_model.start();
        m_actionGroup.start();

        // Make menu available
        m_script.publishMenu();
        m_script.walk(2);

        // Get Action
        QVariant action = m_model.data(m_model.index(1, 0), QMenuModel::Action);
        QVERIFY(action.isValid());
        QAction *act = m_actionGroup.action(action.toString());
        QVERIFY(act);

        act->trigger();
        // wait for dbus propagation
        QTest::qWait(500);

        QCOMPARE(m_script.popActivatedAction(), QString("Menu1Act"));
    }

    void testRemoveAction()
    {
        // start model
        m_model.start();
        m_actionGroup.start();

        // Make menu available and append 2 menus
        m_script.publishMenu();
        m_script.walk(2);
        QCOMPARE(m_actionGroup.count(), 2);

        // Remove 1 menu
        m_script.walk(1);
        QCOMPARE(m_actionGroup.count(), 1);
    }
};

QTEST_MAIN(ActionGroupTest)

#include "actiongrouptest.moc"
