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

#ifndef COMPLETE_NETDEVICE_ENERGY_MODEL_H_
#define COMPLETE_NETDEVICE_ENERGY_MODEL_H_

#include <vector>
#include "ns3/object-vector.h"
#include "netdevice-energy-model.h"
#include "ns3/nstime.h"
#include "ns3/traced-value.h"

namespace ns3 {

/**
 * \ingroup energy
 *
 * \brief A complete net device energy model.
 * 
 * Warning: this energy model can be attached only to PointToPoint and Csma NetDevices.
 *
 */
class CompleteNetdeviceEnergyModel : public NetdeviceEnergyModel
{
public:
  /**
   * Get the TypeId of this class.
   */
  static TypeId GetTypeId (void);
  CompleteNetdeviceEnergyModel ();
  virtual ~CompleteNetdeviceEnergyModel ();

  /**
   * \brief Initialize the internal parameters with the values set by the user.
   */
  void Initialize ();

  /**
   * \brief Set pointer to the net device.
   *
   * \param netdevice Pointer to the net device.
   *
   * Implements NetdeviceEnergyModel::SetNetdevice.
   */
  virtual void SetNetdevice (Ptr<NetDevice> netdevice);

  /**
   * \brief Get pointer to the net device.
   *
   * \returns Pointer to the net device.
   *
   * Implements NetdeviceEnergyModel::GetNetdevice.
   */
  virtual Ptr<NetDevice> GetNetdevice (void) const;

  /**
   * Set the power consumption when idle (static consumption).
   *
   * \param idleconso Value in Watts of the idle power consumption of the net device.
   */
  void SetIdleConso (double idleconso);

  /**
   * Set the power consumption when off.
   *
   * \param offconso Value in Watts of the power consumption when the net device is off.
   */
  void SetOffConso (double offconso);

  /**
   * The complete energy model assumes a fixed energy cost per sent Byte.
   * Setter for this cost.
   *
   * \param sentbyteenergy Value for the cost per sent traffic in nJ per Byte.
   */
  void SetSentByteEnergy (double sentbyteenergy);

  /**
   * The complete energy model assumes a fixed energy cost per received Byte.
   * Setter for this cost.
   *
   * \param recvbyteenergy Value for the cost per received traffic in nJ per Byte.
   */
  void SetRecvByteEnergy (double recvbyteenergy);

  /**
   * The complete energy model assumes a fixed energy cost per sent IP packet (independantly of its size).
   * Setter for this cost.
   *
   * \param sentpktenergy Value for the cost of sending a packet in nJ.
   */
  void SetSentPktEnergy (double sentpktenergy);

  /**
   * The complete energy model assumes a fixed energy cost per received IP packet (independantly of its size).
   * Setter for this cost.
   *
   * \param recvpktenergy Value for the cost of receiving a packet in nJ.
   */
  void SetRecvPktEnergy (double recvpktenergy);

  /**
   * To add a new state for energy-aware functions.
   *
   * \param staticConso Static power consumption of the new state in Watts.
   * \param sentByteEnergy Energy consumption of the new state in nJ per sent Byte.
   * \param recvByteEnergy Energy consumption of the new state in nJ per received Byte.
   * \param sentPktEnergy Energy consumption of the new state in nJ per sent packet.
   * \param recvPktEnergy Energy consumption of the new state in nJ per received packet.
   *
   * \returns Id of this new state.
   */
  uint32_t AddState (double staticConso, double sentByteEnergy, double recvByteEnergy,
                     double sentPktEnergy, double recvPktEnergy);

  /**
   * \returns Total energy consumption of the net device.
   *
   * Implements NetdeviceEnergyModel::GetPowerConsumption.
   */
  virtual double GetPowerConsumption (void);

  /**
   * Update the net device state when there is a switch.
   *
   * \param state Id of the new state.
   * \param energy Energy consumed by the state switch.
   * \param duration Duration of the state switch (Time value).
   */
  virtual void UpdateState (uint32_t state, double energy, Time duration);

private:
  /**
  * To get the number of received Bytes.
  */
  static void GetRecvBytes (std::vector<double> args, std::string context,
                            Ptr<const Packet> packet);

  /**
  * To get the number of received IP packets.
  */

  void GetNbRecv (void);

  /**
  * To get the number of sent Bytes.
  */
  static void GetSentBytes (std::vector<double> args, std::string context,
                            Ptr<const Packet> packet);

  /**
  * To get the number of sent IP packets.
  */

  void GetNbSent (void);

private:
  Time m_lastUpdateTime;
  Ptr<NetDevice> m_netdevice;

  // To deal with the different states
  std::vector<double> m_stateIdleConso;
  std::vector<double> m_stateSentByteEnergy;
  std::vector<double> m_stateRecvByteEnergy;
  std::vector<double> m_stateSentPktEnergy;
  std::vector<double> m_stateRecvPktEnergy;

  // To deal with the callbacks
  uint32_t m_rlastPos;
  uint32_t m_slastPos;
  double m_lastNbRecvBytes;
  double m_lastNbSentBytes;
  double m_lastNbRecvPkts;
  double m_lastNbSentPkts;

  // Current state
  double m_idleConso;
  double m_offConso;
  double m_sentByteEnergy;
  double m_recvByteEnergy;
  double m_sentPktEnergy;
  double m_recvPktEnergy;

  // To deal with the state changes
  std::vector<uint32_t> m_stateTab;
  std::vector<Time> m_timeTab;
  std::vector<double> m_switchEnergyTab;
  std::vector<uint32_t> m_sentBytesNbTab;
  std::vector<uint32_t> m_recvBytesNbTab;
  std::vector<uint32_t> m_sentPktsNbTab;
  std::vector<uint32_t> m_recvPktsNbTab;
};

} // namespace ns3

#endif /* COMPLETE_NETDEVICE_ENERGY_MODEL_H_ */
