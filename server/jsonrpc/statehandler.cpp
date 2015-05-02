/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                                         *
 *  Copyright (C) 2015 Simon Stuerz <simon.stuerz@guh.guru>                *
 *  Copyright (C) 2014 Michael Zanetti <michael_zanetti@gmx.net>           *
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

#include "statehandler.h"
#include "guhcore.h"

StateHandler::StateHandler(QObject *parent) :
    JsonHandler(parent)
{
    QVariantMap params;
    QVariantMap returns;

    params.clear(); returns.clear();
    setDescription("GetStateType", "Get the StateType for the given stateTypeId.");
    params.insert("stateTypeId", JsonTypes::basicTypeToString(JsonTypes::Uuid));
    setParams("GetStateType", params);
    returns.insert("deviceError", JsonTypes::deviceErrorRef());
    returns.insert("o:stateType", JsonTypes::stateTypeRef());
    setReturns("GetStateType", returns);
}

QString StateHandler::name() const
{
    return "States";
}

JsonReply* StateHandler::GetStateType(const QVariantMap &params) const
{
    StateTypeId stateTypeId(params.value("stateTypeId").toString());
    foreach (const DeviceClass &deviceClass, GuhCore::instance()->supportedDevices()) {
        foreach (const StateType &stateType, deviceClass.stateTypes()) {
            if (stateType.id() == stateTypeId) {
                QVariantMap data = statusToReply(DeviceManager::DeviceErrorNoError);
                data.insert("stateType", JsonTypes::packStateType(stateType));
                return createReply(data);
            }
        }
    }
    return createReply(statusToReply(DeviceManager::DeviceErrorStateTypeNotFound));
}
