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

#include <stdio.h>
#include <stdint.h>

#include "ns3/simulator.h"
#include "ns3/trace-source-accessor.h"
#include "linear-netdevice-energy-model.h"
#include "ns3/queue.h"
#include "ns3/point-to-point-net-device.h"
#include "ns3/csma-net-device.h"
#include "ns3/log.h"
#include "ns3/config.h"
#include "ns3/ptr.h"

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (LinearNetdeviceEnergyModel);

std::vector<double> sbytes;
std::vector<double> snodes;
std::vector<double> snetdevs;
std::vector<double> rbytes;
std::vector<double> rnodes;
std::vector<double> rnetdevs;

TypeId
LinearNetdeviceEnergyModel::GetTypeId (void)
{
  static TypeId tid =
      TypeId ("ns3::LinearNetdeviceEnergyModel")
          .SetParent<NetdeviceEnergyModel> ()
          .AddConstructor<LinearNetdeviceEnergyModel> ()
          .AddAttribute ("IdleConso", "Power consumption when idle", DoubleValue (),
                         MakeDoubleAccessor (&LinearNetdeviceEnergyModel::m_idleConso),
                         MakeDoubleChecker<double> ())
          .AddAttribute ("OffConso", "Power consumption when off", DoubleValue (),
                         MakeDoubleAccessor (&LinearNetdeviceEnergyModel::m_offConso),
                         MakeDoubleChecker<double> ())
          .AddAttribute ("ByteEnergy", "Energy consumption in nanojoules (nJ) per Byte",
                         DoubleValue (),
                         MakeDoubleAccessor (&LinearNetdeviceEnergyModel::m_byteEnergy),
                         MakeDoubleChecker<double> ())
      /*.AddTraceSource ("TotalEnergyConsumption",
                     "Total energy consumption of the device.",
                     MakeTraceSourceAccessor (&LinearNetdeviceEnergyModel::conso))*/
      ;
  return tid;
}

LinearNetdeviceEnergyModel::LinearNetdeviceEnergyModel ()
{
  m_lastUpdateTime = Seconds (0.0);
  m_slastPos = 0;
  m_rlastPos = 0;
  m_stateTab.push_back (GetNetdeviceState ());
  m_timeTab.push_back (Seconds (0.0));
}

LinearNetdeviceEnergyModel::~LinearNetdeviceEnergyModel ()
{
}

void
LinearNetdeviceEnergyModel::Initialize ()
{
  m_stateIdleConso.push_back (m_offConso); // state 0 = OFF
  m_stateByteEnergy.push_back (0.0);
  m_stateIdleConso.push_back (m_idleConso); // state 1 = ON
  m_stateByteEnergy.push_back (m_byteEnergy);
  m_stateIdleConso.push_back (0.0); // state 2 = SWITCH
  m_stateByteEnergy.push_back (0.0);
  double nodeNb = (double) m_netdevice->GetNode ()->GetId ();
  double netdevNb = (double) m_netdevice->GetIfIndex ();
  std::vector<double> args;
  args.push_back (nodeNb);
  args.push_back (netdevNb);
  std::ostringstream osss;
  std::ostringstream ossr;
  if (m_netdevice->GetInstanceTypeId ().GetName () == "ns3::CsmaNetDevice")
    {
      osss << "/NodeList/" << nodeNb << "/DeviceList/" << netdevNb
           << "/$ns3::CsmaNetDevice/PhyTxEnd";
      ossr << "/NodeList/" << nodeNb << "/DeviceList/" << netdevNb
           << "/$ns3::CsmaNetDevice/PhyRxEnd";
    }
  else if (m_netdevice->GetInstanceTypeId ().GetName () == "ns3::PointToPointNetDevice")
    {
      osss << "/NodeList/" << nodeNb << "/DeviceList/" << netdevNb
           << "/$ns3::PointToPointNetDevice/PhyTxEnd";
      ossr << "/NodeList/" << nodeNb << "/DeviceList/" << netdevNb
           << "/$ns3::PointToPointNetDevice/PhyRxEnd";
    }
  Config::Connect (osss.str (),
                   MakeBoundCallback (&LinearNetdeviceEnergyModel::GetSentBytes, args));
  Config::Connect (ossr.str (),
                   MakeBoundCallback (&LinearNetdeviceEnergyModel::GetRecvBytes, args));
}

