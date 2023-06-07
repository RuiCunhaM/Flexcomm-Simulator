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
#include <boost/graph/detail/adjacency_list.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>
#include <boost/graph/properties.hpp>

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (Topology);

Graph Topology::m_graph = Graph ();

std::map<Ptr<Node>, Vertex> Topology::m_vertexes = std::map<Ptr<Node>, Vertex> ();
std::map<Vertex, Ptr<Node>> Topology::m_nodes = std::map<Vertex, Ptr<Node>> ();
std::map<Ipv4Address, Vertex> Topology::m_ip_to_vertex = std::map<Ipv4Address, Vertex> ();
std::map<Vertex, Ipv4Address> Topology::m_vertex_to_ip = std::map<Vertex, Ipv4Address> ();

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

Vertex
Topology::AddNode (Ptr<Node> node)
{
  Vertex vd = add_vertex (node, m_graph);
  m_vertexes[node] = vd;
  m_nodes[vd] = node;
  return vd;
}

void
Topology::AddSwitch (Ptr<Node> sw)
{
  AddNode (sw);
}

void
Topology::AddHost (Ptr<Node> host, Ipv4Address ip)
{
  Vertex vd = AddNode (host);
  m_ip_to_vertex[ip] = vd;
  m_vertex_to_ip[vd] = ip;
}

void
Topology::AddLink (Ptr<Node> n1, Ptr<Node> n2)
{
  Vertex vd1 = m_vertexes[n1];
  Vertex vd2 = m_vertexes[n2];
  add_edge (vd1, vd2, 1, m_graph);
}

std::vector<Vertex>
Topology::DijkstraShortestPathsInternal (Vertex src)
{
  std::vector<Vertex> predecessors (num_vertices (m_graph));
  dijkstra_shortest_paths (m_graph, src,
                           predecessor_map (boost::make_iterator_property_map (
                               predecessors.begin (), get (boost::vertex_index, m_graph)))

  );
  return predecessors;
}

std::vector<Vertex>
Topology::DijkstraShortestPathInternal (Vertex src, Vertex dst)
{
  std::vector<Vertex> predecessors = DijkstraShortestPathsInternal (src);
  std::vector<Vertex> path;
  Vertex currentVertex = dst;

  while (currentVertex != src)
    {
      path.push_back (currentVertex);
      currentVertex = predecessors[currentVertex];
    }
  path.push_back (src);
  std::reverse (path.begin (), path.end ());

  return path;
}

std::vector<Ptr<Node>>
Topology::VertexToNode (std::vector<Vertex> path)
{
  std::vector<Ptr<Node>> path_nodes = std::vector<Ptr<Node>> ();
  for (auto i = path.begin (); i != path.end (); i++)
    path_nodes.push_back (m_nodes[*i]);
  return path_nodes;
}

std::vector<Ptr<Node>>
Topology::DijkstraShortestPath (Ptr<Node> src, Ptr<Node> dst)
{
  return VertexToNode (DijkstraShortestPathInternal (m_vertexes[src], m_vertexes[dst]));
}

std::vector<Ptr<Node>>
Topology::DijkstraShortestPath (Ptr<Node> src, Ipv4Address dst)
{
  return DijkstraShortestPath (src, m_nodes[m_ip_to_vertex[dst]]);
}

std::vector<Ptr<Node>>
Topology::DijkstraShortestPath (Ipv4Address src, Ptr<Node> dst)
{
  return DijkstraShortestPath (m_nodes[m_ip_to_vertex[src]], dst);
}

std::vector<Ptr<Node>>
Topology::DijkstraShortestPath (Ipv4Address src, Ipv4Address dst)
{
  return DijkstraShortestPath (m_nodes[m_ip_to_vertex[src]], m_nodes[m_ip_to_vertex[dst]]);
}

std::vector<Ptr<Node>>
Topology::DijkstraShortestPath (std::string src, std::string dst)
{
  Ptr<Node> srcNode = Names::Find<Node> (src);
  Ptr<Node> dstNode = Names::Find<Node> (dst);
  return DijkstraShortestPath (srcNode, dstNode);
}

std::vector<Ptr<Node>>
Topology::DijkstraShortestPaths (Ptr<Node> src)
{
  return VertexToNode (DijkstraShortestPathsInternal (m_vertexes[src]));
}

std::vector<Ptr<Node>>
Topology::DijkstraShortestPaths (Ipv4Address src)
{
  return DijkstraShortestPaths (m_nodes[m_ip_to_vertex[src]]);
}

std::vector<Ptr<Node>>
Topology::DijkstraShortestPaths (std::string src)
{
  Ptr<Node> srcNode = Names::Find<Node> (src);
  return DijkstraShortestPaths (srcNode);
}

Graph
Topology::GetGraph ()
{
  return m_graph;
}

Ptr<Node>
Topology::VertexToNode (Vertex vd)
{
  return m_nodes[vd];
}

Vertex
Topology::NodeToVertex (Ptr<Node> node)
{
  return m_vertexes[node];
}

void
Topology::UpdateEdgeWeight (Ptr<Node> n1, Ptr<Node> n2, int newWeight)
{
  Edge ed;
  std::pair<Edge, bool> pair = boost::edge (m_vertexes[n1], m_vertexes[n2], m_graph);

  if (pair.second)
    Topology::UpdateEdgeWeight (pair.first, newWeight);
}

void
Topology::UpdateEdgeWeight (Edge ed, int newWeight)
{
  // Prevent negative weights
  if (newWeight < 0)
    newWeight = 0;
  put (edge_weight_t (), m_graph, ed, newWeight);
}

} // namespace ns3