/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * The GPLv2 License (GPLv2)
 *
 * Copyright (c) 2023 Rui Pedro C. Monteiro
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http: //www.gnu.org/licenses/>.
 *
 * Author: Rui Pedro C. Monteiro <rui.p.monteiro@inesctec.pt>
 */

#ifndef SIMPLE_CONTROLLER_H
#define SIMPLE_CONTROLLER_H

#include "ofswitch13-controller.h"

namespace ns3 {

class SimpleController : public OFSwitch13Controller
{
public:
  SimpleController ();
  virtual ~SimpleController ();
  static TypeId GetTypeId (void);
  virtual void DoDispose ();

  ofl_err HandlePortStatus (struct ofl_msg_port_status *msg, Ptr<const RemoteSwitch> swtch,
                            uint32_t xid);

protected:
  void HandshakeSuccessful (Ptr<const RemoteSwitch> sw);
  void ApplyRouting (uint64_t src);
};

} // namespace ns3

#endif
