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

#include "restserver.h"
#include "loggingcategories.h"
#include "httprequest.h"
#include "httpreply.h"
#include "guhcore.h"

#include <QJsonDocument>

namespace guhserver {

RestServer::RestServer(QObject *parent) :
    QObject(parent)
{
    m_webserver = new WebServer(this);
    connect(m_webserver, &WebServer::clientConnected, this, &RestServer::clientConnected);
    connect(m_webserver, &WebServer::clientDisconnected, this, &RestServer::clientDisconnected);
    connect(m_webserver, &WebServer::httpRequestReady, this, &RestServer::processHttpRequest);

    m_webserver->startServer();

    QMetaObject::invokeMethod(this, "setup", Qt::QueuedConnection);
}

void RestServer::setup()
{
    // Create resources
    m_deviceResource = new DevicesResource(this);
    m_deviceClassesResource = new DeviceClassesResource(this);
    m_vendorsResource = new VendorsResource(this);
    m_pluginsResource = new PluginsResource(this);
    m_rulesResource = new RulesResource(this);
}

void RestServer::clientConnected(const QUuid &clientId)
{
    m_clientList.append(clientId);
}

void RestServer::clientDisconnected(const QUuid &clientId)
{
    m_clientList.removeAll(clientId);
}

void RestServer::processHttpRequest(const QUuid &clientId, const HttpRequest &request)
{
    qCDebug(dcRest) << "Process HTTP request";

    QStringList urlTokens = request.url().path().split("/");
    urlTokens.removeAll(QString());

    qCDebug(dcRest) << urlTokens;

    if (urlTokens.count() < 3) {
        HttpReply *reply = new HttpReply(HttpReply::BadRequest, HttpReply::TypeSync, this);
        reply->setClientId(clientId);
        m_webserver->sendHttpReply(reply);
        reply->deleteLater();
        return;
    }

    // TODO: make generic with resource name

    if (urlTokens.at(2) == "devices") {
        HttpReply *reply = m_deviceResource->proccessRequest(request, urlTokens);
        reply->setClientId(clientId);
        if (reply->type() == HttpReply::TypeAsync) {
            connect(reply, &HttpReply::finished, this, &RestServer::asyncReplyFinished);
            m_asyncReplies.insert(clientId, reply);
            reply->startWait();
            return;
        }
        m_webserver->sendHttpReply(reply);
        reply->deleteLater();
        return;
    }

    if (urlTokens.at(2) == "deviceclasses") {
        HttpReply *reply = m_deviceClassesResource->proccessRequest(request, urlTokens);
        reply->setClientId(clientId);
        if (reply->type() == HttpReply::TypeAsync) {
            connect(reply, &HttpReply::finished, this, &RestServer::asyncReplyFinished);
            m_asyncReplies.insert(clientId, reply);
            reply->startWait();
            return;
        }
        m_webserver->sendHttpReply(reply);
        reply->deleteLater();
        return;
    }

    if (urlTokens.at(2) == "vendors") {
        HttpReply *reply = m_vendorsResource->proccessRequest(request, urlTokens);
        reply->setClientId(clientId);
        if (reply->type() == HttpReply::TypeAsync) {
            connect(reply, &HttpReply::finished, this, &RestServer::asyncReplyFinished);
            m_asyncReplies.insert(clientId, reply);
            reply->startWait();
            return;
        }
        m_webserver->sendHttpReply(reply);
        reply->deleteLater();
        return;
    }

    if (urlTokens.at(2) == "plugins") {
        HttpReply *reply = m_pluginsResource->proccessRequest(request, urlTokens);
        reply->setClientId(clientId);
        if (reply->type() == HttpReply::TypeAsync) {
            connect(reply, &HttpReply::finished, this, &RestServer::asyncReplyFinished);
            m_asyncReplies.insert(clientId, reply);
            reply->startWait();
            return;
        }
        m_webserver->sendHttpReply(reply);
        reply->deleteLater();
        return;
    }

    if (urlTokens.at(2) == "rules") {
        HttpReply *reply = m_rulesResource->proccessRequest(request, urlTokens);
        reply->setClientId(clientId);
        if (reply->type() == HttpReply::TypeAsync) {
            connect(reply, &HttpReply::finished, this, &RestServer::asyncReplyFinished);
            m_asyncReplies.insert(clientId, reply);
            reply->startWait();
            return;
        }
        m_webserver->sendHttpReply(reply);
        reply->deleteLater();
        return;
    }

    HttpReply *reply = RestResource::createErrorReply(HttpReply::BadRequest);
    m_webserver->sendHttpReply(reply);
    reply->deleteLater();
}

void RestServer::asyncReplyFinished()
{
    HttpReply *reply = qobject_cast<HttpReply*>(sender());

    qCDebug(dcWebServer) << "Async reply finished";

    if (reply->timedOut())
        reply->setHttpStatusCode(HttpReply::GatewayTimeout);

    m_webserver->sendHttpReply(reply);
    reply->deleteLater();
}


}
