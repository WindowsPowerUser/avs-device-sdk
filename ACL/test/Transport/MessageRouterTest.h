/**
 * MessageRouterTest.h
 *
 * Copyright 2017 Amazon.com, Inc. or its affiliates. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License").
 * You may not use this file except in compliance with the License.
 * A copy of the License is located at
 *
 *     http://aws.amazon.com/apache2.0/
 *
 * or in the "license" file accompanying this file. This file is distributed
 * on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied. See the License for the specific language governing
 * permissions and limitations under the License.
 */
#ifndef ALEXA_CLIENT_SDK_ACL_TEST_TRANSPORT_ABSTRACT_MESSAGE_ROUTER_TEST_H_
#define ALEXA_CLIENT_SDK_ACL_TEST_TRANSPORT_ABSTRACT_MESSAGE_ROUTER_TEST_H_

#include <gtest/gtest.h>
#include <memory>
#include <sstream>
#include <string>

#include "AVSUtils/Threading/Executor.h"
#include "AVSUtils/Memory/Memory.h"

#include "MockMessageRouterObserver.h"
#include "MockAuthDelegate.h"
#include "MockTransport.h"
#include "ACL/Transport/MessageRouter.h"
#include "ACL/Transport/MessageConsumerInterface.h"

namespace alexaClientSDK {
namespace acl {

using namespace transport;
using namespace avsUtils::threading;
using namespace avsUtils::memory;

using namespace ::testing;

class TestableMessageRouter : public MessageRouter {
public:
    TestableMessageRouter(
            std::shared_ptr<AuthDelegateInterface> authDelegate,
            std::shared_ptr<MockTransport> transport,
            const std::string& avsEndpoint,
            std::shared_ptr<avsUtils::threading::Executor> sendExecutor,
            std::shared_ptr<avsUtils::threading::Executor> receiveExecutor)
            : MessageRouter(authDelegate, avsEndpoint, sendExecutor, receiveExecutor),
              m_mockTransport{transport} {

    }

    void setMockTransport(std::shared_ptr<MockTransport> transport) {
        m_mockTransport = transport;
    }

private:
    std::shared_ptr<TransportInterface> createTransport(
            std::shared_ptr<AuthDelegateInterface> authDelegate,
            const std::string& avsEndpoint,
            MessageConsumerInterface* messageConsumerInterface,
            TransportObserverInterface* transportObserverInterface) override {
        return m_mockTransport;
    }

    std::shared_ptr<MockTransport> m_mockTransport;
};

class MessageRouterTest : public ::testing::Test {
public:
    const std::string AVS_ENDPOINT = "AVS_ENDPOINT";

    MessageRouterTest()
            :
            m_mockMessageRouterObserver{std::make_shared<MockMessageRouterObserver>()},
            m_mockAuthDelegate{std::make_shared<MockAuthDelegate>()},
            m_mockTransport{std::make_shared<NiceMock<MockTransport>>()},
            m_sendExecutor{std::make_shared<Executor>()},
            m_receiveExecutor{std::make_shared<Executor>()},
            m_router{m_mockAuthDelegate,
                     m_mockTransport,
                     AVS_ENDPOINT,
                     m_sendExecutor,
                     m_receiveExecutor} {
        m_router.setObserver(m_mockMessageRouterObserver);
    }

    void TearDown() {
        // Wait on both executors to ensure everything is finished
        waitOnExecutor(m_sendExecutor, SHORT_TIMEOUT_MS);
        waitOnExecutor(m_receiveExecutor, SHORT_TIMEOUT_MS);
    }

    std::shared_ptr<MessageRequest> createMessageRequest() {
        auto message = createMessage();
        return std::make_shared<MessageRequest>(message);
    }

    std::shared_ptr<Message> createMessage() {
        auto attachment = std::make_shared<std::istringstream>(MESSAGE);
        auto message = std::make_shared<Message>(MESSAGE, attachment);
        return message;
    }

    void waitOnExecutor(std::shared_ptr<Executor> executor, std::chrono::milliseconds millisecondsToWait) {
        auto future = executor->submit([](){;});
        auto status = future.wait_for(millisecondsToWait);

        ASSERT_EQ(std::future_status::ready, status);
    }

    void setupStateToPending() {
        initializeMockTransport(m_mockTransport.get());
        m_router.enable();
    }

    void setupStateToConnected() {
        setupStateToPending();
        m_router.onConnected();
        connectMockTransport(m_mockTransport.get());
    }

    // Ensure the length of MESSAGE always matches MESSAGE_LENGTH - 1 (one for each char and a null terminator)
    static const std::string MESSAGE;
    static const int MESSAGE_LENGTH;
    static const std::chrono::milliseconds SHORT_TIMEOUT_MS;

    std::shared_ptr<MockMessageRouterObserver> m_mockMessageRouterObserver;
    std::shared_ptr<MockAuthDelegate> m_mockAuthDelegate;
    std::shared_ptr<NiceMock<MockTransport>> m_mockTransport;
    std::shared_ptr<avsUtils::threading::Executor> m_sendExecutor;
    std::shared_ptr<avsUtils::threading::Executor> m_receiveExecutor;
    TestableMessageRouter m_router;
};

const std::string MessageRouterTest::MESSAGE = "123456789";
const int MessageRouterTest::MESSAGE_LENGTH = 10;
const std::chrono::milliseconds MessageRouterTest::SHORT_TIMEOUT_MS = std::chrono::milliseconds(1000);

} // namespace acl
} // namespace alexaClientSDK

#endif // ALEXA_CLIENT_SDK_ACL_TEST_TRANSPORT_ABSTRACT_MESSAGE_ROUTER_TEST_H_
