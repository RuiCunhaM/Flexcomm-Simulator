#!/usr/bin/env python3

import matplotlib.pylab as plt
import regex as re
import argparse
from sortedcontainers import SortedSet
from tabulate import tabulate

colors = [
    "gray",
    "red",
    "sienna",
    "sandybrown",
    "y",
    "g",
    "c",
    "blue",
    "purple",
    "violet",
    "pink",
]
markers = ["o", "v", "^", "<", ">", "s", "D", "h", "p", "X", "*"]

# Define arguments
parser = argparse.ArgumentParser()
parser.add_argument(
    "file", metavar="PATH/TO/FILE", type=argparse.FileType("r")
)
parser.add_argument("--plot", "-p", dest="drawPlot", action="store_true")
parser.set_defaults(drawPlot=False)
args = parser.parse_args()

# Read file
# FIXME: This is not ideal
lines = args.file.readlines()

# Parse File
x = SortedSet()
y = {}
for line in lines:
    # time nodeName consumption
    match = re.match(r"(\d+(.\d+)?) (\w+) (\d+(.\d+)?)", line)
    if match is not None:
        node = match.group(3)
        time = float(match.group(1))
        conso = float(match.group(4))
        x.add(time)
        if node not in y:
            y[node] = [conso]
        else:
            y[node].append(conso)

# Calculate consumptions
consumptions = {}
total_con = 0.0
color_index = 0

for node in y:
    consumptions[node] = 0.0
    previous_time = 0.0

    if args.drawPlot:
        plt.plot(
            x,
            y[node],
            label=node,
            color=colors[color_index % len(colors)],
            marker=markers[color_index % len(markers)],
            linestyle="none",
        )
        color_index += 1

    for i in range(len(x)):
        current_time = x[i]
        interval = current_time - previous_time
        previous_time = current_time
        consumption = interval * y[node][i]
        consumptions[node] += consumption
        total_con += consumption

# Calculate aggregated
if args.drawPlot:
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

# Report consumptions
table = []
for node in consumptions:
    table.append([node, consumptions[node]])
print(tabulate(table, headers=["Nodes", "Consumption (J)"], tablefmt="pretty"))

print("Total Consumption: {} kW".format(total_con / 1000))
