/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
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

#include "actionhandler.h"

#include "guhcore.h"
#include "devicemanager.h"
#include "types/action.h"

#include <QDebug>

ActionHandler::ActionHandler(QObject *parent) :
    JsonHandler(parent)
{
    QVariantMap params;
    QVariantMap returns;

    params.clear(); returns.clear();
    setDescription("ExecuteAction", "Execute a single action.");
    setParams("ExecuteAction", JsonTypes::actionDescription());
    returns.insert("success", "bool");
    returns.insert("errorMessage", "string");
    setReturns("ExecuteAction", returns);

    params.clear(); returns.clear();
    setDescription("GetActionType", "Get the ActionType for the given ActionTypeId");
    params.insert("actionTypeId", "uuid");
    setParams("GetActionType", params);
    returns.insert("deviceError", "int");
    returns.insert("o:actionType", JsonTypes::actionTypeDescription());
    setReturns("GetActionType", returns);

    connect(GuhCore::instance(), &GuhCore::actionExecuted, this, &ActionHandler::actionExecuted);
}

QString ActionHandler::name() const
{
    return "Actions";
}

JsonReply* ActionHandler::ExecuteAction(const QVariantMap &params)
{

    DeviceId deviceId(params.value("deviceId").toString());
    ActionTypeId actionTypeId(params.value("actionTypeId").toString());
    ParamList actionParams = JsonTypes::unpackParams(params.value("params").toList());

    Action action(actionTypeId, deviceId);
    action.setParams(actionParams);

    qDebug() << "actions params in json" << action.params() << params;


    DeviceManager::DeviceError status = GuhCore::instance()->executeAction(action);
    if (status == DeviceManager::DeviceErrorAsync) {
        JsonReply *reply = createAsyncReply("ExecuteAction");
        m_asyncActionExecutions.insert(action.id(), reply);
        return reply;
    }

    QVariantMap returns;
    returns.insert("deviceError", status);
    return createReply(returns);
}

JsonReply *ActionHandler::GetActionType(const QVariantMap &params) const
{
    ActionTypeId actionTypeId(params.value("actionTypeId").toString());
    foreach (const DeviceClass &deviceClass, GuhCore::instance()->supportedDevices()) {
        foreach (const ActionType &actionType, deviceClass.actionTypes()) {
            if (actionType.id() == actionTypeId) {
                QVariantMap data;
                data.insert("deviceError", DeviceManager::DeviceErrorNoError);
                data.insert("actionType", JsonTypes::packActionType(actionType));
                return createReply(data);
            }
        }
    }
    QVariantMap data;
    data.insert("deviceError", DeviceManager::DeviceErrorActionTypeNotFound);
    return createReply(data);
}

void ActionHandler::actionExecuted(const ActionId &id, DeviceManager::DeviceError status, const QString &errorMessage)
{
    if (!m_asyncActionExecutions.contains(id)) {
        return; // Not the action we are waiting for.
    }

    JsonReply *reply = m_asyncActionExecutions.take(id);
    reply->setData(statusToReply(status, errorMessage));
    reply->finished();
}

QVariantMap ActionHandler::statusToReply(DeviceManager::DeviceError status, const QString &errorMessage)
{
    QVariantMap returns;
    returns.insert("deviceError", status);
    return returns;
}
