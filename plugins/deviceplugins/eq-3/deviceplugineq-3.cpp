/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                                         *
 *  Copyright (C) 2015 Simon Stuerz <simon.stuerz@guh.guru>                *
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

/*!
    \page eq3.html
    \title eQ-3 Max!

    \ingroup plugins
    \ingroup services

    This plugin allows to find and controll devices from Max!(eQ-3). To use this devices, you need at least
    one \l{http://www.eq-3.de/max-heizungssteuerung-produktdetail/items/bc-lgw-o-tw.html}{Max! Cube LAN Gateway}
    in you local network. Once the cube is connected (DHCP), you can auto detect the cube in the network and
    add it to your \l{https://guh.guru}{guh} devices. Also more than one cube in the network is supported. All
    devices, which are connected to a cube, will be autogenerated. For the setup of a cube, the original
    software is recomanded (min/max setpoint temperature, weekly programm...).

    \chapter Supported devices
        \section2 Max! Cube LAN Gateway
            This \l{http://www.eq-3.de/max-heizungssteuerung-produktdetail/
            items/bc-lgw-o-tw.html}{cube} can be discovered in the network. Every
            device, which is connected with the cube, will be appear automatically, once the cube is configrued and
            added to guh.

        \section2 Max! Wall Thermostat
            In order to use this device, you need a \l{http://www.eq-3.de/max-heizungssteuerung-produktdetail/
            items/bc-lgw-o-tw.html}{Max! Cube LAN Gateway}. A \l{http://www.eq-3.de/max-raumloesung-produktdetail/items/bc-tc-c-wm.html}{MAX! Wall Thermostat} can not be added,
            it will appear automatically in the device list, once you add it to the cube.

        \section2 Max! Radiator Thermostat
            In order to use this device, you need a \l{http://www.eq-3.de/max-heizungssteuerung-produktdetail/
            items/bc-lgw-o-tw.html}{Max! Cube LAN Gateway}. A \l{http://www.eq-3.de/max-heizungssteuerung-produktdetail/items/bc-rt-trx-cyg.html}{MAX! Radiator Thermostat} can not be added,
            it will appear automatically in the device list, once you add it to the cube.

    \chapter Plugin properties
    Following JSON file contains the definition and the description of all available \l{DeviceClass}{DeviceClasses}
    and \l{Vendor}{Vendors} of this \l{DevicePlugin}.

    Each \l{DeviceClass} has a list of \l{ParamType}{paramTypes}, \l{ActionType}{actionTypes}, \l{StateType}{stateTypes}
    and \l{EventType}{eventTypes}. The \l{DeviceClass::CreateMethod}{createMethods} parameter describes how the \l{Device}
    will be created in the system. A device can have more than one \l{DeviceClass::CreateMethod}{CreateMethod}.
    The \l{DeviceClass::SetupMethod}{setupMethod} describes the setup method of the \l{Device}.
    The detailed implementation of each \l{DeviceClass} can be found in the source code.

    \note If a \l{StateType} has the parameter \tt{"writable": true}, an \l{ActionType} with the same uuid and \l{ParamType}{ParamTypes}
    will be created automatically.

    \quotefile plugins/deviceplugins/eq-3/deviceplugineq-3.json
*/


#include "deviceplugineq-3.h"

#include "plugin/device.h"
#include "devicemanager.h"
#include "types/param.h"
#include "plugininfo.h"

#include <QDebug>

DevicePluginEQ3::DevicePluginEQ3()
{
    m_cubeDiscovery = new MaxCubeDiscovery(this);

    connect(m_cubeDiscovery,SIGNAL(cubesDetected(QList<MaxCube*>)),this,SLOT(discoveryDone(QList<MaxCube*>)));
}

DeviceManager::HardwareResources DevicePluginEQ3::requiredHardware() const
{
    return DeviceManager::HardwareResourceTimer;
}

QList<ParamType> DevicePluginEQ3::configurationDescription() const
{
    QList<ParamType> params;
    return params;
}

DeviceManager::DeviceError DevicePluginEQ3::discoverDevices(const DeviceClassId &deviceClassId, const ParamList &params)
{
    Q_UNUSED(params)
    if(deviceClassId == cubeDeviceClassId){
        m_cubeDiscovery->detectCubes();
        return DeviceManager::DeviceErrorAsync;
    }
    return DeviceManager::DeviceErrorDeviceClassNotFound;
}

void DevicePluginEQ3::startMonitoringAutoDevices()
{

}

