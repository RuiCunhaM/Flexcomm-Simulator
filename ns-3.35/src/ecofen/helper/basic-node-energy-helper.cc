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
 * Author: Anne-Cecile Orgerie <anne-cecile.orgerie@irisa.fr>
 */

#include "basic-node-energy-helper.h"
#include "ns3/node-energy-model.h"
#include "ns3/basic-node-energy-model.h"

namespace ns3 {

BasicNodeEnergyHelper::BasicNodeEnergyHelper ()
{
  m_basicNodeEnergyModel.SetTypeId ("ns3::BasicNodeEnergyModel");
}

BasicNodeEnergyHelper::~BasicNodeEnergyHelper ()
{
}

void
BasicNodeEnergyHelper::Set (std::string name, const AttributeValue &v)
{
  m_basicNodeEnergyModel.Set (name, v);
}

Ptr<NodeEnergyModel>
BasicNodeEnergyHelper::DoInstall (Ptr<Node> node) const
{
  NS_ASSERT (node != NULL);
  // check if energy model already exists
  Ptr<NodeEnergyModel> nenergy = node->GetObject<NodeEnergyModel> ();
  if (nenergy != NULL)
    {
      NS_FATAL_ERROR ("Node energy model already installed!");
    }
  Ptr<BasicNodeEnergyModel> energy = m_basicNodeEnergyModel.Create<BasicNodeEnergyModel> ();
  NS_ASSERT (energy != NULL);
  energy->SetNode (node);
  node->AggregateObject (energy);
  energy->Initialize ();
  return energy;
}

} // namespace ns3
