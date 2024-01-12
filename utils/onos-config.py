#!/usr/bin/env python3

import toml
import json
import re
import argparse
from netaddr import EUI, mac_unix_expanded


class MacAddressManager:
    def __init__(self):
        self._counter = 0

    def new_address(self):
        self._counter += 1
        mac = EUI(self._counter, dialect=mac_unix_expanded)
        return f"{mac}"


def to_Mpbs(data_rate):
    result = re.search(r"(([0-9]*[.])?[0-9]+)([a-zA-z/]+)", data_rate)
    r = float(result.group(1))
    u = result.group(3)

    if u == "bps":
        r = r
    elif u == "b/s":
        r = r
    elif u == "Bps":
        r = r * 8
    elif u == "B/s":
        r = r * 8
    elif u == "kbps":
        r = r * 1000
    elif u == "kb/s":
        r = r * 1000
    elif u == "Kbps":
        r = r * 1000
    elif u == "Kb/s":
        r = r * 1000
    elif u == "kBps":
        r = r * 8000
    elif u == "kB/s":
        r = r * 8000
    elif u == "KBps":
        r = r * 8000
    elif u == "KB/s":
        r = r * 8000
    elif u == "Kib/s":
        r = r * 1024
    elif u == "KiB/s":
        r = r * 8192
    elif u == "Mbps":
        r = r * 1000000
    elif u == "Mb/s":
        r = r * 1000000
    elif u == "MBps":
        r = r * 8000000
    elif u == "MB/s":
        r = r * 8000000
    elif u == "Mib/s":
        r = r * 1048576
    elif u == "MiB/s":
        r = r * 1048576 * 8
    elif u == "Gbps":
        r = r * 1000000000
    elif u == "Gb/s":
        r = r * 1000000000
    elif u == "GBps":
        r = r * 8 * 1000000000
    elif u == "GB/s":
        r = r * 8 * 1000000000
    elif u == "Gib/s":
        r = r * 1048576 * 1024
    elif u == "GiB/s":
        r = r * 1048576 * 1024 * 8

    return str(int(r / 1000000))


def gen_config(topology):
    try:
        nodes_file = toml.load(f"topologies/{topology}/nodes.toml")
        links_file = toml.load(f"topologies/{topology}/links.toml")
    except FileNotFoundError:
        print("TOML files not found")
        exit(1)

    # *********** Parse nodes ***********

    devices = {}
    ports = {}
    links = {}

    switches = {}
    switches_counter = 0
    switches_ports = {}
    ip_counter = 1

    mac_manager = MacAddressManager()

    for node in dict(sorted(nodes_file.items(), key=lambda x: x[0])):
        node_type = nodes_file[node].pop("type")

        if node_type == "host":
            pass
        elif node_type == "switch":
            switches_counter += 1
            switches_ports[node] = 0
            deviceId = "of:{:016x}".format(switches_counter)
            switches[node] = deviceId
            devices[deviceId] = {
                "annotations": {
                    "entries": {
                        "emsId": node
                    }
                }
            }
        else:
            print("Unknown Node type")
            exit(1)

    # *********** Parse links ***********

    for link in dict(sorted(links_file.items(), key=lambda x: x[0])):
        edge0, edge1 = links_file[link].pop("edges")
        data_rate = links_file[link].pop("dataRate", "1000Gbps")

        if edge0 in switches and edge1 in switches:
            mac_manager.new_address()
            mac_manager.new_address()
            switches_ports[edge0] += 1
            switches_ports[edge1] += 1
            link1 = f"{switches[edge0]}/{switches_ports[edge0]}-{switches[edge1]}/{switches_ports[edge1]}"
            links[link1] = {
                "basic": {
                    "bidirectional": True,
                    "bandwidth": to_Mpbs(data_rate),
                    "type": "DIRECT",
                }
            }
        elif edge0 in switches:
            switches_ports[edge0] += 1
            port = f"{switches[edge0]}/{switches_ports[edge0]}"
            ports[port] = {}
            ports[port]["interfaces"] = [
                {
                    "name": f"eth{switches_ports[edge0]}",
                    "ips": [f"10.1.1.{ip_counter}/24"],
                    "mac": mac_manager.new_address(),
                }
            ]
            ip_counter += 1
            mac_manager.new_address()
        else:  # edge1 in switches
            mac_manager.new_address()
            switches_ports[edge1] += 1
            port = f"{switches[edge1]}/{switches_ports[edge1]}"
            ports[port] = {}
            ports[port]["interfaces"] = [
                {
                    "name": f"eth{switches_ports[edge1]}",
                    "ips": [f"10.1.1.{ip_counter}/24"],
                    "mac": mac_manager.new_address(),
                }
            ]
            ip_counter += 1

    data = {
        "devices": devices,
        "ports": ports,
        "links": links,
        "apps": {
            "org.onosproject.core": {"core": {"linkDiscoveryMode": "STRICT"}}
        },
    }

    with open(f"topologies/{topology}/switches_config.json", "w") as out_file:
        json.dump(data, out_file, indent=4)


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("--topology", "-t", dest="topology", type=str)
    args = parser.parse_args()

    gen_config(args.topology)
