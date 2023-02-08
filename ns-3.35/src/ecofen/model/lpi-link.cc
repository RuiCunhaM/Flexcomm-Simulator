/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013 CNRS, France.
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
 * Author: Anne-Cecile Orgerie <anne-cecile.orgerie@irisa.fr>
 */

#include "node-energy-model.h"
#include "lpi-link.h"
#include "ns3/uinteger.h"
#include "ns3/double.h"
#include "ns3/log.h"
#include "ns3/simulator.h"
#include "ns3/ipv4.h"
#include "ns3/ipv6.h"
#include "ns3/node.h"
#include "ns3/netdevice-energy-model.h"
#include "ns3/basic-netdevice-energy-model.h"
#include "ns3/linear-netdevice-energy-model.h"
#include "ns3/complete-netdevice-energy-model.h"
#include "ns3/queue.h"
#include "ns3/point-to-point-net-device.h"
#include "ns3/csma-net-device.h"

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (LpiLink);

TypeId
LpiLink::GetTypeId (void)
{
  static TypeId tid =
      TypeId ("ns3::LpiLink")
          .SetParent<Object> ()
          .AddConstructor<LpiLink> ()
          .AddAttribute ("State", "LPI State (Sleep 0, Awake 1)", UintegerValue (1),
                         MakeUintegerAccessor (&LpiLink::m_state), MakeUintegerChecker<uint32_t> ())
      /*.AddAttribute ("SwitchOffEnergy","The energy used to switch off a node.",
                   DoubleValue (),
                   MakeDoubleAccessor (&AlrLink::m_switchOffEnergy),
                   MakeDoubleChecker<double> ())
    .AddAttribute ("SwitchOffDuration","The duration of a switch off a node in seconds.",
                   DoubleValue (),
                   MakeDoubleAccessor (&AlrLink::m_switchOffDuration),
                   MakeDoubleChecker<double> ())
    .AddAttribute ("SwitchOnDuration","The duration of a switch on a node in seconds.",
                   DoubleValue (),
                   MakeDoubleAccessor (&AlrLink::m_switchOnDuration),
                   MakeDoubleChecker<double> ())*/
      ;
  return tid;
}

void (LpiLink::*fp) (uint32_t n) = &LpiLink::Refresh;

LpiLink::LpiLink ()
{
}

LpiLink::~LpiLink ()
{
}

void
LpiLink::SetLink (NetDeviceContainer link)
{
  //NS_ASSERT (link != NULL);
  uint32_t n = link.GetN ();
  if (n != 2)
    {
      NS_FATAL_ERROR ("You did not apply LPI to a link!");
    }
  else
    {
      m_link = link;
    }
}

NetDeviceContainer
LpiLink::GetLink (void) const
{
  return m_link;
}

void
LpiLink::PutToSleep (void)
{
  Time now = Simulator::Now ();
  m_state = 0;
  NetDeviceContainer netdevc = GetLink ();
  uint32_t i;
  uint32_t n = 2;
  // Make the switch to sleep state
  for (i = 0; i < n; i++)
    {
      Ptr<NetdeviceEnergyModel> ndem;
      Ptr<NetDevice> netdevi = netdevc.Get (i);
      ndem = netdevi->GetObject<NetdeviceEnergyModel> ();
      ndem->UpdateState (m_lpiStateNb[i], m_energyToSleep[i], Seconds (m_timeToSleep));
    }
  //Plan next refresh
  Ptr<LpiLink> lpilink = m_link.Get (0)->GetObject<LpiLink> ();
  Simulator::Schedule (Seconds (m_timeQuiet), fp, lpilink, 0);
  //TODO empêcher le lien de communiquer
}

void
LpiLink::WakeUp (void)
{
  m_state = 1;
  NetDeviceContainer netdevc = GetLink ();
  uint32_t i;
  uint32_t n = 2;
  // Make the switch to on state
  for (i = 0; i < n; i++)
    {
      Ptr<NetdeviceEnergyModel> ndem;
      Ptr<NetDevice> netdevi = netdevc.Get (i);
      ndem = netdevi->GetObject<NetdeviceEnergyModel> ();
      ndem->UpdateState (
          ndem->GetNetdeviceOnState (), m_energyToWakeUp[i],
          Seconds (m_timeToWakeUp)); //TODO vérifier comment se passe le recouvrement de switch
    }
  //TODO autoriser le lien à communiquer à partir de now + Seconds (m_timeToWakeUp)
}

