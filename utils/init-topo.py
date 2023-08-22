#!/usr/bin/env python3

import os
import toml
import argparse


# Create nodes.toml
def create_nodes(topo_path):
    with open(os.path.join(topo_path, "nodes.toml"), "w") as nodes_file:
        hosts = {}
        switches = {}

        for i in range(args.n_hosts):
            hosts[f"h{i+1}"] = {"type": "host"}

        for i in range(args.n_switches):
            switches[f"s{i+1}"] = {
                "type": "switch",
                "cpuCapacity": "100Gbps",
            }

        nodes_file.write("#### HOSTS ####\n")
        toml.dump(hosts, nodes_file)
        nodes_file.write("\n#### SWITCHES ####\n")
        toml.dump(switches, nodes_file)


# Create configs.toml
def create_configs(topo_path):
    with open(os.path.join(topo_path, "configs.toml"), "w") as configs_file:
        configs = {
            "simulator": {
                "stopTime": "60s",
            },
            "flowMonitor": {
                "enable": False,
            },
            "ecofen": {
                "logFile": False,
                "logInterval": "5s",
                "interval": "5s",
            },
            "switchStats": {
                "enable": False,
                "interval": "5s",
            },
            "linkStats": {
                "logFile": False,
                "logInterval": "5s",
                "interval": "5s",
            },
        }

        toml.dump(configs, configs_file)


# Create links.toml
def create_links(topo_path):
    with open(os.path.join(topo_path, "links.toml"), "w") as links_file:
        links = {}

        for i in range(args.n_links):
            links[f"l{i+1}"] = {
                "edges": ["<node1>", "<node2>"],
                "dataRate": "100Gbps",
                "delay": "1ms",
                "pcap": False,
            }

        toml.dump(links, links_file)


# Create applications.toml
def create_apps(topo_path):
    with open(os.path.join(topo_path, "applications.toml"), "w") as apps_file:
        apps = {}

        for i in range(args.n_apps):
            apps[f"app{i+1}"] = {
                "type": "<type>",
                "startTime": "1s",
                "stopTime": "60s",
                "host": "<host>",
                "remote": "<remote>",
            }

        toml.dump(apps, apps_file)


# Create energy-templates.toml
def create_energy_templates(topo_path):
    open(os.path.join(topo_path, "energy-templates.toml"), "w")


def create_topo():
    topo_path = os.path.join("topologies", args.topo)

    if os.path.exists(topo_path):
        print("Topology already exists")
        exit(1)

    # Create folder
    os.mkdir(topo_path)

    create_nodes(topo_path)
    create_configs(topo_path)
    create_links(topo_path)
    create_apps(topo_path)
    create_energy_templates(topo_path)


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("topo")
    parser.add_argument("--hosts", dest="n_hosts", type=int, default=2)
    parser.add_argument("--switches", dest="n_switches", type=int, default=1)
    parser.add_argument("--links", dest="n_links", type=int, default=2)
    parser.add_argument("--apps", dest="n_apps", type=int, default=1)

    global args
    args = parser.parse_args()

    create_topo()
