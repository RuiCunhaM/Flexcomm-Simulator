/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013 CNRS, France
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
 * Author: Anne-Cécile Orgerie <anne-cecile.orgerie@irisa.fr>
 */

#include "ns3/simulator.h"
#include "ns3/trace-source-accessor.h"
#include "basic-node-energy-model.h"
#include "ns3/log.h"

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (BasicNodeEnergyModel);

TypeId
BasicNodeEnergyModel::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::BasicNodeEnergyModel")
                          .SetParent<NodeEnergyModel> ()
                          .AddConstructor<BasicNodeEnergyModel> ()
                          .AddAttribute ("OnConso", "Power consumption when on", DoubleValue (),
                                         MakeDoubleAccessor (&BasicNodeEnergyModel::m_onConso),
                                         MakeDoubleChecker<double> ())
                          .AddAttribute ("OffConso", "Power consumption when off", DoubleValue (),
                                         MakeDoubleAccessor (&BasicNodeEnergyModel::m_offConso),
                                         MakeDoubleChecker<double> ());
  return tid;
}

BasicNodeEnergyModel::BasicNodeEnergyModel ()
{
  m_lastUpdateTime = Seconds (0.0);
  m_stateTab.push_back (GetNodeState ());
  m_timeTab.push_back (Seconds (0.0));
}

BasicNodeEnergyModel::~BasicNodeEnergyModel ()
{
}

void
BasicNodeEnergyModel::Initialize ()
{
  m_stateConso.push_back (m_offConso); // State 0 = OFF
  m_stateConso.push_back (m_onConso); // State 1 = ON
  m_stateConso.push_back (0.0); // State 2 = SWITCH
}

void
BasicNodeEnergyModel::SetNode (Ptr<Node> node)
{
  NS_ASSERT (node != NULL);
  m_node = node;
}

Ptr<Node>
BasicNodeEnergyModel::GetNode () const
{
  return m_node;
}

void
BasicNodeEnergyModel::SetOnConso (double onconso)
{
  m_stateConso.erase (m_stateConso.begin () + 1);
  m_stateConso.insert (m_stateConso.begin () + 1, onconso);
  m_onConso = onconso;
}

void
BasicNodeEnergyModel::SetOffConso (double offconso)
{
  m_stateConso.erase (m_stateConso.begin ());
  m_stateConso.insert (m_stateConso.begin (), offconso);
  m_offConso = offconso;
}

uint32_t
BasicNodeEnergyModel::AddState (double conso)
{
  m_stateConso.push_back (conso);
  uint32_t statenb = (uint32_t) m_stateConso.size () - 1;
  return (statenb);
}

double
BasicNodeEnergyModel::GetPowerConsumption (void)
{
  //TODO: peut-être implémentation à changer pour une plus grande modularité
  double totconso = 0.0;
  Time now = Simulator::Now ();
  Time firsttime = m_timeTab.front ();
  bool end = false;
  double conso = 0.0;
  while (!end)
    {
      // Set the parameters of the period.
      uint32_t state = m_stateTab.front ();
      Time first = m_timeTab.front ();
      Time last;
      Time duration;
      // Determine the duration of the period.
      if (first < m_lastUpdateTime)
        {
          first = m_lastUpdateTime;
        }
      if (m_stateTab.size () > 1)
        {
          last = m_timeTab.at (1);
          if (last > now)
            {
              last = now;
            }
        }
      else
        {
          last = now;
        }
      duration = last - first;
      // Determine the energy consumption of the period.
      if (duration != Seconds (0.0))
        {
          switch (state)
            {
            case 0:
              conso += m_offConso * duration.GetSeconds ();
              break;
            case 1:
              conso += m_onConso * duration.GetSeconds ();
              break;
              case 2: {
                Time switchduration = m_timeTab.at (1) - m_timeTab.front ();
                conso += (m_switchEnergyTab.front () * duration.GetSeconds () /
                          switchduration.GetSeconds ());
                break;
              }
            default:
              conso += m_stateConso.at (state) * duration.GetSeconds ();
            }
        }
      else if (now == Seconds (0.0))
        {
          switch (state)
            {
            case 0:
              totconso = m_offConso;
              break;
            case 1:
              totconso = m_onConso;
              break;
              case 2: {
                Time switchduration = m_timeTab.at (1) - m_timeTab.front ();
                totconso = (m_switchEnergyTab.front () / switchduration.GetSeconds ());
                break;
              }
            default:
              totconso = m_stateConso.at (state);
            }
        }
      // Go to the next period.
      if (last == now)
        {
          end = true;
        }
      else
        {
          if (state == 2)
            {
              m_switchEnergyTab.erase (m_switchEnergyTab.begin ());
            }
          m_timeTab.erase (m_timeTab.begin ());
          m_stateTab.erase (m_stateTab.begin ());
        }
    }
  // Compute the total power consumption;
  if (firsttime != now)
    {
      Time tottime = now - m_lastUpdateTime;
      totconso = conso / tottime.GetSeconds ();
    }
  m_lastUpdateTime = now;
  return totconso;
}

void
BasicNodeEnergyModel::UpdateState (uint32_t state, double energy, Time duration)
{
  if (duration != Seconds (0.0))
    {
      // Add the switch
      if ((m_timeTab.back () == Simulator::Now () && (Simulator::Now () != Seconds (0.0))))
        {
          m_stateTab.pop_back ();
          m_stateTab.push_back (2);
          m_switchEnergyTab.push_back (energy);
        }
      else
        {
          m_stateTab.push_back (2);
          m_timeTab.push_back (Simulator::Now ());
          m_switchEnergyTab.push_back (energy);
        }
    }
  // Add the next state
  if ((m_timeTab.back () == Simulator::Now () + duration) &&
      (Simulator::Now () + duration != Seconds (0.0)))
    {
      m_stateTab.pop_back ();
      m_stateTab.push_back (state);
    }
  else
    {
      m_stateTab.push_back (state);
      m_timeTab.push_back (Simulator::Now () + duration);
    }
}

} // namespace ns3
