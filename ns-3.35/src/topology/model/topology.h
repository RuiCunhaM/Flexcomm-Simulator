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
#include "ns3/mac48-address.h"
#include "ns3/node.h"
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/named_function_params.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>
#include <boost/property_map/transform_value_property_map.hpp>
#include "ns3/channel.h"

using namespace boost;

namespace ns3 {

using Edge = std::pair<uint32_t, uint32_t>;
using Graph = adjacency_list<vecS, vecS, undirectedS, no_property, Edge>;
using Vertex = graph_traits<Graph>::vertex_descriptor;

// weights have to define operator+ and operator<
template <class Weight>
class WeightCalc
{
public:
  virtual Weight GetInitialWeight () const = 0;
  virtual Weight GetNonViableWeight () const = 0;
  virtual Weight GetWeight (Edge &) const = 0;
};

class IntegerWeightCalc : public WeightCalc<int>
{
private:
  unordered_map<Edge, int> m_weights;

public:
  IntegerWeightCalc ();
  ~IntegerWeightCalc ();

  int GetInitialWeight () const override;
  int GetNonViableWeight () const override;
  int GetWeight (Edge &) const override;

  void UpdateEdgeWeight (Edge &, int);
};

class Topology : public Object
{
public:
  static TypeId GetTypeId (void);
  Topology ();
  ~Topology ();

  static void AddNodeIpAddress (Ptr<Node> node, Ipv4Address ip);
  static void AddEdge (Ptr<Node> n1, Ptr<Node> n2, Ptr<Channel> channel);

  template <class Weight = int>
  static std::vector<Ptr<Node>>
  DijkstraShortestPath (Ptr<Node> src, Ptr<Node> dst,
                        WeightCalc<Weight> &weight_calc = m_default_calc);

  template <class Weight = int>
  static std::vector<Ptr<Node>>
  DijkstraShortestPath (Ptr<Node> src, Ipv4Address dst,
                        WeightCalc<Weight> &weight_calc = m_default_calc);

  template <class Weight = int>
  static std::vector<Ptr<Node>>
  DijkstraShortestPath (Ipv4Address src, Ptr<Node> dst,
                        WeightCalc<Weight> &weight_calc = m_default_calc);

  template <class Weight = int>
  static std::vector<Ptr<Node>>
  DijkstraShortestPath (Ipv4Address src, Ipv4Address dst,
                        WeightCalc<Weight> &weight_calc = m_default_calc);

  template <class Weight = int>
  static std::vector<Ptr<Node>>
  DijkstraShortestPath (Ptr<Node> src, Mac48Address dst,
                        WeightCalc<Weight> &weight_calc = m_default_calc);

  template <class Weight = int>
  static std::vector<Ptr<Node>>
  DijkstraShortestPath (Mac48Address src, Ptr<Node> dst,
                        WeightCalc<Weight> &weight_calc = m_default_calc);

  template <class Weight = int>
  static std::vector<Ptr<Node>>
  DijkstraShortestPath (Mac48Address src, Mac48Address dst,
                        WeightCalc<Weight> &weight_calc = m_default_calc);

  template <class Weight = int>
  static std::vector<Ptr<Node>>
  DijkstraShortestPath (std::string src, std::string dst,
                        WeightCalc<Weight> &weight_calc = m_default_calc);

  template <class Weight = int>
  static std::vector<Ptr<Node>>
  DijkstraShortestPaths (Ptr<Node> src, WeightCalc<Weight> &weight_calc = m_default_calc);

  template <class Weight = int>
  static std::vector<Ptr<Node>>
  DijkstraShortestPaths (Ipv4Address src, WeightCalc<Weight> &weight_calc = m_default_calc);

  template <class Weight = int>
  static std::vector<Ptr<Node>>
  DijkstraShortestPaths (std::string src, WeightCalc<Weight> &weight_calc = m_default_calc);

  template <class Weight = int>
  static std::vector<std::pair<std::vector<Ptr<Node>>, Weight>>
  DijkstraShortestPaths (Ptr<Node> src, Ptr<Node> dst,
                         WeightCalc<Weight> &weight_calc = m_default_calc);

