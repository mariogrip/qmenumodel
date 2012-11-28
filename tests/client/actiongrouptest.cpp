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
#include "qstateaction.h"

#include <QObject>
#include <QSignalSpy>
#include <QtTest>
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
        QVERIFY(m_script.connect());
    }

    void cleanupTestCase()
    {
        m_script.quit();
    }

    void init()
    {
        m_model.stop();
        m_model.setBusType(DBusEnums::SessionBus);
        m_model.setBusName(MENU_SERVICE_NAME);
        m_model.setObjectPath(MENU_OBJECT_PATH);

        m_actionGroup.stop();
        m_actionGroup.setBusType(DBusEnums::SessionBus);
        m_actionGroup.setBusName(MENU_SERVICE_NAME);
        m_actionGroup.setObjectPath(MENU_OBJECT_PATH);
    }

    void cleanup()
    {
        m_script.unpublishMenu();
    }

    /*
     * Test if the propety busType handle correct integer values
     */
    void testBusTypeProperty()
    {
        m_actionGroup.setProperty("busType", 1);
        QCOMPARE(m_actionGroup.busType(), DBusEnums::SessionBus);

        m_actionGroup.setProperty("busType", 2);
        QCOMPARE(m_actionGroup.busType(), DBusEnums::SystemBus);

        m_actionGroup.setProperty("busType", 0);
        QCOMPARE(m_actionGroup.busType(), DBusEnums::SystemBus);

        m_actionGroup.setProperty("busType", 10);
        QCOMPARE(m_actionGroup.busType(), DBusEnums::SystemBus);
    }

    /*
     * Test if QDBusActionGroup change to correct state after DBus
     * ervice appear
     */
    void testServiceAppear()
    {
        m_model.start();
        m_actionGroup.start();
        QCOMPARE(m_actionGroup.status(), DBusEnums::Connecting);

        // Make menu available
        m_script.publishMenu();

        QCOMPARE(m_actionGroup.status(), DBusEnums::Connected);
    }

    /*
     * Test if QDBusActionGroup change to correct state after DBus
     * service disappear
     */
    void testServiceDisappear()
    {
        m_model.start();
        m_actionGroup.start();

        // Make menu available
        m_script.publishMenu();
        QCOMPARE(m_actionGroup.status(), DBusEnums::Connected);

        // Append menus
        m_script.walk(2);

        // Remove menu from dbus
        m_script.unpublishMenu();
        QCOMPARE(m_actionGroup.status(), DBusEnums::Connecting);

        m_actionGroup.stop();
        QCOMPARE(m_actionGroup.status(), DBusEnums::Disconnected);
    }

    /*
     * Test if Action::trigger active the action over DBus
     */
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
        QStateAction *act = m_actionGroup.action(action.toString());
        QVERIFY(act);

        // test action name
        QCOMPARE(act->property("name").toString(), QString("Menu1Act"));

        act->activate(QVariant("42"));
        // wait for dbus propagation
        QTest::qWait(500);

        QPair<QString, QVariant> result = m_script.popActivatedAction();
        QCOMPARE(result.first, QString("Menu1Act"));
        QCOMPARE(result.second.toString(), QString("42"));
    }

    /*
     * Test if Action became invalid after desappear from DBus
     */
    void testRemoveAction()
    {
        // start model
        m_model.start();
        m_actionGroup.start();

        // Make menu available and append 2 menus
        m_script.publishMenu();
        m_script.walk(2);

        // Get Action
        QStateAction *act = m_actionGroup.action(QString("Menu1Act"));
        QVERIFY(act);
        QVERIFY(act->isValid());

        // Remove 1 menu
        m_script.walk(1);

        //Check if action is invalid
        QVERIFY(!act->isValid());
    }

    /*
     * Test if Action became valid after service appears
     */
    void testActionIsValid()
    {
        // start model
        m_model.start();
        m_actionGroup.start();

        // Make menu available and append 2 menus
        m_script.publishMenu();

        // Get invalid Action
        QStateAction *act = m_actionGroup.action(QString("Menu1Act"));
        QVERIFY(act);
        QVERIFY(!act->isValid());
        QVERIFY(!act->state().isValid());

        // Append menus
        m_script.walk(2);

        // Action appear
        QVERIFY(act->isValid());
    }
};

QTEST_MAIN(ActionGroupTest)

#include "actiongrouptest.moc"
