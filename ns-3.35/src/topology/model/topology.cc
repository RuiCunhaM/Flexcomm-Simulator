/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * The GPLv2 License (GPLv2)
 *
 * Copyright (c) 2023 Rui Pedro C. Monteiro
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http: //www.gnu.org/licenses/>.
 *
 * Author: Rui Pedro C. Monteiro <rui.p.monteiro@inesctec.pt>
 */

#include "topology.h"
#include "ns3/mac48-address.h"
#include "ns3/node-list.h"

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (Topology);

/* ########### IntegerWeightCalc ########### */

IntegerWeightCalc::IntegerWeightCalc () : m_weights ()
{
}

IntegerWeightCalc::~IntegerWeightCalc ()
{
}

int
IntegerWeightCalc::GetInitialWeight () const
{
  return 0;
};

int
IntegerWeightCalc::GetNonViableWeight () const
{
  return std::numeric_limits<int>::max ();
};

int
IntegerWeightCalc::GetWeight (Edge &e) const
{
  auto iter = m_weights.find (e);
  if (iter == m_weights.end ())
    return 1;

  return (*iter).second;
};

void
IntegerWeightCalc::UpdateEdgeWeight (Edge &e, int weight)
{
  m_weights[e] = weight;
}

/* ########### Topology ########### */

Graph Topology::m_graph{};
IntegerWeightCalc Topology::m_default_calc{};

std::map<Ipv4Address, Ptr<Node>> Topology::m_ip_to_node{};
std::map<Mac48Address, Ptr<Node>> Topology::m_mac_to_node{};
std::map<Edge, Ptr<Channel>> Topology::m_channels{};

TypeId
Topology::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::Topology").SetParent<Object> ().AddConstructor<Topology> ();
  return tid;
}

Topology::Topology ()
{
}

Topology::~Topology ()
{
}

void
Topology::AddNodeIpAddress (Ptr<Node> node, Ipv4Address ip)
{
  m_ip_to_node[ip] = node;
}

void
Topology::AddEdge (Ptr<Node> n1, Ptr<Node> n2, Ptr<Channel> channel)
{
  Edge e (n1->GetId (), n2->GetId ());
  add_edge (e.first, e.second, e, m_graph);
  m_channels[e] = channel;
  m_mac_to_node[Mac48Address::ConvertFrom (channel->GetDevice (0)->GetAddress ())] = n1;
  m_mac_to_node[Mac48Address::ConvertFrom (channel->GetDevice (1)->GetAddress ())] = n2;
}

std::vector<Ptr<Node>>
Topology::VertexToNode (std::vector<Vertex> path)
{
  std::vector<Ptr<Node>> path_nodes = std::vector<Ptr<Node>> ();
  for (auto i = path.begin (); i != path.end (); i++)
    path_nodes.push_back (NodeList::GetNode (*i));
  return path_nodes;
}

Graph
Topology::GetGraph ()
{
  return m_graph;
}

IntegerWeightCalc &
Topology::GetDefaultWeightCalc ()
{
  return m_default_calc;
}

Ptr<Node>
Topology::VertexToNode (Vertex vd)
{
  return NodeList::GetNode (vd);
}

Ptr<Channel>
Topology::GetChannel (Ptr<Node> n1, Ptr<Node> n2)
{
  Edge e (n1->GetId (), n2->GetId ());

  return Topology::GetChannel (e);
}

Ptr<Channel>
Topology::GetChannel (Edge e)
{

  auto iter = m_channels.find (e);
  if (iter == m_channels.end ())
    return NULL;

  return (*iter).second;
}

} // namespace ns3
