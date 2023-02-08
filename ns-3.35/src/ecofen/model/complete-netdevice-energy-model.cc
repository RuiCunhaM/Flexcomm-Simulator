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
#include "complete-netdevice-energy-model.h"
#include "ns3/queue.h"
#include "ns3/point-to-point-net-device.h"
#include "ns3/csma-net-device.h"
#include "ns3/log.h"
#include "ns3/config.h"
#include "ns3/ptr.h"

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (CompleteNetdeviceEnergyModel);

std::vector<double> scbytes;
std::vector<double> scnodes;
std::vector<double> scnetdevs;
std::vector<double> rcbytes;
std::vector<double> rcnodes;
std::vector<double> rcnetdevs;

TypeId
CompleteNetdeviceEnergyModel::GetTypeId (void)
{
  static TypeId tid =
      TypeId ("ns3::CompleteNetdeviceEnergyModel")
          .SetParent<NetdeviceEnergyModel> ()
          .AddConstructor<CompleteNetdeviceEnergyModel> ()
          .AddAttribute ("IdleConso", "Power consumption when idle", DoubleValue (),
                         MakeDoubleAccessor (&CompleteNetdeviceEnergyModel::m_idleConso),
                         MakeDoubleChecker<double> ())
          .AddAttribute ("OffConso", "Power consumption when off", DoubleValue (),
                         MakeDoubleAccessor (&CompleteNetdeviceEnergyModel::m_offConso),
                         MakeDoubleChecker<double> ())
          .AddAttribute ("RecvByteEnergy",
                         "Energy consumption in nanojoules (nJ) per received Byte", DoubleValue (),
                         MakeDoubleAccessor (&CompleteNetdeviceEnergyModel::m_recvByteEnergy),
                         MakeDoubleChecker<double> ())
          .AddAttribute ("SentByteEnergy", "Energy consumption in nanojoules (nJ) per sent Byte",
                         DoubleValue (),
                         MakeDoubleAccessor (&CompleteNetdeviceEnergyModel::m_sentByteEnergy),
                         MakeDoubleChecker<double> ())
          .AddAttribute (
              "RecvPktEnergy", "Energy consumption in nanojoules (nJ) per received packet",
              DoubleValue (), MakeDoubleAccessor (&CompleteNetdeviceEnergyModel::m_recvPktEnergy),
              MakeDoubleChecker<double> ())
          .AddAttribute ("SentPktEnergy", "Energy consumption in nanojoules (nJ) per sent packet",
                         DoubleValue (),
                         MakeDoubleAccessor (&CompleteNetdeviceEnergyModel::m_sentPktEnergy),
                         MakeDoubleChecker<double> ())
      /*.AddTraceSource ("TotalEnergyConsumption",
                     "Total energy consumption of the device.",
                     MakeTraceSourceAccessor (&CompleteNetdeviceEnergyModel::conso))*/
      ;
  return tid;
}

CompleteNetdeviceEnergyModel::CompleteNetdeviceEnergyModel ()
{
  m_lastUpdateTime = Seconds (0.0);
  m_slastPos = 0;
  m_rlastPos = 0;
  m_stateTab.push_back (GetNetdeviceState ());
  m_timeTab.push_back (Seconds (0.0));
}

CompleteNetdeviceEnergyModel::~CompleteNetdeviceEnergyModel ()
{
}

