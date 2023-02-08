/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2011-2019 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
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
 * Author: Jaume Nin <jnin@cttc.es>
 *         Nicola Baldo <nbaldo@cttc.es>
 *         Manuel Requena <manuel.requena@cttc.es>
 *         (most of the code refactored to no-backhaul-epc-helper.cc)
 */

#include "ns3/boolean.h"
#include "ns3/string.h"
#include "ns3/log.h"
#include "ns3/point-to-point-helper.h"

#include "ns3/point-to-point-epc-helper.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("PointToPointEpcHelper");

NS_OBJECT_ENSURE_REGISTERED (PointToPointEpcHelper);

PointToPointEpcHelper::PointToPointEpcHelper () : NoBackhaulEpcHelper ()
{
  NS_LOG_FUNCTION (this);
  // To access the attribute value within the constructor
  ObjectBase::ConstructSelf (AttributeConstructionList ());

  // since we use point-to-point links for the backhaul links,
  // we use a /30 subnet which can hold exactly two addresses
  // (remember that net broadcast and null address are not valid)
  m_s1uIpv4AddressHelper.SetBase ("10.0.0.0", "255.255.255.252");
  m_s1apIpv4AddressHelper.SetBase ("11.0.0.0", "255.255.255.252");
}

PointToPointEpcHelper::~PointToPointEpcHelper ()
{
  NS_LOG_FUNCTION (this);
}

TypeId
PointToPointEpcHelper::GetTypeId (void)
{
  NS_LOG_FUNCTION_NOARGS ();
  static TypeId tid =
      TypeId ("ns3::PointToPointEpcHelper")
          .SetParent<NoBackhaulEpcHelper> ()
          .SetGroupName ("Lte")
          .AddConstructor<PointToPointEpcHelper> ()
          .AddAttribute ("S1uLinkDataRate",
                         "The data rate to be used for the next S1-U link to be created",
                         DataRateValue (DataRate ("10Gb/s")),
                         MakeDataRateAccessor (&PointToPointEpcHelper::m_s1uLinkDataRate),
                         MakeDataRateChecker ())
          .AddAttribute (
              "S1uLinkDelay", "The delay to be used for the next S1-U link to be created",
              TimeValue (Seconds (0)), MakeTimeAccessor (&PointToPointEpcHelper::m_s1uLinkDelay),
              MakeTimeChecker ())
          .AddAttribute ("S1uLinkMtu",
                         "The MTU of the next S1-U link to be created. Note that, because of the "
                         "additional GTP/UDP/IP tunneling overhead, you need a MTU larger than the "
                         "end-to-end MTU that you want to support.",
                         UintegerValue (2000),
                         MakeUintegerAccessor (&PointToPointEpcHelper::m_s1uLinkMtu),
                         MakeUintegerChecker<uint16_t> ())
          .AddAttribute ("S1uLinkPcapPrefix", "Prefix for Pcap generated by S1-U link",
                         StringValue ("s1u"),
                         MakeStringAccessor (&PointToPointEpcHelper::m_s1uLinkPcapPrefix),
                         MakeStringChecker ())
          .AddAttribute ("S1uLinkEnablePcap", "Enable Pcap for X2 link", BooleanValue (false),
                         MakeBooleanAccessor (&PointToPointEpcHelper::m_s1uLinkEnablePcap),
                         MakeBooleanChecker ());
  return tid;
}

TypeId
PointToPointEpcHelper::GetInstanceTypeId () const
{
  return GetTypeId ();
}

void
PointToPointEpcHelper::DoDispose ()
{
  NS_LOG_FUNCTION (this);
  NoBackhaulEpcHelper::DoDispose ();
}

void
PointToPointEpcHelper::AddEnb (Ptr<Node> enb, Ptr<NetDevice> lteEnbNetDevice, uint16_t cellId)
{
  NS_LOG_FUNCTION (this << enb << lteEnbNetDevice << cellId);

  NoBackhaulEpcHelper::AddEnb (enb, lteEnbNetDevice, cellId);

  // create a point to point link between the eNB and the SGW with
  // the corresponding new NetDevices on each side
  Ptr<Node> sgw = GetSgwNode ();

  PointToPointHelper p2ph;
  p2ph.SetDeviceAttribute ("DataRate", DataRateValue (m_s1uLinkDataRate));
  p2ph.SetDeviceAttribute ("Mtu", UintegerValue (m_s1uLinkMtu));
  p2ph.SetChannelAttribute ("Delay", TimeValue (m_s1uLinkDelay));
  NetDeviceContainer enbSgwDevices = p2ph.Install (enb, sgw);
  NS_LOG_LOGIC ("Ipv4 ifaces of the eNB after installing p2p dev: "
                << enb->GetObject<Ipv4> ()->GetNInterfaces ());

  if (m_s1uLinkEnablePcap)
    {
      p2ph.EnablePcapAll (m_s1uLinkPcapPrefix);
    }

  m_s1uIpv4AddressHelper.NewNetwork ();
  Ipv4InterfaceContainer enbSgwIpIfaces = m_s1uIpv4AddressHelper.Assign (enbSgwDevices);
  NS_LOG_LOGIC ("number of Ipv4 ifaces of the eNB after assigning Ipv4 addr to S1 dev: "
                << enb->GetObject<Ipv4> ()->GetNInterfaces ());

  Ipv4Address enbS1uAddress = enbSgwIpIfaces.GetAddress (0);
  Ipv4Address sgwS1uAddress = enbSgwIpIfaces.GetAddress (1);

  NoBackhaulEpcHelper::AddS1Interface (enb, enbS1uAddress, sgwS1uAddress, cellId);
}

} // namespace ns3
