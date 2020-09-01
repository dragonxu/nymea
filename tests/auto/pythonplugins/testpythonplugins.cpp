/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
*
* Copyright 2013 - 2020, nymea GmbH
* Contact: contact@nymea.io
*
* This file is part of nymea.
* This project including source code and documentation is protected by
* copyright law, and remains the property of nymea GmbH. All rights, including
* reproduction, publication, editing and translation, are reserved. The use of
* this project is subject to the terms of a license agreement to be concluded
* with nymea GmbH in accordance with the terms of use of nymea GmbH, available
* under https://nymea.io/license
*
* GNU General Public License Usage
* Alternatively, this project may be redistributed and/or modified under the
* terms of the GNU General Public License as published by the Free Software
* Foundation, GNU version 3. This project is distributed in the hope that it
* will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
* of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
* Public License for more details.
*
* You should have received a copy of the GNU General Public License along with
* this project. If not, see <https://www.gnu.org/licenses/>.
*
* For any further details and any questions please contact us under
* contact@nymea.io or see our FAQ/Licensing Information on
* https://nymea.io/license/faq
*
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "nymeatestbase.h"

#include "integrations/thing.h"

ThingClassId pyMockThingClassId = ThingClassId("1761c256-99b1-41bd-988a-a76087f6a4f1");
ThingClassId pyMockDiscoveryPairingThingClassId = ThingClassId("248c5046-847b-44d0-ab7c-684ff79197dc");
ParamTypeId pyMockDiscoveryPairingResultCountDiscoveryParamTypeID = ParamTypeId("ef5f6b90-e9d8-4e77-a14d-6725cfb07116");

using namespace nymeaserver;

class TestPythonPlugins: public NymeaTestBase
{
    Q_OBJECT

private:
    inline void verifyThingError(const QVariant &response, Thing::ThingError error = Thing::ThingErrorNoError) {
        verifyError(response, "thingError", enumValueName(error));
    }

private slots:

    void initTestCase();

    void setupAndRemoveThing();
    void testDiscoverPairAndRemoveThing();


};


void TestPythonPlugins::initTestCase()
{
    NymeaTestBase::initTestCase();
    QLoggingCategory::setFilterRules("*.debug=false\n"
                                     "Tests.debug=true\n"
                                     "PyMock.debug=true\n"
                                     );
}

void TestPythonPlugins::setupAndRemoveThing()
{
    QVariantMap resultCountParam;
    resultCountParam.insert("paramTypeId", pyMockDiscoveryPairingResultCountDiscoveryParamTypeID);
    resultCountParam.insert("value", 2);

    QVariantList discoveryParams;
    discoveryParams.append(resultCountParam);

    QVariantMap params;
    params.insert("thingClassId", pyMockThingClassId);
    params.insert("name", "Py test thing");
    QVariant response = injectAndWait("Integrations.AddThing", params);

    verifyThingError(response, Thing::ThingErrorNoError);
    ThingId thingId = response.toMap().value("params").toMap().value("thingId").toUuid();
    qCDebug(dcTests()) << "New thing id" << thingId;

    params.clear();
    params.insert("thingId", thingId);
    injectAndWait("Integrations.RemoveThing", params);
    verifyThingError(response, Thing::ThingErrorNoError);
}

void TestPythonPlugins::testDiscoverPairAndRemoveThing()
{
    // Discover
    QVariantMap resultCountParam;
    resultCountParam.insert("paramTypeId", pyMockDiscoveryPairingResultCountDiscoveryParamTypeID);
    resultCountParam.insert("value", 2);

    QVariantList discoveryParams;
    discoveryParams.append(resultCountParam);

    QVariantMap params;
    params.insert("thingClassId", pyMockDiscoveryPairingThingClassId);
    params.insert("discoveryParams", discoveryParams);
    QVariant response = injectAndWait("Integrations.DiscoverThings", params);

    verifyThingError(response, Thing::ThingErrorNoError);
    QCOMPARE(response.toMap().value("params").toMap().value("thingDescriptors").toList().count(), 2);

    ThingDescriptorId descriptorId = response.toMap().value("params").toMap().value("thingDescriptors").toList().first().toMap().value("id").toUuid();

    // Pair
    params.clear();
    params.insert("thingDescriptorId", descriptorId);
    response = injectAndWait("Integrations.PairThing", params);
    verifyThingError(response, Thing::ThingErrorNoError);

    qWarning() << "respo" << response.toMap().value("params").toMap();
    PairingTransactionId transactionId = response.toMap().value("params").toMap().value("pairingTransactionId").toUuid();
    qWarning() << "transactionId" << transactionId;

    params.clear();
    params.insert("pairingTransactionId", transactionId);
    params.insert("username", "john");
    params.insert("secret", "smith");
    response = injectAndWait("Integrations.ConfirmPairing", params);
    verifyThingError(response, Thing::ThingErrorNoError);
    ThingId thingId = response.toMap().value("params").toMap().value("thingId").toUuid();

    // Remove
    params.clear();
    params.insert("thingId", thingId);
    response = injectAndWait("Integrations.RemoveThing", params);
    verifyThingError(response, Thing::ThingErrorNoError);
}

#include "testpythonplugins.moc"
QTEST_MAIN(TestPythonPlugins)