  static Graph GetGraph ();
  static IntegerWeightCalc &GetDefaultWeightCalc ();

  static Ptr<Node> VertexToNode (Vertex vd);
  static std::vector<Ptr<Node>> VertexToNode (std::vector<Vertex> path);

  static Ptr<Channel> GetChannel (Ptr<Node> n1, Ptr<Node> n2);

private:
  static Graph m_graph;
  static IntegerWeightCalc m_default_calc;

  static std::map<Ipv4Address, Ptr<Node>> m_ip_to_node;
  static std::map<Mac48Address, Ptr<Node>> m_mac_to_node;
  static std::map<Edge, Ptr<Channel>> m_channels;

  template <class Weight>
  static std::vector<Vertex> DijkstraShortestPathsInternal (Vertex src,
                                                            WeightCalc<Weight> &weight_calc);

  template <class Weight>
  static std::vector<Vertex> DijkstraShortestPathInternal (Vertex src, Vertex dst,
                                                           WeightCalc<Weight> &weight_calc);

  static Ptr<Channel> GetChannel (Edge e);
};

} // namespace ns3

/********************************************************************
 *  Implementation of the templates declared above.
 ********************************************************************/

namespace ns3 {

template <class Weight>
std::vector<Vertex>
Topology::DijkstraShortestPathsInternal (Vertex src, WeightCalc<Weight> &weight_calc)
{
  std::vector<Vertex> predecessors (num_vertices (m_graph));
  dijkstra_shortest_paths (
      m_graph, src,
      predecessor_map (&predecessors[0])
          .weight_map (make_transform_value_property_map (
              [&weight_calc] (Edge &e) -> Weight { return weight_calc.GetWeight (e); },
              get (edge_bundle, m_graph)))
          .distance_zero (weight_calc.GetInitialWeight ())
          .distance_inf (weight_calc.GetNonViableWeight ()));
  return predecessors;
}

template <class Weight>
std::vector<Vertex>
Topology::DijkstraShortestPathInternal (Vertex src, Vertex dst, WeightCalc<Weight> &weight_calc)
{
  std::vector<Vertex> predecessors = DijkstraShortestPathsInternal<Weight> (src, weight_calc);
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

template <class Weight>
inline std::vector<Ptr<Node>>
Topology::DijkstraShortestPath (Ptr<Node> src, Ptr<Node> dst, WeightCalc<Weight> &weight_calc)
{
  Vertex source = vertex (src->GetId (), m_graph);
  Vertex target = vertex (dst->GetId (), m_graph);
  return VertexToNode (DijkstraShortestPathInternal<Weight> (source, target, weight_calc));
}

template <class Weight>
inline std::vector<Ptr<Node>>
Topology::DijkstraShortestPath (Ptr<Node> src, Ipv4Address dst, WeightCalc<Weight> &weight_calc)
{
  return DijkstraShortestPath<Weight> (src, m_ip_to_node[dst], weight_calc);
}

template <class Weight>
inline std::vector<Ptr<Node>>
Topology::DijkstraShortestPath (Ipv4Address src, Ptr<Node> dst, WeightCalc<Weight> &weight_calc)
{
  return DijkstraShortestPath<Weight> (m_ip_to_node[src], dst, weight_calc);
}

template <class Weight>
inline std::vector<Ptr<Node>>
Topology::DijkstraShortestPath (Ipv4Address src, Ipv4Address dst, WeightCalc<Weight> &weight_calc)
{
  return DijkstraShortestPath<Weight> (m_ip_to_node[src], m_ip_to_node[dst], weight_calc);
}

template <class Weight>
inline std::vector<Ptr<Node>>
Topology::DijkstraShortestPath (Ptr<Node> src, Mac48Address dst, WeightCalc<Weight> &weight_calc)
{
  return DijkstraShortestPath<Weight> (src, m_mac_to_node[dst], weight_calc);
}

template <class Weight>
inline std::vector<Ptr<Node>>
Topology::DijkstraShortestPath (Mac48Address src, Ptr<Node> dst, WeightCalc<Weight> &weight_calc)
{
  return DijkstraShortestPath<Weight> (m_mac_to_node[src], dst, weight_calc);
}

template <class Weight>
inline std::vector<Ptr<Node>>
Topology::DijkstraShortestPath (Mac48Address src, Mac48Address dst, WeightCalc<Weight> &weight_calc)
{
  return DijkstraShortestPath<Weight> (m_mac_to_node[src], m_mac_to_node[dst], weight_calc);
}

template <class Weight>
inline std::vector<Ptr<Node>>
Topology::DijkstraShortestPath (std::string src, std::string dst, WeightCalc<Weight> &weight_calc)
{
  Ptr<Node> srcNode = Names::Find<Node> (src);
  Ptr<Node> dstNode = Names::Find<Node> (dst);
  return DijkstraShortestPath<Weight> (srcNode, dstNode, weight_calc);
}

template <class Weight>
inline std::vector<Ptr<Node>>
Topology::DijkstraShortestPaths (Ptr<Node> src, WeightCalc<Weight> &weight_calc)
{
  Vertex src_vertex = vertex (src->GetId (), m_graph);
  return VertexToNode (DijkstraShortestPathsInternal<Weight> (src_vertex, weight_calc));
}

template <class Weight>
inline std::vector<Ptr<Node>>
Topology::DijkstraShortestPaths (Ipv4Address src, WeightCalc<Weight> &weight_calc)
{
  return DijkstraShortestPaths<Weight> (m_ip_to_node[src], weight_calc);
}

template <class Weight>
inline std::vector<Ptr<Node>>
Topology::DijkstraShortestPaths (std::string src, WeightCalc<Weight> &weight_calc)
{
  Ptr<Node> source = Names::Find<Node> (src);
  return DijkstraShortestPaths<Weight> (source, weight_calc);
}

// NOTE: Code partially generated by ChatGPT
template <class Weight>
inline std::vector<std::pair<std::vector<Ptr<Node>>, Weight>>
Topology::DijkstraShortestPaths (Ptr<Node> src, Ptr<Node> dst, WeightCalc<Weight> &weight_calc)
{
  std::vector<Vertex> predecessors (num_vertices (m_graph));
  std::vector<Weight> distances (num_vertices (m_graph));
  Vertex source = vertex (src->GetId (), m_graph);
  Vertex target = vertex (dst->GetId (), m_graph);

  // add distance map
  dijkstra_shortest_paths (
      m_graph, source,
      predecessor_map (&predecessors[0])
          .weight_map (make_transform_value_property_map (
              [&weight_calc] (Edge &e) -> Weight { return weight_calc.GetWeight (e); },
              get (edge_bundle, m_graph)))
          .distance_zero (weight_calc.GetInitialWeight ())
          .distance_inf (weight_calc.GetNonViableWeight ())
          .distance_map (&distances[0]));

  // Create a vector to store the paths and their distances
  std::vector<std::pair<std::vector<Ptr<Node>>, Weight>> paths;

  // Loop through all vertices and store the path and distance from the source
  for (Vertex v = 0; v < num_vertices (m_graph); ++v)
    {
      if (v == source || v == target)
        continue;

      std::vector<Ptr<Node>> path;
      for (Vertex u = v; u != source; u = predecessors[u])
        path.push_back (VertexToNode (u));

      path.push_back (VertexToNode (source));
      std::reverse (path.begin (), path.end ());
      path.push_back (VertexToNode (target));

      paths.emplace_back (std::move (path), distances[v].get ());
    }

  // Sort the paths by distance in ascending order
  std::sort (paths.begin (), paths.end (),
             [] (const auto &lhs, const auto &rhs) { return lhs.second < rhs.second; });

  // Now, the 'paths' vector contains all paths between 'source' and 'target'
  // sorted by their distances in ascending order.
  // Each entry in the 'paths' vector is a pair (path, distance).
  return paths;
}

} // namespace ns3

#endif /* TOPOLOGY_H */
