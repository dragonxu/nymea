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

#include "devicepluginmock.h"
#include "httpdaemon.h"

#include "plugin/device.h"
#include "devicemanager.h"

#include <QDebug>
#include <QStringList>

DeviceClassId mockDeviceClassId = DeviceClassId("753f0d32-0468-4d08-82ed-1964aab03298");
EventTypeId mockEvent1Id = EventTypeId("45bf3752-0fc6-46b9-89fd-ffd878b5b22b");
EventTypeId mockEvent2Id = EventTypeId("863d5920-b1cf-4eb9-88bd-8f7b8583b1cf");
StateTypeId mockIntStateId = StateTypeId("80baec19-54de-4948-ac46-31eabfaceb83");
StateTypeId mockBoolStateId = StateTypeId("9dd6a97c-dfd1-43dc-acbd-367932742310");
ActionTypeId mockActionIdWithParams = ActionTypeId("dea0f4e1-65e3-4981-8eaa-2701c53a9185");
ActionTypeId mockActionIdNoParams = ActionTypeId("defd3ed6-1a0d-400b-8879-a0202cf39935");
ActionTypeId mockActionIdAsync = ActionTypeId("fbae06d3-7666-483e-a39e-ec50fe89054e");
ActionTypeId mockActionIdFailing = ActionTypeId("df3cf33d-26d5-4577-9132-9823bd33fad0");
ActionTypeId mockActionIdAsyncFailing = ActionTypeId("bfe89a1d-3497-4121-8318-e77c37537219");

DevicePluginMock::DevicePluginMock()
{
}

DevicePluginMock::~DevicePluginMock()
{
}

//QList<DeviceClass> DevicePluginMock::supportedDevices() const
//{
//    QList<DeviceClass> ret;

//    DeviceClass deviceClassMock(pluginId(), supportedVendors().first().id(), mockDeviceClassId);
//    deviceClassMock.setName("Mock Device");

//    QList<ParamType> mockParams;
//    ParamType portParam("httpport", QVariant::Int);
//    mockParams.append(portParam);

//    deviceClassMock.setParamTypes(mockParams);

//    QList<StateType> mockStates;

//    StateType intState(mockIntStateId);
//    intState.setName("Dummy int state");
//    intState.setType(QVariant::Int);
//    intState.setDefaultValue(10);
//    mockStates.append(intState);

//    StateType boolState(mockBoolStateId);
//    boolState.setName("Dummy bool state");
//    boolState.setType(QVariant::Int);
//    boolState.setDefaultValue(false);
//    mockStates.append(boolState);

//    deviceClassMock.setStateTypes(mockStates);

//    QList<EventType> mockEvents;
    
//    EventType event1(mockEvent1Id);
//    event1.setName("Mock Event 1");
//    mockEvents.append(event1);

//    EventType event2(mockEvent2Id);
//    event2.setName("Mock Event 2");
//    QList<ParamType> event2ParamTypes;
//    ParamType event2Param1Type("mockParamInt", QVariant::Int, 42);
//    event2ParamTypes.append(event2Param1Type);
//    event2.setParameters(event2ParamTypes);
//    mockEvents.append(event2);

//    deviceClassMock.setEventTypes(mockEvents);

//    QList<ActionType> mockActions;

//    mockParams.clear();
//    ActionType action1(mockActionIdWithParams);
//    action1.setName("Mock Action 1 (with params)");
//    ParamType mockActionParam1("mockActionParam1", QVariant::Int);
//    mockParams.append(mockActionParam1);
//    ParamType mockActionParam2("mockActionParam2", QVariant::Bool);
//    mockParams.append(mockActionParam2);
//    action1.setParameters(mockParams);
//    mockActions.append(action1);

//    ActionType action2(mockActionIdNoParams);
//    action2.setName("Mock Action 3 (without params)");
//    mockActions.append(action2);

//    ActionType action3(mockActionIdAsync);
//    action3.setName("Mock Action 3 (async)");
//    mockActions.append(action3);

//    ActionType action4(mockActionIdFailing);
//    action4.setName("Mock Action 4 (broken)");
//    mockActions.append(action4);

//    ActionType action5(mockActionIdAsyncFailing);
//    action5.setName("Mock Action 5 (async, broken)");
//    mockActions.append(action4);

//    deviceClassMock.setActions(mockActions);

//    ret.append(deviceClassMock);

//    // Auto created mock device
//    DeviceClass deviceClassMockAuto(pluginId(), supportedVendors().first().id(), mockDeviceAutoClassId);
//    deviceClassMockAuto.setName("Mock Device (Auto created)");
//    deviceClassMockAuto.setCreateMethod(DeviceClass::CreateMethodAuto);

//    mockParams.clear();
//    deviceClassMockAuto.setParamTypes(mockParams);
//    deviceClassMockAuto.setStateTypes(mockStates);
//    deviceClassMockAuto.setEventTypes(mockEvents);
//    deviceClassMockAuto.setActions(mockActions);

//    ret.append(deviceClassMockAuto);