DeviceManager::DeviceSetupStatus DevicePluginEQ3::setupDevice(Device *device)
{
    qDebug() << "setupDevice" << device->params();

    if(device->deviceClassId() == cubeDeviceClassId){
        foreach (MaxCube *cube, m_cubes.keys()) {
            if(cube->serialNumber() == device->paramValue("serial number").toString()){
                qDebug() << cube->serialNumber() << " already exists...";
                return DeviceManager::DeviceSetupStatusFailure;
            }
        }

        MaxCube *cube = new MaxCube(this,device->paramValue("serial number").toString(),QHostAddress(device->paramValue("host address").toString()),device->paramValue("port").toInt());
        m_cubes.insert(cube,device);

        connect(cube,SIGNAL(cubeConnectionStatusChanged(bool)),this,SLOT(cubeConnectionStatusChanged(bool)));
        connect(cube,SIGNAL(commandActionFinished(bool,ActionId)),this,SLOT(commandActionFinished(bool,ActionId)));
        connect(cube,SIGNAL(cubeConfigReady()),this,SLOT(updateCubeConfig()));
        connect(cube,SIGNAL(wallThermostatFound()),this,SLOT(wallThermostatFound()));
        connect(cube,SIGNAL(wallThermostatDataUpdated()),this,SLOT(wallThermostatDataUpdated()));
        connect(cube,SIGNAL(radiatorThermostatFound()),this,SLOT(radiatorThermostatFound()));
        connect(cube,SIGNAL(radiatorThermostatDataUpdated()),this,SLOT(radiatorThermostatDataUpdated()));

        cube->connectToCube();

        return DeviceManager::DeviceSetupStatusAsync;
    }
    if(device->deviceClassId() == wallThermostateDeviceClassId){
        device->setName("Max! Wall Thermostat (" + device->paramValue("serial number").toString() + ")");
    }

    return DeviceManager::DeviceSetupStatusSuccess;
}

void DevicePluginEQ3::deviceRemoved(Device *device)
{
    if (!m_cubes.values().contains(device)) {
        return;
    }

    MaxCube *cube = m_cubes.key(device);
    cube->disconnectFromCube();
    qDebug() << "remove cube " << cube->serialNumber();
    m_cubes.remove(cube);
    cube->deleteLater();
}

void DevicePluginEQ3::guhTimer()
{
    foreach (MaxCube *cube, m_cubes.keys()){
        if(cube->isConnected() && cube->isInitialized()){
            cube->refresh();
        }
    }
}

DeviceManager::DeviceError DevicePluginEQ3::executeAction(Device *device, const Action &action)
{    
    if(device->deviceClassId() == wallThermostateDeviceClassId || device->deviceClassId() == radiatorThermostateDeviceClassId){
        foreach (MaxCube *cube, m_cubes.keys()){
            if(cube->serialNumber() == device->paramValue("parent cube").toString()){

                QByteArray rfAddress = device->paramValue("rf address").toByteArray();
                int roomId = device->paramValue("room id").toInt();

                if (action.actionTypeId() == desiredTemperatureActionTypeId){
                    cube->setDeviceSetpointTemp(rfAddress, roomId, action.param("desired temperature").value().toDouble(), action.id());
                } else if (action.actionTypeId() == setAutoModeActionTypeId){
                    cube->setDeviceAutoMode(rfAddress, roomId, action.id());
                } else if (action.actionTypeId() == setManualModeActionTypeId){
                    cube->setDeviceManuelMode(rfAddress, roomId, action.id());
                } else if (action.actionTypeId() == setEcoModeActionTypeId){
                    cube->setDeviceEcoMode(rfAddress, roomId, action.id());
                } else if (action.actionTypeId() == displayCurrentTempActionTypeId){
                    cube->displayCurrentTemperature(rfAddress, roomId, action.param("display").value().toBool(), action.id());
                }
                return DeviceManager::DeviceErrorAsync;
            }
        }
    }

    return DeviceManager::DeviceErrorActionTypeNotFound;
}

void DevicePluginEQ3::cubeConnectionStatusChanged(const bool &connected)
{
    if(connected){
        MaxCube *cube = static_cast<MaxCube*>(sender());
        Device *device;
        if (m_cubes.contains(cube)) {
            device = m_cubes.value(cube);
            device->setName("Max! Cube " + cube->serialNumber());
            device->setStateValue(connectionStateTypeId,true);
            emit deviceSetupFinished(device, DeviceManager::DeviceSetupStatusSuccess);
        }
    }else{
        MaxCube *cube = static_cast<MaxCube*>(sender());
        Device *device;
        if (m_cubes.contains(cube)){
            device = m_cubes.value(cube);
            device->setStateValue(connectionStateTypeId,false);
            emit deviceSetupFinished(device, DeviceManager::DeviceSetupStatusFailure);
        }
    }
}

