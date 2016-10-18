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

extern "C" {
#include <glib.h>
}

#include "converter.h"

#include <QObject>
#include <QtTest>
#include <QDebug>

class ConverterTest : public QObject
{
    Q_OBJECT

private:
    bool compare(const QVariant &qv, const GVariantType *type)
    {
        bool result;
        GVariant *gv = Converter::toGVariant(qv);
        result = g_variant_type_equal(g_variant_get_type(gv), type);
        if (!result) {
            qWarning() << "types are different: QVariant:" << qv.typeName()
                       << "Result:" << (const char*) g_variant_get_type(gv)
                       << "Expected:"<< (const char*) type;
        }
        g_variant_unref(gv);
        return result;
    }
    bool compareWithSchema(const QVariant &qv, const QString strType)
    {
        GVariantType* expected_type;
        expected_type = g_variant_type_new(strType.toUtf8().data());

        bool result;
        GVariant *gv = Converter::toGVariantWithSchema(qv, strType.toUtf8().data());
        result = g_variant_type_equal(g_variant_get_type(gv), expected_type);
        if (!result) {
            qWarning() << "types are different: QVariant:" << qv.typeName()
                       << "Result:" << (const char*) g_variant_get_type(gv)
                       << "Expected:"<< (const char*) expected_type;
        }
        g_variant_unref(gv);
        return result;
    }

private Q_SLOTS:

    /*
     * Test converter QVariant to GVariant
     */

    void testBooleanToGVariant()
    {
        // Boolean
        QVERIFY(compare(QVariant(true), G_VARIANT_TYPE_BOOLEAN));
    }

    void testByteToGVariant()
    {
        // Byte
        QVERIFY(compare(QVariant::fromValue<uchar>(42), G_VARIANT_TYPE_BYTE));
    }

    void testInt16ToGVariant()
    {
        // Int16
        QVERIFY(compare(QVariant::fromValue<short>(-42), G_VARIANT_TYPE_INT16));
    }

    void testUInt16ToGVariant()
    {
        // UInt16
        QVERIFY(compare(QVariant::fromValue<ushort>(-42), G_VARIANT_TYPE_UINT16));
    }

    void testInt32ToGVariant()
    {
        // Int32
        QVERIFY(compare(QVariant(-42), G_VARIANT_TYPE_INT32));
    }

    void testUInt32ToGVariant()
    {
        // UInt32
        QVERIFY(compare(QVariant((uint)42), G_VARIANT_TYPE_UINT32));
    }

    void testInt64ToGVariant()
    {
        // Int64
        QVERIFY(compare(QVariant::fromValue<long>(-42), G_VARIANT_TYPE_INT64));
    }

    void testUInt64ToGVariant()
    {
        // UInt64
        QVERIFY(compare(QVariant::fromValue<ulong>(42), G_VARIANT_TYPE_UINT64));
    }

    void testQlongLongToGVariant()
    {
        // Int64
        QVERIFY(compare(QVariant::fromValue<qlonglong>(-42), G_VARIANT_TYPE_INT64));
    }

    void testUQlongLongToGVariant()
    {
        // UInt64
        QVERIFY(compare(QVariant::fromValue<qulonglong>(42), G_VARIANT_TYPE_UINT64));
    }

    void testDoubleToGVariant()
    {
        // Double
        QVERIFY(compare(QVariant((double)42.42), G_VARIANT_TYPE_DOUBLE));
    }

    void testStringToGVariant()
    {
        // String
        QVERIFY(compare(QVariant(QString("42")), G_VARIANT_TYPE_STRING));
    }

    void testByteArrayToGVariant()
    {
        // ByteArray
        QVERIFY(compare(QVariant(QByteArray("42")), G_VARIANT_TYPE_BYTESTRING));
    }

    void testMapToGVariant()
    {
        // Map
        QVERIFY(compare(QVariantMap({{"fooBar", 0xdeadbeef}}), G_VARIANT_TYPE_VARDICT));
    }

    void testTupleConversion()
    {
        QVariantList qTuple;
        qTuple << 1 << "2" << 3.3;

        GVariant *gTuple = Converter::toGVariant(qTuple);
        QVERIFY(g_variant_type_is_tuple(g_variant_get_type(gTuple)));
        QCOMPARE(g_variant_n_children(gTuple), (gsize)3);

        GVariant *v = g_variant_get_child_value(gTuple, 0);
        int v0 = g_variant_get_int32(v);
        QCOMPARE(v0, 1);
        g_variant_unref(v);

        v = g_variant_get_child_value(gTuple, 1);
        const gchar *v1 = g_variant_get_string(v, NULL);
        QCOMPARE(QString(v1), QString("2"));
        g_variant_unref(v);

        v = g_variant_get_child_value(gTuple, 2);
        gdouble v2 = g_variant_get_double(v);
        QCOMPARE(v2, 3.3);
        g_variant_unref(v);

        g_variant_unref(gTuple);
    }

    void testSchemaConvert()
    {
        // convert to integer
        compareWithSchema(QVariant::fromValue<int>(1), "i");
        compareWithSchema(QVariant::fromValue<double>(1.1), "i");

        // convert to integer
        compareWithSchema(QVariant::fromValue<bool>(true), "b");
        compareWithSchema(QVariant::fromValue<int>(1), "b");

        // convert to double
        compareWithSchema(QVariant::fromValue<double>(1.0), "d");
        compareWithSchema(QVariant::fromValue<int>(1), "d");

        // convert to string
        compareWithSchema(QVariant::fromValue<int>(1), "s");
        compareWithSchema(QVariant::fromValue<double>(1.1), "s");

        // convert to tuple
        compareWithSchema(QVariantList() << QVariant::fromValue<bool>(true) << QVariant::fromValue<int>(1) << QVariant::fromValue<int>(1) << QVariant::fromValue<QString>("test1"), "(bdis)");

        // convert to array
        compareWithSchema(QVariantList() << QVariant::fromValue<int>(1) << QVariant::fromValue<int>(1), "ad");
        compareWithSchema(QVariantList() << QVariant::fromValue<QString>("test1") << QVariant::fromValue<QString>("test2"), "as");

        // convert to array of tuple
        QVariantList si1(QVariantList() << QVariant::fromValue<QString>("test1") << QVariant::fromValue<int>(1));
        QVariantList si2(QVariantList() << QVariant::fromValue<QString>("test1") << QVariant::fromValue<int>(1));
        compareWithSchema(QVariantList() << QVariant::fromValue(si1) << QVariant::fromValue(si2), "a(sd)");

        // convert to vardict
        QVariantMap map;
        map["test1"] = QVariant::fromValue<int>(1);
        map["test2"] = QVariant::fromValue<double>(1);
        compareWithSchema(map, "a{sv}");
    }

};

QTEST_MAIN(ConverterTest)

#include "convertertest.moc"
