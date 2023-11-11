#!/usr/bin/env python3

import matplotlib.pylab as plt
import argparse
import csv

from tabulate import tabulate
from sortedcontainers import SortedSet
from common import colors, markers


def parse_args():
    parser = argparse.ArgumentParser()
    parser.add_argument("file", type=str)
    parser.add_argument("--plot", "-p", dest="drawPlot", action="store_true")
    parser.set_defaults(drawPlot=False)
    return parser.parse_args()


def report_values(cpuUsages, nPackets, nDroppedPackets, nBytes):
    table = []
    for switch in cpuUsages:
        usages = cpuUsages[switch]
        table.append([switch, max(usages), sum(usages) / len(usages)])
    print("CPU Usage:")
    print(
        tabulate(
            table,
            headers=["Switch", "Max (%)", "Average (%)"],
            tablefmt="pretty",
        )
    )
    print("")

    table = []
    for switch in nPackets:
        packets = nPackets[switch]
        table.append(
            [
                switch,
                max(packets),
                sum(packets) / len(packets),
                sum(packets),
            ]
        )
    print("Packets processed:")
    print(
        tabulate(
            table,
            headers=["Switch", "Max", "Average", "Total"],
            tablefmt="pretty",
        )
    )
    print("")

    table = []
    for switch in nDroppedPackets:
        packets = nDroppedPackets[switch]
        table.append(
            [
                switch,
                max(packets),
                sum(packets) / len(packets),
                sum(packets),
            ]
        )
    print("Dropped Packets:")
    print(
        tabulate(
            table,
            headers=["Switch", "Max", "Average", "Total"],
            tablefmt="pretty",
        )
    )
    print("")

    table = []
    for switch in nBytes:
        bytes = nBytes[switch]
        table.append(
            [
                switch,
                max(bytes),
                sum(bytes) / len(bytes),
                sum(bytes),
            ]
        )
    print("Bytes processed:")
    print(
        tabulate(
            table,
            headers=["Switch", "Max", "Average", "Total"],
            tablefmt="pretty",
        )
    )
    print("")


def draw_plot(x, cpuUsages, nPackets, nDroppedPackets, nBytes):
    # CpuUsage graph
    color_index = 0
    for node in cpuUsages:
        plt.plot(
            x,
            cpuUsages[node],
            label=node,
            color=colors[color_index % len(colors)],
            marker=markers[color_index % len(markers)],
            linestyle="none",
        )
        color_index += 1

    # Show plot
    plt.legend()
    plt.xlabel("Time Interval (s)")
    plt.ylabel("Cpu Usage (%)")
    plt.show()

    ################################################################

    # nPackets graph
    color_index = 0
    for node in nPackets:
        plt.plot(
            x,
            nPackets[node],
            label=node,
            color=colors[color_index % len(colors)],
            marker=markers[color_index % len(markers)],
            linestyle="none",
        )
        color_index += 1

    # Show plot
    plt.legend()
    plt.xlabel("Time Interval (s)")
    plt.ylabel("Nº Packets")
    plt.show()

    ################################################################

    # nDroppedPackets graph
    color_index = 0
    for node in nDroppedPackets:
        plt.plot(
            x,
            nDroppedPackets[node],
            label=node,
            color=colors[color_index % len(colors)],
            marker=markers[color_index % len(markers)],
            linestyle="none",
        )
        color_index += 1

    # Show plot
    plt.legend()
    plt.xlabel("Time Interval (s)")
    plt.ylabel("Nº Packets")
    plt.show()

    ################################################################

    # nBytes graph
    color_index = 0
    for node in nBytes:
        plt.plot(
            x,
            nBytes[node],
            color=colors[color_index % len(colors)],
            marker=markers[color_index % len(markers)],
            linestyle="none",
            label=node,
        )
        color_index += 1

    # Show plot
    plt.legend()
    plt.xlabel("Time Interval (s)")
    plt.ylabel("Nº Bytes")
    plt.show()


def read_file(file_path, drawPlot):
    with open(file_path) as file:
        data = csv.reader(file, delimiter=";")

        x = SortedSet()
        cpuUsages = {}
        nPackets = {}
        nDroppedPackets = {}
        nBytes = {}
        for row in data:
            time = float(row[0])
            node = row[1]
            cpu = float(row[2])
            packets = int(row[3])
            droppedPackets = int(row[4])
            bytes = int(row[5])
            x.add(time)

            if node not in cpuUsages:
                cpuUsages[node] = [cpu]
                nPackets[node] = [packets]
                nDroppedPackets[node] = [droppedPackets]
                nBytes[node] = [bytes]
            else:
                cpuUsages[node].append(cpu)
                nPackets[node].append(packets)
                nDroppedPackets[node].append(droppedPackets)
                nBytes[node].append(bytes)

        report_values(cpuUsages, nPackets, nDroppedPackets, nBytes)
        if drawPlot:
            draw_plot(x, cpuUsages, nPackets, nDroppedPackets, nBytes)


if __name__ == "__main__":
    args = parse_args()
    read_file(args.file, args.drawPlot)