void
CompleteNetdeviceEnergyModel::Initialize ()
{
  // Initialization of the energy values per state
  m_stateIdleConso.push_back (m_offConso); // state 0 = OFF
  m_stateRecvByteEnergy.push_back (0.0);
  m_stateSentByteEnergy.push_back (0.0);
  m_stateRecvPktEnergy.push_back (0.0);
  m_stateSentPktEnergy.push_back (0.0);
  m_stateIdleConso.push_back (m_idleConso); // state 1 = ON
  m_stateRecvByteEnergy.push_back (m_recvByteEnergy);
  m_stateSentByteEnergy.push_back (m_sentByteEnergy);
  m_stateRecvPktEnergy.push_back (m_recvPktEnergy);
  m_stateSentPktEnergy.push_back (m_sentPktEnergy);
  m_stateIdleConso.push_back (0.0); // state 2 = SWITCH
  m_stateRecvByteEnergy.push_back (0.0);
  m_stateSentByteEnergy.push_back (0.0);
  m_stateRecvPktEnergy.push_back (0.0);
  m_stateSentPktEnergy.push_back (0.0);
  // Callbacks
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
                   MakeBoundCallback (&CompleteNetdeviceEnergyModel::GetSentBytes, args));
  Config::Connect (ossr.str (),
                   MakeBoundCallback (&CompleteNetdeviceEnergyModel::GetRecvBytes, args));
}

void
CompleteNetdeviceEnergyModel::SetNetdevice (Ptr<NetDevice> netdevice)
{
  NS_ASSERT (netdevice != NULL);
  m_netdevice = netdevice;
}

Ptr<NetDevice>
CompleteNetdeviceEnergyModel::GetNetdevice () const
{
  return m_netdevice;
}

void
CompleteNetdeviceEnergyModel::SetIdleConso (double idleconso)
{
  m_idleConso = idleconso;
}

void
CompleteNetdeviceEnergyModel::SetOffConso (double offconso)
{
  m_offConso = offconso;
}

void
CompleteNetdeviceEnergyModel::SetSentByteEnergy (double sentbyteenergy)
{
  m_sentByteEnergy = sentbyteenergy;
}

void
CompleteNetdeviceEnergyModel::SetRecvByteEnergy (double recvbyteenergy)
{
  m_recvByteEnergy = recvbyteenergy;
}

void
CompleteNetdeviceEnergyModel::SetSentPktEnergy (double sentpktenergy)
{
  m_sentPktEnergy = sentpktenergy;
}

void
CompleteNetdeviceEnergyModel::SetRecvPktEnergy (double recvpktenergy)
{
  m_recvPktEnergy = recvpktenergy;
}

uint32_t
CompleteNetdeviceEnergyModel::AddState (double staticConso, double sentByteEnergy,
                                        double recvByteEnergy, double sentPktEnergy,
                                        double recvPktEnergy)
{
  m_stateIdleConso.push_back (staticConso);
  m_stateSentByteEnergy.push_back (sentByteEnergy);
  m_stateRecvByteEnergy.push_back (recvByteEnergy);
  m_stateSentPktEnergy.push_back (sentPktEnergy);
  m_stateRecvPktEnergy.push_back (recvPktEnergy);
  uint32_t statenb = (uint32_t) m_stateIdleConso.size () - 1;
  return (statenb);
}

void
CompleteNetdeviceEnergyModel::GetSentBytes (std::vector<double> args, std::string context,
                                            Ptr<const Packet> packet)
{
  scbytes.push_back ((double) packet->GetSize ());
  scnodes.push_back (args[0]);
  scnetdevs.push_back (args[1]);
}

void
CompleteNetdeviceEnergyModel::GetRecvBytes (std::vector<double> args, std::string context,
                                            Ptr<const Packet> packet)
{
  rcbytes.push_back ((double) packet->GetSize ());
  rcnodes.push_back (args[0]);
  rcnetdevs.push_back (args[1]);
}

