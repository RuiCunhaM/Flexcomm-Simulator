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

#ifndef ALR_LINK_H
#define ALR_LINK_H

#include "ns3/object.h"
#include "ns3/ptr.h"
#include "ns3/type-id.h"
#include "ns3/net-device.h"
#include "ns3/nstime.h"
#include <vector>
#include "ns3/object-vector.h"
#include "ns3/net-device-container.h"

namespace ns3 {

/**
 * \ingroup energy
 * \brief Base class for ALR functions on links.
 */

class AlrLink : public Object
{
public:
  static TypeId GetTypeId (void);
  AlrLink ();
  virtual ~AlrLink ();

  /**
   * \brief Sets pointer to netdevices containing this AlrLink.
   *
   * \param link Pointer to the link containing this AlrLink.
   */
  void SetLink (NetDeviceContainer link);

  /**
   * \returns Pointer to link containing this AlrLink.
   *
   * When a subclass needs to get access to the underlying link base class to
   * print the NetDeviceId for example, it can invoke this method.
   */
  NetDeviceContainer GetLink (void) const;

  /**
   * To declare the different available transmission rates in Mbps.
   * Warning: declare also the maximum rate (= bandwidth) and do not declare 0.
   */
  void DeclareRates (std::vector<uint32_t> rates);

  /**
   * To add the energy values for a rate on a net device.
   * The number of energy values depends on the net device energy model.
   */
  void AddRateConso (uint32_t netdevnb, uint32_t rate, double conso);
  void AddRateConso (uint32_t netdevnb, uint32_t rate, double conso, double conso);
  void AddRateConso (uint32_t netdevnb, uint32_t rate, double conso, double conso, double conso,
                     double conso, double conso);

  /**
   *
   * To change the state of a link and state the new rate.
   *
   * \param rate Value of the new rate of the link.
   */
  void ChangeState (uint32_t rate);

  /**
   *
   * To decrease the rate of a link.
   *
   */
  void DecreaseSpeed (void);

  /**
   * To increase the rate of a link.
   *
   */
  void IncreaseSpeed (void);

  /**
   * To have automatic ALR function.
   *
   */
  void Watch (void);

private:
  NetDeviceContainer m_link;

  Time m_lastChange;
  std::vector<uint32_t> m_stateNbNetdev1;
  std::vector<uint32_t> m_stateNbNetdev2;
  std::vector<uint32_t> m_rates;
  //std::vector<double> m_consosNetdev1;
  //std::vector<double> m_consosNetdev2;
  std::vector<std::vector<double>> m_changeTimes;
  std::vector<std::vector<double>> m_changeEnergysNetdev1;
  std::vector<std::vector<double>> m_changeEnergysNetdev2;
};

} // namespace ns3

#endif /* ALR_LINK_H */
