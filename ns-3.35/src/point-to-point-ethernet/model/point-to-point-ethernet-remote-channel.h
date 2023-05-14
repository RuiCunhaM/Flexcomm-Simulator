/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2007 University of Washington
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Author: George Riley <riley@ece.gatech.edu>
 */

// This object connects two point-to-point-ethernet net devices where at least one
// is not local to this simulator object.  It simply over-rides the transmit
// method and uses an MPI Send operation instead.

#ifndef POINT_TO_POINT_ETHERNET_REMOTE_CHANNEL_H
#define POINT_TO_POINT_ETHERNET_REMOTE_CHANNEL_H

#include "point-to-point-ethernet-channel.h"

namespace ns3 {

/**
 * \ingroup point-to-point-ethernet
 *
 * \brief A Remote Point-To-Point-Ethernet Channel
 * 
 * This object connects two point-to-point-ethernet net devices where at least one
 * is not local to this simulator object. It simply override the transmit
 * method and uses an MPI Send operation instead.
 */
class PointToPointEthernetRemoteChannel : public PointToPointEthernetChannel
{
public:
  /**
   * \brief Get the TypeId
   *
   * \return The TypeId for this class
   */
  static TypeId GetTypeId (void);

  /** 
   * \brief Constructor
   */
  PointToPointEthernetRemoteChannel ();

  /** 
   * \brief Deconstructor
   */
  ~PointToPointEthernetRemoteChannel ();

  /**
   * \brief Transmit the packet
   *
   * \param p Packet to transmit
   * \param src Source PointToPointEthernetNetDevice
   * \param txTime Transmit time to apply
   * \returns true if successful (currently always true)
   */
  virtual bool TransmitStart (Ptr<const Packet> p, Ptr<PointToPointEthernetNetDevice> src,
                              Time txTime);
};

} // namespace ns3

#endif
