/*
 * TransportInterface.h
 *
 * Copyright 2016-2017 Amazon.com, Inc. or its affiliates. All Rights Reserved.
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

#ifndef ALEXA_CLIENT_SDK_ACL_INCLUDE_ACL_TRANSPORT_TRANSPORT_INTERFACE_H_
#define ALEXA_CLIENT_SDK_ACL_INCLUDE_ACL_TRANSPORT_TRANSPORT_INTERFACE_H_

#include <memory>

#include "ACL/MessageRequest.h"
#include "ACL/Message.h"

namespace alexaClientSDK {
namespace acl {

/**
 * This class defines the interface which must be implemented to represent the creation and management of a
 * specific connection to AVS.
 */
class TransportInterface {
public:
    /**
     * Destructor.
     */
    virtual ~TransportInterface() = default;

    /**
     * Initiate a connection to AVS.  This function may operate asynchronously, meaning its return value
     * does not imply a successful connection, but that an attempt to connect has been successfully started.
     * This function may not be thread-safe.
     *
     * @return If connection setup was successful.
     */
    virtual bool connect() = 0;

    /**
     * Disconnect from AVS.
     * This function may not be thread-safe.
     */
    virtual void disconnect() = 0;

    /**
     * Returns whether this object is currently connected to AVS.
     *
     * @return If the object is currently connected to AVS.
     */
    virtual bool isConnected() = 0;

    /**
     * Sends an message request. This call blocks until the message can be sent.
     *
     * @param request The requested message.
     */
    virtual void send(std::shared_ptr<MessageRequest> request) = 0;

};

} // alexaClientSDK
} // acl

#endif // ALEXA_CLIENT_SDK_ACL_INCLUDE_ACL_TRANSPORT_TRANSPORT_INTERFACE_H_