void
CompleteNetdeviceEnergyModel::GetNbSent (void)
{
  double nbSentBytes = 0;
  double nbSentPkts = 0;
  double nodeNb = (double) m_netdevice->GetNode ()->GetId ();
  double netdevNb = (double) m_netdevice->GetIfIndex ();
  uint32_t i = 0;
  uint32_t n = scbytes.size ();
  // Find concerned packets
  for (i = m_slastPos; i < n; i++)
    {
      if ((scnodes[i] == nodeNb) && (scnetdevs[i] == netdevNb))
        {
          nbSentBytes += scbytes[i];
          nbSentPkts += 1;
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
  m_lastNbSentBytes = nbSentBytes;
  m_lastNbSentPkts = nbSentPkts;
}

void
CompleteNetdeviceEnergyModel::GetNbRecv (void)
{
  double nbRecvBytes = 0;
  double nbRecvPkts = 0;
  double nodeNb = (double) m_netdevice->GetNode ()->GetId ();
  double netdevNb = (double) m_netdevice->GetIfIndex ();
  uint32_t i = 0;
  uint32_t n = rcbytes.size ();
  // Find concerned packets
  for (i = m_rlastPos; i < n; i++)
    {
      if ((rcnodes[i] == nodeNb) && (rcnetdevs[i] == netdevNb))
        {
          nbRecvBytes += rcbytes[i];
          nbRecvPkts += 1;
        }
    }
  m_rlastPos = i;
  m_lastNbRecvBytes = nbRecvBytes;
  m_lastNbRecvPkts = nbRecvPkts;
}

double
CompleteNetdeviceEnergyModel::GetPowerConsumption (void)
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
      // Determine the energy consumption of the period.
      if (duration != Seconds (0.0))
        {
          double nbsentBytes;
          double nbrecvBytes;
          double nbsentPkts;
          double nbrecvPkts;
          if (last != now)
            {
              nbsentBytes = (double) m_sentBytesNbTab.front ();
              nbrecvBytes = (double) m_recvBytesNbTab.front ();
              nbsentPkts = (double) m_sentPktsNbTab.front ();
              nbrecvPkts = (double) m_recvPktsNbTab.front ();
            }
          else
            {
              GetNbRecv ();
              GetNbSent ();
              nbrecvBytes = m_lastNbRecvBytes;
              nbsentBytes = m_lastNbSentBytes;
              nbrecvPkts = m_lastNbRecvPkts;
              nbsentPkts = m_lastNbSentPkts;
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
                       (nbrecvBytes * m_stateRecvByteEnergy.at (state) * 1E-9) +
                       (nbsentBytes * m_stateSentByteEnergy.at (state) * 1E-9) +
                       (nbrecvPkts * m_stateRecvPktEnergy.at (state) * 1E-9) +
                       (nbsentPkts * m_stateSentPktEnergy.at (state) * 1E-9);
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
                       (nbrecvBytes * m_stateRecvByteEnergy.at (state) * 1E-9) +
                       (nbsentBytes * m_stateSentByteEnergy.at (state) * 1E-9) +
                       (nbrecvPkts * m_stateRecvPktEnergy.at (state) * 1E-9) +
                       (nbsentPkts * m_stateSentPktEnergy.at (state) * 1E-9);
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
CompleteNetdeviceEnergyModel::UpdateState (uint32_t state, double energy, Time duration)
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
      GetNbRecv ();
      GetNbSent ();
      double nbrecvBytes = m_lastNbRecvBytes;
      double nbsentBytes = m_lastNbSentBytes;
      double nbrecvPkts = m_lastNbRecvPkts;
      double nbsentPkts = m_lastNbSentPkts;
      m_recvBytesNbTab.push_back (nbrecvBytes);
      m_sentBytesNbTab.push_back (nbsentBytes);
      m_recvPktsNbTab.push_back (nbrecvPkts);
      m_sentPktsNbTab.push_back (nbsentPkts);
      //m_lastUpdateTime = Simulator::Now ();
    }
  else
    {
      m_stateTab.push_back (state);
      m_timeTab.push_back (Simulator::Now () + duration);
      GetNbRecv ();
      GetNbSent ();
      double nbrecvBytes = m_lastNbRecvBytes;
      double nbsentBytes = m_lastNbSentBytes;
      double nbrecvPkts = m_lastNbRecvPkts;
      double nbsentPkts = m_lastNbSentPkts;
      m_recvBytesNbTab.push_back (nbrecvBytes);
      m_sentBytesNbTab.push_back (nbsentBytes);
      m_recvPktsNbTab.push_back (nbrecvPkts);
      m_sentPktsNbTab.push_back (nbsentPkts);
      //m_lastUpdateTime = Simulator::Now ();
    }
}

} // namespace ns3