void
LinearNetdeviceEnergyModel::SetNetdevice (Ptr<NetDevice> netdevice)
{
  NS_ASSERT (netdevice != NULL);
  m_netdevice = netdevice;
}

Ptr<NetDevice>
LinearNetdeviceEnergyModel::GetNetdevice () const
{
  return m_netdevice;
}

void
LinearNetdeviceEnergyModel::SetIdleConso (double idleconso)
{
  m_idleConso = idleconso;
}

void
LinearNetdeviceEnergyModel::SetOffConso (double offconso)
{
  m_offConso = offconso;
}

void
LinearNetdeviceEnergyModel::SetByteEnergy (double byteenergy)
{
  m_byteEnergy = byteenergy;
}

uint32_t
LinearNetdeviceEnergyModel::AddState (double staticConso, double dynamicEnergy)
{
  m_stateIdleConso.push_back (staticConso);
  m_stateByteEnergy.push_back (dynamicEnergy);
  uint32_t statenb = (uint32_t) m_stateIdleConso.size () - 1;
  return (statenb);
}

void
LinearNetdeviceEnergyModel::GetSentBytes (std::vector<double> args, std::string context,
                                          Ptr<const Packet> packet)
{
  sbytes.push_back ((double) packet->GetSize ());
  snodes.push_back (args[0]);
  snetdevs.push_back (args[1]);
}

void
LinearNetdeviceEnergyModel::GetRecvBytes (std::vector<double> args, std::string context,
                                          Ptr<const Packet> packet)
{
  rbytes.push_back ((double) packet->GetSize ());
  rnodes.push_back (args[0]);
  rnetdevs.push_back (args[1]);
}

double
LinearNetdeviceEnergyModel::GetNbSentBytes (void)
{
  double nbSentBytes = 0;
  double nodeNb = (double) m_netdevice->GetNode ()->GetId ();
  double netdevNb = (double) m_netdevice->GetIfIndex ();
  uint32_t i = 0;
  uint32_t n = sbytes.size ();
  // Find concerned packets
  for (i = m_slastPos; i < n; i++)
    {
      if ((snodes[i] == nodeNb) && (snetdevs[i] == netdevNb))
        {
          nbSentBytes += sbytes[i];
        }
    }
  m_slastPos = i;
  // Clean
  /*uint32_t j;
  for (j = 0; j < positions.size (); j++)
    {
      bytes.erase (bytes.begin () + positions[j] - 1);
      nodes.erase (nodes.begin () + positions[j] - 1);
      netdevs.erase (netdevs.begin () + positions[j] - 1);
    }*/
  return nbSentBytes;
}

double
LinearNetdeviceEnergyModel::GetNbRecvBytes (void)
{
  double nbRecvBytes = 0;
  double nodeNb = (double) m_netdevice->GetNode ()->GetId ();
  double netdevNb = (double) m_netdevice->GetIfIndex ();
  uint32_t i = 0;
  uint32_t n = rbytes.size ();
  // Find concerned packets
  for (i = m_rlastPos; i < n; i++)
    {
      if ((rnodes[i] == nodeNb) && (rnetdevs[i] == netdevNb))
        {
          nbRecvBytes += rbytes[i];
        }
    }
  m_rlastPos = i;
  return nbRecvBytes;
}

