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

#ifndef LPI_LINK_H
#define LPI_LINK_H

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
 * \brief Base class for LPI functions on links.
 *
 * Warning: this class only works with Csma and PointToPoint net devices.
 */

class LpiLink : public Object
{
public:
  static TypeId GetTypeId (void);
  LpiLink ();
  virtual ~LpiLink ();

  /**
   * \brief Sets pointer to netdevices containing this LpiLink.
   *
   * \param link Pointer to the link containing this LpiLink.
   */
  void SetLink (NetDeviceContainer link);

  /**
   * \returns Pointer to link containing this LpiLink.
   *
   * When a subclass needs to get access to the underlying link base class to
   * print the NetDeviceId for example, it can invoke this method.
   */
  NetDeviceContainer GetLink (void) const;

  /**
   *
   * To put the two net devices to sleep.
   *
   */
  void PutToSleep (void);

  /**
   * To wake up the two net devices.
   *
   */
  void WakeUp (void);

  /**
   * To refresh the link.
   *
   */
  void Refresh (uint32_t n);

  /**
   * To have an automatic LPI function.
   *
   */
  void Watch (void);

private:
  NetDeviceContainer m_link;

  uint32_t m_state;
  Time m_lastChange;
  double m_lpiConso;
  double m_timeToSleep; // T_s
  double m_timeToWakeUp; // T_w
  double m_timeToRefresh; // T_r
  double m_timeQuiet; // T_q
  std::vector<double> m_energyToSleep;
  std::vector<double> m_energyToWakeUp;
  std::vector<double> m_energyToRefresh;
  std::vector<double> m_lpiStateNb;
};

} // namespace ns3

#endif /* LPI_LINK_H */
