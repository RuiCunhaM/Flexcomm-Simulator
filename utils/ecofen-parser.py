#!/usr/bin/env python3

import matplotlib.pylab as plt
import argparse
import csv

from sortedcontainers import SortedSet
from tabulate import tabulate
from common import colors, markers


def parse_args():
    parser = argparse.ArgumentParser()
    parser.add_argument("file", type=str)
    parser.add_argument("--plot", "-p", dest="drawPlot", action="store_true")
    parser.set_defaults(drawPlot=False)
    return parser.parse_args()


def report_values(consumptions, total_con):
    table = []
    for node in consumptions:
        table.append([node, consumptions[node]])
    print(
        tabulate(
            table, headers=["Nodes", "Consumption (J)"], tablefmt="pretty"
        )
    )
    print("Total Consumption: {} kW".format(total_con / 1000))


def draw_plots(x, y):
    color_index = 0

    for node in y:
        plt.plot(
            x,
            y[node],
            label=node,
            color=colors[color_index % len(colors)],
            marker=markers[color_index % len(markers)],
            linestyle="none",
        )
        color_index += 1

    aggregated = []
    for i in range(len(x)):
        aggregated.append(0.0)
        for node in y:
            aggregated[i] += y[node][i]

    plt.plot(
        x,
        aggregated,
        "ro",
        label="Aggregated",
    )

    plt.legend()
    plt.xlabel("Time Interval (s)")
    plt.ylabel("Consumption (W)")
    plt.show()


def read_file(file_path, drawPlot):
    with open(file_path) as file:
        data = csv.reader(file, delimiter=";")

        # Parse File
        x = SortedSet()
        y = {}
        for row in data:
            time = float(row[0])
            node = row[1]
            conso = float(row[2])
            x.add(time)
            if node not in y:
                y[node] = [conso]
            else:
                y[node].append(conso)

        # Calculate consumptions
        consumptions = {}
        total_con = 0.0

        for node in y:
            consumptions[node] = 0.0
            previous_time = 0.0

            for i in range(len(x)):
                current_time = x[i]
                interval = current_time - previous_time
                previous_time = current_time
                consumption = interval * y[node][i]
                consumptions[node] += consumption
                total_con += consumption

        report_values(consumptions, total_con)
        if drawPlot:
            draw_plots(x, y)


if __name__ == "__main__":
    args = parse_args()
    read_file(args.file, args.drawPlot)