void DevicePluginEQ3::discoveryDone(const QList<MaxCube *> &cubeList)
{
    QList<DeviceDescriptor> retList;
    foreach (MaxCube *cube, cubeList) {
        DeviceDescriptor descriptor(cubeDeviceClassId, "Max! Cube LAN Gateway",cube->serialNumber());
        ParamList params;
        Param hostParam("host address", cube->hostAddress().toString());
        params.append(hostParam);
        Param portParam("port", cube->port());
        params.append(portParam);
        Param firmwareParam("firmware version", cube->firmware());
        params.append(firmwareParam);
        Param serialNumberParam("serial number", cube->serialNumber());
        params.append(serialNumberParam);

        descriptor.setParams(params);
        retList.append(descriptor);
    }
    emit devicesDiscovered(cubeDeviceClassId,retList);
}

void DevicePluginEQ3::commandActionFinished(const bool &succeeded, const ActionId &actionId)
{
    if(succeeded){
        emit actionExecutionFinished(actionId, DeviceManager::DeviceErrorNoError);
    }else{
        emit actionExecutionFinished(actionId, DeviceManager::DeviceErrorSetupFailed);
    }
}

void DevicePluginEQ3::wallThermostatFound()
{
    MaxCube *cube = static_cast<MaxCube*>(sender());

    QList<DeviceDescriptor> descriptorList;

    foreach (WallThermostat *wallThermostat, cube->wallThermostatList()) {
        bool allreadyAdded = false;
        foreach (Device *device, deviceManager()->findConfiguredDevices(wallThermostateDeviceClassId)){
            if(wallThermostat->serialNumber() == device->paramValue("serial number").toString()){
                allreadyAdded = true;
                break;
            }
        }
        if(!allreadyAdded){
            DeviceDescriptor descriptor(wallThermostateDeviceClassId, wallThermostat->serialNumber());
            ParamList params;
            params.append(Param("name", wallThermostat->deviceName()));
            params.append(Param("parent cube", cube->serialNumber()));
            params.append(Param("serial number", wallThermostat->serialNumber()));
            params.append(Param("rf address", wallThermostat->rfAddress()));
            params.append(Param("room id", wallThermostat->roomId()));
            params.append(Param("room name", wallThermostat->roomName()));
            descriptor.setParams(params);
            descriptorList.append(descriptor);
        }
    }

    if(!descriptorList.isEmpty()){
        metaObject()->invokeMethod(this, "autoDevicesAppeared", Qt::QueuedConnection, Q_ARG(DeviceClassId, wallThermostateDeviceClassId), Q_ARG(QList<DeviceDescriptor>, descriptorList));
    }

}

void DevicePluginEQ3::radiatorThermostatFound()
{
    MaxCube *cube = static_cast<MaxCube*>(sender());

    QList<DeviceDescriptor> descriptorList;

    foreach (RadiatorThermostat *radiatorThermostat, cube->radiatorThermostatList()) {
        bool allreadyAdded = false;
        foreach (Device *device, deviceManager()->findConfiguredDevices(radiatorThermostateDeviceClassId)){
            if(radiatorThermostat->serialNumber() == device->paramValue("serial number").toString()){
                allreadyAdded = true;
                break;
            }
        }
        if(!allreadyAdded){
            DeviceDescriptor descriptor(radiatorThermostateDeviceClassId, radiatorThermostat->serialNumber());
            ParamList params;
            params.append(Param("name", radiatorThermostat->deviceName()));
            params.append(Param("parent cube", cube->serialNumber()));
            params.append(Param("serial number", radiatorThermostat->serialNumber()));
            params.append(Param("rf address", radiatorThermostat->rfAddress()));
            params.append(Param("room id", radiatorThermostat->roomId()));
            params.append(Param("room name", radiatorThermostat->roomName()));
            descriptor.setParams(params);
            descriptorList.append(descriptor);
        }
    }

    if(!descriptorList.isEmpty()){
        metaObject()->invokeMethod(this, "autoDevicesAppeared", Qt::QueuedConnection, Q_ARG(DeviceClassId, radiatorThermostateDeviceClassId), Q_ARG(QList<DeviceDescriptor>, descriptorList));
    }
}

void DevicePluginEQ3::updateCubeConfig()
{
    MaxCube *cube = static_cast<MaxCube*>(sender());
    Device *device;
    if (m_cubes.contains(cube)) {
        device = m_cubes.value(cube);
        device->setStateValue(portalEnabledStateTypeId,cube->portalEnabeld());
        return;
    }
}

