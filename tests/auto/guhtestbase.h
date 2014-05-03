/****************************************************************************
 *                                                                          *
 *  This file is part of guh.                                               *
 *                                                                          *
 *  Guh is free software: you can redistribute it and/or modify             *
 *  it under the terms of the GNU General Public License as published by    *
 *  the Free Software Foundation, version 2 of the License.                 *
 *                                                                          *
 *  Guh is distributed in the hope that it will be useful,                  *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of          *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the           *
 *  GNU General Public License for more details.                            *
 *                                                                          *
 *  You should have received a copy of the GNU General Public License       *
 *  along with guh.  If not, see <http://www.gnu.org/licenses/>.            *
 *                                                                          *
 ***************************************************************************/

#ifndef GUHTESTBASE_H
#define GUHTESTBASE_H

#include "typeutils.h"
#include "mocktcpserver.h"

#include <QObject>
#include <QUuid>
#include <QVariantMap>
#include <QtTest>

extern VendorId guhVendorId;
extern DeviceClassId mockDeviceClassId;
extern DeviceClassId mockDeviceAutoClassId;
extern ActionTypeId mockAction1Id;
extern EventTypeId mockEvent1Id;
extern StateTypeId mockIntStateId;

class MockTcpServer;

class GuhTestBase : public QObject
{
    Q_OBJECT
public:
    explicit GuhTestBase(QObject *parent = 0);

private slots:
    void initTestCase();
    void cleanupTestCase();

protected:
    QVariant injectAndWait(const QString &method, const QVariantMap &params = QVariantMap());

protected:
    MockTcpServer *m_mockTcpServer;
    QUuid m_clientId;
    int m_commandId;

    int m_mockDevice1Port;
    int m_mockDevice2Port;

    DeviceId m_mockDeviceId;

};

#endif // GUHTESTBASE_H