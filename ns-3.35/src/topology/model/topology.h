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

#ifndef TOPOLOGY_H
#define TOPOLOGY_H

#include "ns3/core-module.h"
#include "ns3/node.h"
#include <boost/graph/adjacency_list.hpp>
#include "ns3/channel.h"

using namespace boost;

namespace ns3 {

typedef adjacency_list<vecS, vecS, undirectedS, Ptr<Node>, property<edge_weight_t, int>> Graph;
typedef graph_traits<Graph>::vertex_descriptor Vertex;
typedef graph_traits<Graph>::edge_descriptor Edge;

class Topology : public Object
{
public:
  static TypeId GetTypeId (void);
  Topology ();
  ~Topology ();

  static void AddSwitch (Ptr<Node> sw);
  static void AddHost (Ptr<Node> host, Ipv4Address ip);
  static void AddLink (Ptr<Node> n1, Ptr<Node> n2, Ptr<Channel> channel);

  static std::vector<Ptr<Node>> DijkstraShortestPath (Ptr<Node> src, Ptr<Node> dst);
  static std::vector<Ptr<Node>> DijkstraShortestPath (Ptr<Node> src, Ipv4Address dst);
  static std::vector<Ptr<Node>> DijkstraShortestPath (Ipv4Address src, Ptr<Node> dst);
  static std::vector<Ptr<Node>> DijkstraShortestPath (Ipv4Address src, Ipv4Address dst);
  static std::vector<Ptr<Node>> DijkstraShortestPath (std::string src, std::string dst);
  static std::vector<Ptr<Node>> DijkstraShortestPaths (Ptr<Node> src);
  static std::vector<Ptr<Node>> DijkstraShortestPaths (Ipv4Address src);
  static std::vector<Ptr<Node>> DijkstraShortestPaths (std::string src);

  static void UpdateEdgeWeight (Ptr<Node> n1, Ptr<Node> n2, int newWeight);

  static Graph GetGraph ();

  static Ptr<Node> VertexToNode (Vertex vd);
  static std::vector<Ptr<Node>> VertexToNode (std::vector<Vertex> path);

  static Vertex NodeToVertex (Ptr<Node> node);

  static Ptr<Channel> GetChannel (Ptr<Node> n1, Ptr<Node> n2);

private:
  static Graph m_graph;

  // NOTE: Perhaps we can simplify this using vertex/edge properties
  static std::map<Ptr<Node>, Vertex> m_vertexes;
  static std::map<Vertex, Ptr<Node>> m_nodes;
  static std::map<Ipv4Address, Vertex> m_ip_to_vertex;
  static std::map<Vertex, Ipv4Address> m_vertex_to_ip;
  static std::map<Edge, Ptr<Channel>> m_channels;

  static std::vector<Vertex> DijkstraShortestPathsInternal (Vertex src);
  static std::vector<Vertex> DijkstraShortestPathInternal (Vertex src, Vertex dst);
  static void UpdateEdgeWeight (Edge ed, int newWeight);
  static Ptr<Channel> GetChannel (Edge e);

  static Vertex AddNode (Ptr<Node> node);
};

} // namespace ns3

#endif /* TOPOLOGY_H */