void DevicePluginEQ3::wallThermostatDataUpdated()
{
    MaxCube *cube = static_cast<MaxCube*>(sender());

    foreach (WallThermostat *wallThermostat, cube->wallThermostatList()) {
        foreach (Device *device, deviceManager()->findConfiguredDevices(wallThermostateDeviceClassId)){
            if(device->paramValue("serial number").toString() == wallThermostat->serialNumber()){
                device->setStateValue(comfortTempStateTypeId, wallThermostat->comfortTemp());
                device->setStateValue(ecoTempStateTypeId, wallThermostat->ecoTemp());
                device->setStateValue(maxSetpointTempStateTypeId, wallThermostat->maxSetPointTemp());
                device->setStateValue(minSetpointTempStateTypeId, wallThermostat->minSetPointTemp());
                device->setStateValue(errorOccurredStateTypeId, wallThermostat->errorOccured());
                device->setStateValue(initializedStateTypeId, wallThermostat->initialized());
                device->setStateValue(batteryLowStateTypeId, wallThermostat->batteryLow());
                device->setStateValue(linkStatusOKStateTypeId, wallThermostat->linkStatusOK());
                device->setStateValue(panelLockedStateTypeId, wallThermostat->panelLocked());
                device->setStateValue(gatewayKnownStateTypeId, wallThermostat->gatewayKnown());
                device->setStateValue(dtsActiveStateTypeId, wallThermostat->dtsActive());
                device->setStateValue(deviceModeStateTypeId, wallThermostat->deviceMode());
                device->setStateValue(deviceModeStringStateTypeId, wallThermostat->deviceModeString());
                device->setStateValue(desiredTemperatureStateTypeId, wallThermostat->setpointTemperature());
                device->setStateValue(currentTemperatureStateTypeId, wallThermostat->currentTemperature());


            }
        }
    }
}

void DevicePluginEQ3::radiatorThermostatDataUpdated()
{
    MaxCube *cube = static_cast<MaxCube*>(sender());

    foreach (RadiatorThermostat *radiatorThermostat, cube->radiatorThermostatList()) {
        foreach (Device *device, deviceManager()->findConfiguredDevices(radiatorThermostateDeviceClassId)){
            if(device->paramValue("serial number").toString() == radiatorThermostat->serialNumber()){
                device->setStateValue(comfortTempStateTypeId, radiatorThermostat->comfortTemp());
                device->setStateValue(ecoTempStateTypeId, radiatorThermostat->ecoTemp());
                device->setStateValue(maxSetpointTempStateTypeId, radiatorThermostat->maxSetPointTemp());
                device->setStateValue(minSetpointTempStateTypeId, radiatorThermostat->minSetPointTemp());
                device->setStateValue(errorOccurredStateTypeId, radiatorThermostat->errorOccured());
                device->setStateValue(initializedStateTypeId, radiatorThermostat->initialized());
                device->setStateValue(batteryLowStateTypeId, radiatorThermostat->batteryLow());
                device->setStateValue(linkStatusOKStateTypeId, radiatorThermostat->linkStatusOK());
                device->setStateValue(panelLockedStateTypeId, radiatorThermostat->panelLocked());
                device->setStateValue(gatewayKnownStateTypeId, radiatorThermostat->gatewayKnown());
                device->setStateValue(dtsActiveStateTypeId, radiatorThermostat->dtsActive());
                device->setStateValue(deviceModeStateTypeId, radiatorThermostat->deviceMode());
                device->setStateValue(deviceModeStringStateTypeId, radiatorThermostat->deviceModeString());
                device->setStateValue(desiredTemperatureStateTypeId, radiatorThermostat->setpointTemperature());
                device->setStateValue(offsetTempStateTypeId, radiatorThermostat->offsetTemp());
                device->setStateValue(windowOpenDurationStateTypeId, radiatorThermostat->windowOpenDuration());
                device->setStateValue(boostValveValueStateTypeId, radiatorThermostat->boostValveValue());
                device->setStateValue(boostDurationStateTypeId, radiatorThermostat->boostDuration());
                device->setStateValue(discalcWeekDayStateTypeId, radiatorThermostat->discalcingWeekDay());
                device->setStateValue(discalcTimeStateTypeId, radiatorThermostat->discalcingTime().toString("HH:mm"));
                device->setStateValue(valveMaximumSettingsStateTypeId, radiatorThermostat->valveMaximumSettings());
                device->setStateValue(valveOffsetStateTypeId, radiatorThermostat->valveOffset());
                device->setStateValue(valvePositionStateTypeId, radiatorThermostat->valvePosition());
            }
        }
    }
}
