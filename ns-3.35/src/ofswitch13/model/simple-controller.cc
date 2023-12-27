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

#include <cstdint>
#include "simple-controller.h"
#include "ns3/log.h"
#include "ns3/core-module.h"
#include "ns3/internet-module.h"
#include "ns3/network-module.h"
#include "ns3/ofswitch13-module.h"
#include "ns3/topology-module.h"

NS_LOG_COMPONENT_DEFINE ("SimpleController");

#define PORT_DOWN 1
#define PORT_UP 4

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (SimpleController);

SimpleController::SimpleController ()
{
  NS_LOG_FUNCTION (this);
}

SimpleController::~SimpleController ()
{
  NS_LOG_FUNCTION (this);
}

TypeId
SimpleController::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::SimpleController")
                          .SetParent<OFSwitch13Controller> ()
                          .SetGroupName ("OFSwitch13")
                          .AddConstructor<SimpleController> ();
  return tid;
}

void
SimpleController::DoDispose ()
{
  NS_LOG_FUNCTION (this);
  OFSwitch13Controller::DoDispose ();
}

void
SimpleController::ApplyRouting (uint64_t swDpId)
{
  uint32_t swId = DpId2Id (swDpId);
  Ptr<Node> sw = NodeContainer::GetGlobal ().Get (swId);
  Ptr<OFSwitch13Device> ofDevice = sw->GetObject<OFSwitch13Device> ();
  NodeContainer hosts = NodeContainer::GetGlobalHosts ();

  for (NodeContainer::Iterator i = hosts.Begin (); i != hosts.End (); i++)
    {
      Ipv4Address remoteAddr = (*i)->GetObject<Ipv4> ()->GetAddress (1, 0).GetLocal ();
      std::vector<Ptr<Node>> path = Topology::DijkstraShortestPath (sw, *i);

      uint32_t port = ofDevice->GetPortNoConnectedTo (path.at (1));

      std::ostringstream cmd;
      cmd << "flow-mod cmd=add,table=0 eth_type=0x800,ip_dst=" << remoteAddr
          << " apply:output=" << port;

      NS_LOG_DEBUG ("[" << swDpId << "]: " << cmd.str ());

      DpctlExecute (swDpId, cmd.str ());
    }
}

void
SimpleController::HandshakeSuccessful (Ptr<const RemoteSwitch> sw)
{
  NS_LOG_FUNCTION (this << sw);

  uint64_t swDpId = sw->GetDpId ();

  // Default rules
  DpctlExecute (swDpId, "flow-mod cmd=add,table=0,prio=0 "
                        "apply:output=ctrl:128");
  DpctlExecute (swDpId, "set-config miss=128");

  ApplyRouting (swDpId);
}

ofl_err
SimpleController::HandlePortStatus (struct ofl_msg_port_status *msg, Ptr<const RemoteSwitch> swtch,
                                    uint32_t xid)
{
  NS_LOG_FUNCTION (this << swtch << xid);

  enum ofp_port_reason reason = msg->reason;
  struct ofl_port port = *msg->desc;

  uint64_t swDpId = swtch->GetDpId ();
  uint32_t switchId = DpId2Id (swDpId);
  Ptr<Node> sw = NodeContainer::GetGlobal ().Get (switchId);

  if (reason == OFPPR_MODIFY)
    {
      // FIXME: port.state is a bitmap. This needs to be properly checked
      switch (port.state)
        {
        case PORT_DOWN:
          std::cout << Names::FindName (sw) << " port nº" << port.port_no << " DOWN" << std::endl;
          break;
        case PORT_UP:
          std::cout << Names::FindName (sw) << " port nº" << port.port_no << " UP" << std::endl;
          break;
        default:
          NS_LOG_DEBUG ("Unknow Port State received");
        }
    }
  else
    {
      NS_LOG_DEBUG ("Other Port Status received: " << reason);
    }

  // TODO: There is no free handler for port status messages?
  return 0;
}

} // namespace ns3
