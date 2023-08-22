#!/usr/bin/env python3

import toml
import argparse
import networkx as nx
import matplotlib.pyplot as plt


def vis_topo(topology, showHosts):
    try:
        nodes_file = toml.load(f"topologies/{topology}/nodes.toml")
        links_file = toml.load(f"topologies/{topology}/links.toml")
    except FileNotFoundError:
        print("Topology not found")
        exit(1)

    G = nx.Graph()

    # *********** Parse nodes ***********

    for node, params in nodes_file.items():
        if params["type"] == "switch":
            G.add_node(node)
        elif params["type"] == "host":
            if showHosts:
                G.add_node(node)
        else:
            print("Unknown Node type")
            exit(1)

    # *********** Parse links ***********

    for _, params in links_file.items():
        n1, n2 = params["edges"]
        if n1 in G.nodes and n2 in G.nodes:
            G.add_edge(n1, n2)

    # *********** Display ***********
    nx.draw_kamada_kawai(G, with_labels=True)
    plt.show()


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("--topology", "-t", dest="topology", type=str)
    parser.add_argument(
        "--showHosts", "-sh", dest="showHosts", action="store_true"
    )
    parser.set_defaults(showHosts=False)
    args = parser.parse_args()

    vis_topo(args.topology, args.showHosts)