void
LpiLink::Refresh (uint32_t n)
{
  // Refresh only if it's still needed = still sleeping
  n++;
  if (m_state == 0)
    {
      NetDeviceContainer netdevc = GetLink ();
      Ptr<Queue> queue1, queue2;
      Ptr<NetDevice> netdev1 = netdevc.Get (0);
      Ptr<NetDevice> netdev2 = netdevc.Get (1);
      // Warning: works only for Csma and PointToPoint net devices.
      if (netdev1->GetInstanceTypeId ().GetName () == "ns3::PointToPointNetDevice")
        {
          queue1 = netdev1->GetObject<PointToPointNetDevice> ()->GetQueue ();
        }
      else if (netdev1->GetInstanceTypeId ().GetName () == "ns3::CsmaNetDevice")
        {
          queue1 = netdev1->GetObject<CsmaNetDevice> ()->GetQueue ();
        }
      if (netdev2->GetInstanceTypeId ().GetName () == "ns3::PointToPointNetDevice")
        {
          queue2 = netdev2->GetObject<PointToPointNetDevice> ()->GetQueue ();
        }
      else if (netdev2->GetInstanceTypeId ().GetName () == "ns3::CsmaNetDevice")
        {
          queue2 = netdev2->GetObject<CsmaNetDevice> ()->GetQueue ();
        }
      // If queues are not empty -> Wake up!
      if ((!queue1->IsEmpty ()) || (!queue2->IsEmpty ()))
        {
          WakeUp ();
        }
      // If queues are empty -> stay asleep.
      else
        {
          uint32_t i;
          uint32_t n = 2;
          for (i = 0; i < n; i++)
            {
              Ptr<NetdeviceEnergyModel> ndem;
              Ptr<NetDevice> netdevi = netdevc.Get (i);
              ndem = netdevi->GetObject<NetdeviceEnergyModel> ();
              ndem->UpdateState (m_lpiStateNb[i], m_energyToRefresh[i], Seconds (m_timeToRefresh));
            }
          // Plan the next Refresh
          Ptr<LpiLink> lpilink = m_link.Get (0)->GetObject<LpiLink> ();
          Simulator::Schedule (Seconds (m_timeQuiet), fp, lpilink, n);
        }
    }
}

void
LpiLink::Watch (void)
{
  NetDeviceContainer netdevc = GetLink ();
  Ptr<Queue> queue1, queue2;
  Ptr<NetDevice> netdev1 = netdevc.Get (0);
  Ptr<NetDevice> netdev2 = netdevc.Get (1);
  // Warning: works only for Csma and PointToPoint net devices.
  if (netdev1->GetInstanceTypeId ().GetName () == "ns3::PointToPointNetDevice")
    {
      queue1 = netdev1->GetObject<PointToPointNetDevice> ()->GetQueue ();
    }
  else if (netdev1->GetInstanceTypeId ().GetName () == "ns3::CsmaNetDevice")
    {
      queue1 = netdev1->GetObject<CsmaNetDevice> ()->GetQueue ();
    }
  if (netdev2->GetInstanceTypeId ().GetName () == "ns3::PointToPointNetDevice")
    {
      queue2 = netdev2->GetObject<PointToPointNetDevice> ()->GetQueue ();
    }
  else if (netdev2->GetInstanceTypeId ().GetName () == "ns3::CsmaNetDevice")
    {
      queue2 = netdev2->GetObject<CsmaNetDevice> ()->GetQueue ();
    }
  // If awake
  if (m_state == 1)
    {
      // If empty queue -> put to sleep
      if ((queue1->IsEmpty ()) && (queue2->IsEmpty ()))
        {
          PutToSleep ();
        }
      // Else -> nothing = link communicating
    }
  // If sleeping
  else
    {
      // If queue not empty -> wake up
      if ((!queue1->IsEmpty ()) || (!queue2->IsEmpty ()))
        {
          WakeUp ();
        }
      // Else -> nothing to do
    }
}

} // namespace ns3
