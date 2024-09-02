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

#include "netdevice-energy-model.h"
#include "ns3/net-device.h"
#include "ns3/nstime.h"
#include "ns3/uinteger.h"
#include "ns3/log.h"
#include "ns3/simulator.h"
#include "ns3/node.h"

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (NetdeviceEnergyModel);

TypeId
NetdeviceEnergyModel::GetTypeId (void)
{
  static TypeId tid =
      TypeId ("ns3::NetdeviceEnergyModel")
          .SetParent<Object> ()
          .AddConstructor<NetdeviceEnergyModel> ()
          .AddAttribute ("NetdeviceState",
                         "The default state of the net device (0=off, 1=on, 2=switching)",
                         UintegerValue (1),
                         MakeUintegerAccessor (&NetdeviceEnergyModel::SetNetdeviceState,
                                               &NetdeviceEnergyModel::GetNetdeviceState),
                         MakeUintegerChecker<uint32_t> ())
          .AddAttribute ("NetdeviceOnState", "The default on state of the net device.",
                         UintegerValue (1),
                         MakeUintegerAccessor (&NetdeviceEnergyModel::SetNetdeviceOnState,
                                               &NetdeviceEnergyModel::GetNetdeviceOnState),
                         MakeUintegerChecker<uint32_t> ())
          .AddAttribute ("NetdeviceOffState", "The default off state of the net device.",
                         UintegerValue (0),
                         MakeUintegerAccessor (&NetdeviceEnergyModel::m_netdeviceOffState),
                         MakeUintegerChecker<uint32_t> ());
  return tid;
}

//TODO ajouter une fonction pour initialiser le device à un autre état

NetdeviceEnergyModel::NetdeviceEnergyModel ()
    : m_netdeviceState (1), m_netdeviceOnState (1), m_netdeviceOffState (0), m_powerDrawn (0)
{
}

NetdeviceEnergyModel::~NetdeviceEnergyModel ()
{
}

uint32_t
NetdeviceEnergyModel::GetNetdeviceState (void) const
{
  return m_netdeviceState;
}

void
NetdeviceEnergyModel::SetNetdeviceState (uint32_t netdeviceState)
{
  m_netdeviceState = netdeviceState;
}

uint32_t
NetdeviceEnergyModel::GetNetdeviceOnState (void) const
{
  return m_netdeviceOnState;
}

void
NetdeviceEnergyModel::SetNetdeviceOnState (uint32_t state)
{
  m_netdeviceOnState = state;
}

uint32_t
NetdeviceEnergyModel::GetNetdeviceOffState (void) const
{
  return m_netdeviceOffState;
}

void
NetdeviceEnergyModel::SetNetdeviceOffState (uint32_t state)
{
  m_netdeviceOffState = state;
}

void
NetdeviceEnergyModel::SetNetdevice (Ptr<NetDevice> netdevice)
{
  NS_ASSERT (netdevice != NULL);
  m_netdevice = netdevice;
}

Ptr<NetDevice>
NetdeviceEnergyModel::GetNetdevice (void) const
{
  return m_netdevice;
}

double
NetdeviceEnergyModel::GetPowerConsumption (void)
{
  return 0.0;
}

double
NetdeviceEnergyModel::GetPowerDrawn (void)
{
  return m_powerDrawn / 3600;
}

double
NetdeviceEnergyModel::GetCurrentPowerConsumption (void)
{
  return m_lastConso;
}

void
NetdeviceEnergyModel::GetConso (Time interval, Time stop)
{
  Time i = Seconds (0.0);
  while (i <= stop)
    {
      Simulator::Schedule (i, &NetdeviceEnergyModel::UpdateEnergy, this);
      i += interval;
    }
}

void
NetdeviceEnergyModel::UpdateState (uint32_t state, double energy, Time duration)
{
}

void
NetdeviceEnergyModel::UpdateEnergy (void)
{
  Time t_now = Simulator::Now ();
  m_lastConso = GetPowerConsumption ();
  m_powerDrawn += m_lastConso * (t_now - m_lastUpdate).GetSeconds ();
  m_lastUpdate = t_now;
}

} // namespace ns3