/*double  /// Old programmation using the receive Queue of a device
LinearNetdeviceEnergyModel::GetNbRecvBytes (void)
{
  uint32_t nbBytes = 0;
  if (m_netdevice->GetInstanceTypeId ().GetName () == "ns3::CsmaNetDevice")
    {
      Ptr<Queue> queue =  m_netdevice->GetObject<CsmaNetDevice> ()->GetQueue ();
      if (queue != NULL)
      {
        nbBytes = queue->GetTotalReceivedBytes () - m_lastNbBytes;
        m_lastNbBytes = queue->GetTotalReceivedBytes ();
      }
    }
  else if (m_netdevice->GetInstanceTypeId ().GetName () == "ns3::PointToPointNetDevice")
    {
      Ptr<Queue> queueb =  m_netdevice->GetObject<PointToPointNetDevice> ()->GetQueue ();
      if (queueb != NULL)
      {
        nbBytes = queueb->GetTotalReceivedBytes () - m_lastNbBytes;
        m_lastNbBytes = queueb->GetTotalReceivedBytes ();
      }
    }
  //NS_LOG_UNCOND ("NbBytes " << nbBytes);
  return (double) nbBytes;
}*/

double
LinearNetdeviceEnergyModel::GetPowerConsumption (void)
{
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
      if (duration.GetSeconds () < 0)
        {
          NS_LOG_UNCOND ("last " << last.GetSeconds () << " first " << first.GetSeconds ()
                                 << " m_lastUpdateTime " << m_lastUpdateTime << " now " << now);
          for (uint32_t k = 0; k < m_timeTab.size (); k++)
            {
              NS_LOG_UNCOND ("  " << m_timeTab[k].GetSeconds ());
            }
        }
      // Determine the energy consumption of the period.
      if (duration != Seconds (0.0))
        {
          double nbBytes;
          if (last != now)
            {
              nbBytes = (double) m_bytesNbTab.front ();
            }
          else
            {
              nbBytes = GetNbRecvBytes () + GetNbSentBytes ();
            }
          //NS_LOG_UNCOND ("NbSentBytes " << nbSentBytes);
          switch (state)
            {
            case 0:
              //NS_LOG_UNCOND ("Case 0");
              conso += m_offConso * duration.GetSeconds ();
              break;
            case 1:
              //NS_LOG_UNCOND ("Case 1 idle " << m_idleConso << " duration " << duration.GetSeconds () << " nbBytes " << nbBytes << " ByteEnergy "  << m_stateByteEnergy.at (state));
              conso += m_idleConso * duration.GetSeconds () +
                       (nbBytes * m_stateByteEnergy.at (state) * 1E-9);
              break;
              case 2: {
                //NS_LOG_UNCOND ("Case 2");
                Time switchduration = m_timeTab.at (1) - m_timeTab.front ();
                conso += (m_switchEnergyTab.front () * duration.GetSeconds () /
                          switchduration.GetSeconds ());
                break;
              }
            default:
              //NS_LOG_UNCOND ("Case default");
              conso += m_stateIdleConso.at (state) * duration.GetSeconds () +
                       (nbBytes * m_stateByteEnergy.at (state) * 1E-9);
            }
          if (conso < 0)
            {
              NS_LOG_UNCOND ("conso " << conso << " state " << state << " duration "
                                      << duration.GetSeconds ());
            }
        }
      else if (now == Seconds (0.0))
        {
          switch (state)
            {
            case 0:
              //NS_LOG_UNCOND ("Case 0bis");
              totconso = m_offConso;
              break;
            case 1:
              //NS_LOG_UNCOND ("Case 1bis");
              totconso = m_idleConso;
              break;
              case 2: {
                //NS_LOG_UNCOND ("Case 2bis");
                Time switchduration = m_timeTab.at (1) - m_timeTab.front ();
                totconso = (m_switchEnergyTab.front () / switchduration.GetSeconds ());
                break;
              }
            default:
              //NS_LOG_UNCOND ("Case defaultbis");
              totconso = m_stateIdleConso.at (state);
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
LinearNetdeviceEnergyModel::UpdateState (uint32_t state, double energy, Time duration)
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
      m_bytesNbTab.push_back (GetNbRecvBytes () + GetNbSentBytes ());
      //m_lastUpdateTime = Simulator::Now ();
    }
  else
    {
      m_stateTab.push_back (state);
      m_timeTab.push_back (Simulator::Now () + duration);
      m_bytesNbTab.push_back (GetNbRecvBytes () + GetNbSentBytes ());
      //m_lastUpdateTime = Simulator::Now ();
    }
}

} // namespace ns3
