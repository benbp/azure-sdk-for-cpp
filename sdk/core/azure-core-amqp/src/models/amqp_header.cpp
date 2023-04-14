// Copyright (c) Microsoft Corporation. All rights reserved.
// SPDX-Licence-Identifier: MIT

#include "azure/core/amqp/models/amqp_header.hpp"

#include <azure_uamqp_c/amqp_definitions_milliseconds.h>

#include <azure_uamqp_c/amqp_definitions_header.h>
#include <chrono>
#include <iostream>

void Azure::Core::_internal::UniqueHandleHelper<HEADER_INSTANCE_TAG>::FreeAmqpHeader(
    HEADER_HANDLE handle)
{
  header_destroy(handle);
}

namespace Azure { namespace Core { namespace Amqp { namespace Models {

  MessageHeader::MessageHeader(HEADER_HANDLE handle)
  {
    bool boolValue;
    if (!header_get_durable(handle, &boolValue))
    {
      Durable = boolValue;
    }

    uint8_t uint8Value;
    if (!header_get_priority(handle, &uint8Value))
    {
      Priority = uint8Value;
    }

    milliseconds millisecondsValue;
    if (!header_get_ttl(handle, &millisecondsValue))
    {
      TimeToLive = std::chrono::milliseconds(millisecondsValue);
    }

    if (!header_get_first_acquirer(handle, &boolValue))
    {
      IsFirstAcquirer = boolValue;
    }

    uint32_t uint32Value;
    if (!header_get_delivery_count(handle, &uint32Value))
    {
      DeliveryCount = uint32Value;
    }
  }

  MessageHeader::operator UniqueMessageHeaderHandle() const
  {
    UniqueMessageHeaderHandle rv{header_create()};
    if (Durable)
    {
      if (header_set_durable(rv.get(), Durable))
      {
        throw std::runtime_error("Could not set durable value.");
      }
    }
    if (Priority != 4)
    {
      if (header_set_priority(rv.get(), Priority))
      {
        throw std::runtime_error("Could not set priority value.");
      }
    }
    if (TimeToLive.HasValue())
    {
      if (header_set_ttl(rv.get(), static_cast<milliseconds>(TimeToLive.Value().count())))
      {
        throw std::runtime_error("Could not set header TTL."); // LCOV_EXCL_LINE
      }
    }

    if (IsFirstAcquirer)
    {
      if (header_set_first_acquirer(rv.get(), IsFirstAcquirer))
      {
        throw std::runtime_error("Could not set first acquirer value.");
      }
    }
    if (DeliveryCount != 0)
    {
      if (header_set_delivery_count(rv.get(), DeliveryCount))
      {
        throw std::runtime_error("Could not set delivery count value.");
      }
    }

    return rv;
  }

  std::ostream& operator<<(std::ostream& os, MessageHeader const& header)
  {
    os << "Header{";
    os << "durable=" << header.Durable;
    os << ", priority=" << header.Priority;
    if (header.TimeToLive.HasValue())
    {
      os << ", ttl=" << header.TimeToLive.Value().count() << " milliseconds";
    }
    os << ", firstAcquirer=" << header.IsFirstAcquirer;
    os << ", deliveryCount=" << header.DeliveryCount;
    os << "}";
    return os;
  }
}}}} // namespace Azure::Core::Amqp::Models
