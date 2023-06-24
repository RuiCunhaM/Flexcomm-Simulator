#!/usr/bin/env python3

import matplotlib.pylab as plt
import regex as re
import argparse
from tabulate import tabulate
from sortedcontainers import SortedSet

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

x = SortedSet()
y = {}
for line in lines:
    # time nodeName consumption zone
    match = re.match(r"(\d+(.\d+)?) (\w+) (\d+(.\d+)?)", line)
    if match is not None:
        time = float(match.group(1))
        link = match.group(3)
        utilization = float(match.group(4))
        x.add(time)

        if link not in y:
            y[link] = [utilization]
        else:
            y[link].append(utilization)

table = []
color_index = 0
for link, usages in y.items():
    table.append([link, max(usages), sum(usages) / len(usages)])

    if args.drawPlot:
        plt.plot(
            x,
            usages,
            color=colors[color_index % len(colors)],
            marker=markers[color_index % len(markers)],
            linestyle="none",
            label=link,
        )
        color_index += 1

print(
    tabulate(
        table,
        headers=["Link", "Max (%)", "Average (%)"],
        tablefmt="pretty",
    )
)

if args.drawPlot:
    plt.show()
