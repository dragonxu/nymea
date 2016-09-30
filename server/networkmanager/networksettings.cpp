/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                                         *
 *  Copyright (C) 2016 Simon Stürz <simon.stuerz@guh.io>                   *
 *                                                                         *
 *  This file is part of guh.                                              *
 *                                                                         *
 *  Guh is free software: you can redistribute it and/or modify            *
 *  it under the terms of the GNU General Public License as published by   *
 *  the Free Software Foundation, version 2 of the License.                *
 *                                                                         *
 *  Guh is distributed in the hope that it will be useful,                 *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *  GNU General Public License for more details.                           *
 *                                                                         *
 *  You should have received a copy of the GNU General Public License      *
 *  along with guh. If not, see <http://www.gnu.org/licenses/>.            *
 *                                                                         *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "networksettings.h"
#include "dbus-interfaces.h"
#include "loggingcategories.h"

namespace guhserver {

NetworkSettings::NetworkSettings(QObject *parent) : QObject(parent)
{
    QDBusConnection systemBus = QDBusConnection::systemBus();
    if (!systemBus.isConnected()) {
        qCWarning(dcNetworkManager()) << "System DBus not connected";
        return;
    }

    m_settingsInterface = new QDBusInterface(serviceString, settingsPathString, settingsInterfaceString, QDBusConnection::systemBus(), this);
    if(!m_settingsInterface->isValid()) {
        qCWarning(dcNetworkManager()) << "Invalid DBus network settings interface";
        return;
    }

    QDBusConnection::systemBus().connect(serviceString, settingsPathString, settingsInterfaceString, "NewConnection", this, SLOT(connectionAdded(QDBusObjectPath)));
    QDBusConnection::systemBus().connect(serviceString, settingsPathString, settingsInterfaceString, "ConnectionRemoved", this, SLOT(connectionRemoved(QDBusObjectPath)));
    QDBusConnection::systemBus().connect(serviceString, settingsPathString, settingsInterfaceString, "PropertiesChanged", this, SLOT(propertiesChanged(QVariantMap)));

    loadConnections();

}

void NetworkSettings::loadConnections()
{
    // Get
    QDBusMessage query = m_settingsInterface->call("ListConnections");
    if(query.type() != QDBusMessage::ReplyMessage) {
        qCWarning(dcNetworkManager()) << query.errorName() << query.errorMessage();
        return;
    }

    const QDBusArgument &argument = query.arguments().at(0).value<QDBusArgument>();
    argument.beginArray();
    while(!argument.atEnd()) {
        QDBusObjectPath objectPath = qdbus_cast<QDBusObjectPath>(argument);
        connectionAdded(objectPath);
    }
    argument.endArray();

}

void NetworkSettings::connectionAdded(const QDBusObjectPath &objectPath)
{
    qCDebug(dcNetworkManager()) << "Settings: [+]" << objectPath.path();
}

void NetworkSettings::connectionRemoved(const QDBusObjectPath &objectPath)
{
    qCDebug(dcNetworkManager()) << "Settings: [-]" << objectPath.path();
}

void NetworkSettings::propertiesChanged(const QVariantMap &properties)
{
    qCDebug(dcNetworkManager()) << "Settins: properties changed" << properties;
}

}
