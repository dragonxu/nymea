#!/usr/bin/python

import json
import sys


inputFile = open(sys.argv[1], "r")
outputfile = open(sys.argv[2], "w")

def out(line):
    outputfile.write("%s\n" % line)

try:
    pluginMap = json.loads(inputFile.read())
except:
    print("Error opening input file")


def extractVendors(pluginMap):
    for vendor in pluginMap['vendors']:
        try:
            vendorIdName = pluginMap["idName"];
            out("VendorId %s = VendorId(\"%s\");" % pluginMap["idName"], pluginMap["id"])
        except:
            pass
        extractDeviceClasses(vendor)


def extractDeviceClasses(vendorMap):
    for deviceClass in vendorMap["deviceClasses"]:
        print("have deviceclass %s" % deviceClass["deviceClassId"])
        try:
            deviceClassIdName = deviceClass["idName"]
            out("DeviceClassId %sDeviceClassId = DeviceClassId(\"%s\");" % (deviceClassIdName, deviceClass["deviceClassId"]))
        except:
            pass
        extractActionTypes(deviceClass)
        extractStateTypes(deviceClass)
        extractEventTypes(deviceClass)


def extractStateTypes(deviceClassMap):
    try:
        for stateType in deviceClassMap["stateTypes"]:
            try:
                stateTypeIdName = stateType["idName"]
                out("StateTypeId %sStateTypeId = StateTypeId(\"%s\");" % (stateTypeIdName, stateType["id"]))
            except:
                pass
    except:
        pass

def extractActionTypes(deviceClassMap):
    try:
        for actionType in deviceClassMap["actionTypes"]:
            try:
                actionTypeIdName = actionType["idName"]
                out("ActionTypeId %sActionTypeId = ActionTypeId(\"%s\");" % (actionTypeIdName, actionType["id"]))
            except:
                pass
    except:
        pass

def extractEventTypes(deviceClassMap):
    try:
        for eventType in deviceClassMap["eventTypes"]:
            try:
                eventTypeIdName = eventType["idName"]
                out("EventTypeId %sEventTypeId = EventTypeId(\"%s\");" % (eventTypeIdName, eventType["id"]))
            except:
                pass
    except:
        pass


# write header
out("/* This file is generated by the guh build system. Any changes to this fille")
out(" * be lost.")
out("")
out(" * If you want to change this file, edit the plugin's json file and add")
out(" * idName tags where appropriate.")
out(" */")

out("#ifndef PLUGININFO_H")
out("#define PLUGININFO_H")
out("#include \"typeutils.h\"");


out("PluginId pluginId = PluginId(\"%s\");" % pluginMap['id'])
extractVendors(pluginMap)


out("#endif")