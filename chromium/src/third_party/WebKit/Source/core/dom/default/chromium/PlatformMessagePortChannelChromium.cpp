/*
 * Copyright (C) 2009 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "core/dom/default/chromium/PlatformMessagePortChannelChromium.h"

#include "bindings/v8/SerializedScriptValue.h"
#include "core/dom/MessagePort.h"

#include "public/platform/Platform.h"
#include "public/platform/WebMessagePortChannel.h"
#include "public/platform/WebString.h"

namespace WebCore {

PassOwnPtr<MessagePortChannel> MessagePortChannel::create(PassRefPtr<PlatformMessagePortChannel> channel)
{
    return adoptPtr(new MessagePortChannel(channel));
}

void MessagePortChannel::createChannel(PassRefPtr<MessagePort> port1, PassRefPtr<MessagePort> port2)
{
    PlatformMessagePortChannel::createChannel(port1, port2);
}

MessagePortChannel::MessagePortChannel(PassRefPtr<PlatformMessagePortChannel> channel)
    : m_channel(channel)
{
}

MessagePortChannel::~MessagePortChannel()
{
    // Make sure we close our platform channel when the base is freed, to keep the channel objects from leaking.
    m_channel->close();
}

bool MessagePortChannel::entangleIfOpen(MessagePort* port)
{
    return m_channel->entangleIfOpen(port);
}

void MessagePortChannel::disentangle()
{
    m_channel->disentangle();
}

void MessagePortChannel::postMessageToRemote(PassRefPtr<SerializedScriptValue> message, PassOwnPtr<MessagePortChannelArray> channels)
{
    m_channel->postMessageToRemote(message, channels);
}

bool MessagePortChannel::tryGetMessageFromRemote(RefPtr<SerializedScriptValue>& message, OwnPtr<MessagePortChannelArray>& channels)
{
    return m_channel->tryGetMessageFromRemote(message, channels);
}

void MessagePortChannel::close()
{
    m_channel->close();
}

bool MessagePortChannel::isConnectedTo(MessagePort* port)
{
    return m_channel->isConnectedTo(port);
}

bool MessagePortChannel::hasPendingActivity()
{
    return m_channel->hasPendingActivity();
}

MessagePort* MessagePortChannel::locallyEntangledPort(const ScriptExecutionContext* context)
{
    // This is just an optimization, so return 0 always.
    return 0;
}


PassRefPtr<PlatformMessagePortChannel> PlatformMessagePortChannel::create()
{
    return adoptRef(new PlatformMessagePortChannel());
}

PassRefPtr<PlatformMessagePortChannel> PlatformMessagePortChannel::create(
    WebKit::WebMessagePortChannel* channel)
{
    return adoptRef(new PlatformMessagePortChannel(channel));
}


PlatformMessagePortChannel::PlatformMessagePortChannel()
    : m_localPort(0)
{
    m_webChannel = WebKit::Platform::current()->createMessagePortChannel();
    if (m_webChannel)
        m_webChannel->setClient(this);
}

PlatformMessagePortChannel::PlatformMessagePortChannel(WebKit::WebMessagePortChannel* channel)
    : m_localPort(0)
    , m_webChannel(channel)
{
}

PlatformMessagePortChannel::~PlatformMessagePortChannel()
{
    if (m_webChannel)
        m_webChannel->destroy();
}

void PlatformMessagePortChannel::createChannel(PassRefPtr<MessagePort> port1, PassRefPtr<MessagePort> port2)
{
    // Create proxies for each endpoint.
    RefPtr<PlatformMessagePortChannel> channel1 = PlatformMessagePortChannel::create();
    RefPtr<PlatformMessagePortChannel> channel2 = PlatformMessagePortChannel::create();

    // Entangle the two endpoints.
    channel1->setEntangledChannel(channel2);
    channel2->setEntangledChannel(channel1);

    // Now entangle the proxies with the appropriate local ports.
    port1->entangle(MessagePortChannel::create(channel2));
    port2->entangle(MessagePortChannel::create(channel1));
}

void PlatformMessagePortChannel::messageAvailable()
{
    MutexLocker lock(m_mutex);
    if (m_localPort)
        m_localPort->messageAvailable();
}

bool PlatformMessagePortChannel::entangleIfOpen(MessagePort* port)
{
    MutexLocker lock(m_mutex);
    m_localPort = port;
    return true;
}

void PlatformMessagePortChannel::disentangle()
{
    MutexLocker lock(m_mutex);
    m_localPort = 0;
}

void PlatformMessagePortChannel::postMessageToRemote(PassRefPtr<SerializedScriptValue> message, PassOwnPtr<MessagePortChannelArray> channels)
{
    if (!m_localPort || !m_webChannel)
        return;

    WebKit::WebString messageString = message->toWireString();
    WebKit::WebMessagePortChannelArray* webChannels = 0;
    if (channels && channels->size()) {
        webChannels = new WebKit::WebMessagePortChannelArray(channels->size());
        for (size_t i = 0; i < channels->size(); ++i) {
            PlatformMessagePortChannel* platformChannel = (*channels)[i]->channel();
            (*webChannels)[i] = platformChannel->webChannelRelease();
            (*webChannels)[i]->setClient(0);
        }
    }
    m_webChannel->postMessage(messageString, webChannels);
}

bool PlatformMessagePortChannel::tryGetMessageFromRemote(RefPtr<SerializedScriptValue>& serializedMessage, OwnPtr<MessagePortChannelArray>& channels)
{
    if (!m_webChannel)
        return false;

    WebKit::WebString message;
    WebKit::WebMessagePortChannelArray webChannels;
    bool rv = m_webChannel->tryGetMessage(&message, webChannels);
    if (rv) {
        if (webChannels.size()) {
            channels = adoptPtr(new MessagePortChannelArray(webChannels.size()));
            for (size_t i = 0; i < webChannels.size(); ++i) {
                RefPtr<PlatformMessagePortChannel> platformChannel = create(webChannels[i]);
                webChannels[i]->setClient(platformChannel.get());
                (*channels)[i] = MessagePortChannel::create(platformChannel);
            }
        }
        serializedMessage = SerializedScriptValue::createFromWire(message);
    }

    return rv;
}

void PlatformMessagePortChannel::close()
{
    MutexLocker lock(m_mutex);
    // Disentangle ourselves from the other end. We still maintain a reference to m_webChannel,
    // since previously-existing messages should still be delivered.
    m_localPort = 0;
    m_entangledChannel = 0;
}

bool PlatformMessagePortChannel::isConnectedTo(MessagePort* port)
{
    MutexLocker lock(m_mutex);
    return m_entangledChannel && m_entangledChannel->m_localPort == port;
}

bool PlatformMessagePortChannel::hasPendingActivity()
{
    MutexLocker lock(m_mutex);
    return m_localPort;
}

void PlatformMessagePortChannel::setEntangledChannel(PassRefPtr<PlatformMessagePortChannel> remote)
{
    if (m_webChannel)
        m_webChannel->entangle(remote->m_webChannel);

    MutexLocker lock(m_mutex);
    m_entangledChannel = remote;
}

WebKit::WebMessagePortChannel* PlatformMessagePortChannel::webChannelRelease()
{
    WebKit::WebMessagePortChannel* rv = m_webChannel;
    m_webChannel = 0;
    return rv;
}

} // namespace WebCore
