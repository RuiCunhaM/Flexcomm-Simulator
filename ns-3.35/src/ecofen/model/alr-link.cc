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
#include "alr-link.h"
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

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (AlrLink);

TypeId
AlrLink::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::AlrLink").SetParent<Object> ().AddConstructor<AlrLink> ()
      /*.AddAttribute ("FullyOnState","The state number of",
                   DoubleValue (),
                   MakeDoubleAccessor (&AlrLink::m_switchOnEnergy),
                   MakeDoubleChecker<double> ())
    .AddAttribute ("SwitchOffEnergy","The energy used to switch off a node.",
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

AlrLink::AlrLink ()
{
}

AlrLink::~AlrLink ()
{
}

void
AlrLink::SetLink (NetDeviceContainer link)
{
  NS_ASSERT (link != NULL);
  //TODO vérifier qu'il n'y a bien que 2 net devices
  m_link = link;
}

NetDeviceContainer
AlrLink::GetLink (void) const
{
  return m_link;
}

void
AlrLink::DeclareRates (std::vector<uint32_t> rates)
{
  uint32_t i;
  sort (rates.begin (), rates.end ());
  for (i = 0; i < rates.size (); i++)
    {
      m_rates.push_back (rates[i]);
    }
  sort (m_rates.begin (), m_rates.end ());
}

void
AlrLink::ChangeState (uint32_t rate)
{
  for (NetDeviceContainer::Iterator i = m_link.Begin (); i != m_link.End (); ++i)
    {
      //TODO TODO NetDeviceSwitchOff (*i, date);
    }
}

void
AlrLink::DecreaseSpeed (void)
{
}

void
AlrLink::IncreaseSpeed (void)
{
}

void
AlrLink::Watch (void)
{
}

} // namespace ns3