//    // Discovery created device
//    DeviceClass deviceClassMockDiscovery(pluginId(), supportedVendors().first().id(), mockDeviceDiscoveryClassId);
//    deviceClassMockDiscovery.setName("Mock Device (Discovery created)");
//    deviceClassMockDiscovery.setCreateMethod(DeviceClass::CreateMethodDiscovery);
//    QList<ParamType> paramTypes;
//    ParamType paramType = ParamType("resultCount", QVariant::Int, 2);
//    paramType.setAllowedValues(QList<QVariant>() << 1 << 2);
//    paramTypes.append(paramType);
//    deviceClassMockDiscovery.setDiscoveryParamTypes(paramTypes);

//    mockParams.clear();
//    mockParams.append(portParam);
//    deviceClassMockDiscovery.setParamTypes(mockParams);
//    deviceClassMockDiscovery.setStateTypes(mockStates);
//    deviceClassMockDiscovery.setEventTypes(mockEvents);
//    deviceClassMockDiscovery.setActions(mockActions);

//    ret.append(deviceClassMockDiscovery);

//    // Async setup device
//    DeviceClass deviceClassMockAsync(pluginId(), supportedVendors().first().id(), mockDeviceAsyncSetupClassId);
//    deviceClassMockAsync.setName("Mock Device (Async)");
//    deviceClassMockAsync.setCreateMethod(DeviceClass::CreateMethodUser);

//    deviceClassMockAsync.setParamTypes(mockParams);
//    deviceClassMockAsync.setStateTypes(mockStates);
//    deviceClassMockAsync.setEventTypes(mockEvents);
//    deviceClassMockAsync.setActions(mockActions);

//    ret.append(deviceClassMockAsync);

//    // Async setup device
//    DeviceClass deviceClassMockBroken(pluginId(), supportedVendors().first().id(), mockDeviceBrokenClassId);
//    deviceClassMockBroken.setName("Mock Device (Broken setup)");
//    deviceClassMockBroken.setCreateMethod(DeviceClass::CreateMethodUser);

//    deviceClassMockBroken.setParamTypes(mockParams);
//    deviceClassMockBroken.setStateTypes(mockStates);
//    deviceClassMockBroken.setEventTypes(mockEvents);
//    deviceClassMockBroken.setActions(mockActions);

//    ret.append(deviceClassMockBroken);

//    // Broken Async setup device
//    DeviceClass deviceClassMockBrokenAsyncSetup(pluginId(), supportedVendors().first().id(), mockDeviceBrokenAsyncSetupClassId);
//    deviceClassMockBrokenAsyncSetup.setName("Mock Device (Async Broken setup)");
//    deviceClassMockBrokenAsyncSetup.setCreateMethod(DeviceClass::CreateMethodUser);

//    deviceClassMockBrokenAsyncSetup.setParamTypes(mockParams);
//    deviceClassMockBrokenAsyncSetup.setStateTypes(mockStates);
//    deviceClassMockBrokenAsyncSetup.setEventTypes(mockEvents);
//    deviceClassMockBrokenAsyncSetup.setActions(mockActions);

//    ret.append(deviceClassMockBrokenAsyncSetup);

//    return ret;
//}

DeviceManager::HardwareResources DevicePluginMock::requiredHardware() const
{
    return DeviceManager::HardwareResourceTimer;
}

QPair<DeviceManager::DeviceError, QString> DevicePluginMock::discoverDevices(const DeviceClassId &deviceClassId, const ParamList &params)
{
    Q_UNUSED(deviceClassId)
    qDebug() << "starting mock discovery:" << params;
    m_discoveredDeviceCount = params.paramValue("resultCount").toInt();
    QTimer::singleShot(1000, this, SLOT(emitDevicesDiscovered()));
    return report(DeviceManager::DeviceErrorNoError);
}

QPair<DeviceManager::DeviceSetupStatus, QString> DevicePluginMock::setupDevice(Device *device)
{
    qDebug() << "Mockdevice created returning true" << device->paramValue("httpport").toInt() << device->paramValue("async").toBool() << device->paramValue("broken").toBool();

    if (device->paramValue("broken").toBool()) {
        return reportDeviceSetup(DeviceManager::DeviceSetupStatusFailure, "This device is intentionally broken.");
    }

    HttpDaemon *daemon = new HttpDaemon(device, this);
    m_daemons.insert(device, daemon);

    if (!daemon->isListening()) {
        qDebug() << "HTTP port opening failed.";
        return reportDeviceSetup(DeviceManager::DeviceSetupStatusFailure, QString("Could not bind port."));
    }

    connect(daemon, &HttpDaemon::triggerEvent, this, &DevicePluginMock::triggerEvent);
    connect(daemon, &HttpDaemon::setState, this, &DevicePluginMock::setState);

    if (device->paramValue("async").toBool()) {
        m_asyncSetupDevices.append(device);
        QTimer::singleShot(1000, this, SLOT(emitDeviceSetupFinished()));
        return reportDeviceSetup(DeviceManager::DeviceSetupStatusAsync);
    }
    return reportDeviceSetup();
}

void DevicePluginMock::deviceRemoved(Device *device)
{
    delete m_daemons.take(device);
}

void DevicePluginMock::startMonitoringAutoDevices()
{
    foreach (Device *device, myDevices()) {
        if (device->paramValue("auto").toBool()) {
            return; // We already have a Auto Mock device... do nothing.
        }
    }

    DeviceDescriptor mockDescriptor(mockDeviceClassId, "Mock Device (Auto created)");

    ParamList params;
    qsrand(QDateTime::currentMSecsSinceEpoch());
    int port = 4242 + (qrand() % 1000);
    Param param("httpport", port);
    params.append(param);
    mockDescriptor.setParams(params);

    QList<DeviceDescriptor> deviceDescriptorList;
    deviceDescriptorList.append(mockDescriptor);

    emit autoDevicesAppeared(mockDeviceClassId, deviceDescriptorList);
}

QList<ParamType> DevicePluginMock::configurationDescription() const
{
    QList<ParamType> params;
    ParamType mockParam1("configParamInt", QVariant::Int, 42);
    mockParam1.setLimits(1, 50);
    params.append(mockParam1);

    ParamType mockParam2("configParamBool", QVariant::Bool, true);
    params.append(mockParam2);

    return params;
}

QPair<DeviceManager::DeviceError, QString> DevicePluginMock::executeAction(Device *device, const Action &action)
{
    if (!myDevices().contains(device)) {
        qWarning() << "Should execute action for a device which doesn't seem to be mine.";
        return report(DeviceManager::DeviceErrorDeviceNotFound, "Should execute an action for a device which doesn't seem to be mine.");
    }

    if (action.actionTypeId() == mockActionIdAsync || action.actionTypeId() == mockActionIdAsyncFailing) {
        m_asyncActions.append(qMakePair<Action, Device*>(action, device));
        QTimer::singleShot(1000, this, SLOT(emitActionExecuted()));
        return report(DeviceManager::DeviceErrorAsync);
    }

    if (action.actionTypeId() == mockActionIdFailing) {
        return report(DeviceManager::DeviceErrorSetupFailed);
    }

    qDebug() << "Should execute action" << action.actionTypeId();
    m_daemons.value(device)->actionExecuted(action.actionTypeId());
    return report();
}

void DevicePluginMock::setState(const StateTypeId &stateTypeId, const QVariant &value)
{
    HttpDaemon *daemon = qobject_cast<HttpDaemon*>(sender());
    if (!daemon) {
        return;
    }

    Device *device = m_daemons.key(daemon);
    device->setStateValue(stateTypeId, value);
}

void DevicePluginMock::triggerEvent(const EventTypeId &id)
{
    HttpDaemon *daemon = qobject_cast<HttpDaemon*>(sender());
    if (!daemon) {
        return;
    }

    Device *device = m_daemons.key(daemon);

    Event event(id, device->id());

    qDebug() << "Emitting event " << event.eventTypeId();
    emit emitEvent(event);
}

void DevicePluginMock::emitDevicesDiscovered()
{
    QList<DeviceDescriptor> deviceDescriptors;

    if (m_discoveredDeviceCount > 0) {
        DeviceDescriptor d1(mockDeviceClassId, "Mock Device (Discovered)");
        ParamList params;
        Param httpParam("httpport", "55555");
        params.append(httpParam);
        d1.setParams(params);
        deviceDescriptors.append(d1);
    }

    if (m_discoveredDeviceCount > 1) {
        DeviceDescriptor d2(mockDeviceClassId, "Mock Device (Discovered)");
        ParamList params;
        Param httpParam("httpport", "55556");
        params.append(httpParam);
        d2.setParams(params);
        deviceDescriptors.append(d2);
    }

    emit devicesDiscovered(mockDeviceClassId, deviceDescriptors);
}

void DevicePluginMock::emitDeviceSetupFinished()
{
    qDebug() << "emitting setup finised";
    Device *device = m_asyncSetupDevices.takeFirst();
    if (device->paramValue("broken").toBool()) {
        emit deviceSetupFinished(device, DeviceManager::DeviceSetupStatusFailure, QString("This device is intentionally broken"));
    } else {
        emit deviceSetupFinished(device, DeviceManager::DeviceSetupStatusSuccess, QString());
    }
}

void DevicePluginMock::emitActionExecuted()
{
    QPair<Action, Device*> action = m_asyncActions.takeFirst();
    if (action.first.actionTypeId() == mockActionIdAsync) {
        m_daemons.value(action.second)->actionExecuted(action.first.actionTypeId());
        emit actionExecutionFinished(action.first.id(), DeviceManager::DeviceErrorNoError, QString());
    } else if (action.first.actionTypeId() == mockActionIdAsyncFailing) {
        emit actionExecutionFinished(action.first.id(), DeviceManager::DeviceErrorSetupFailed, QString());
    }
}
