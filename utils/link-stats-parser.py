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


def report_values(y):
    table = []
    for link, usages in y.items():
        table.append([link, max(usages), sum(usages) / len(usages)])

    print(
        tabulate(
            table,
            headers=["Link", "Max (%)", "Average (%)"],
            tablefmt="pretty",
        )
    )


def draw_plot(x, y):
    color_index = 0
    for link, usages in y.items():
        plt.plot(
            x,
            usages,
            color=colors[color_index % len(colors)],
            marker=markers[color_index % len(markers)],
            linestyle="none",
            label=link,
        )
        color_index += 1

    plt.legend()
    plt.xlabel("Time Interval (s)")
    plt.ylabel("Usage (%)")
    plt.show()


def read_file(file_path, drawPlot):
    with open(file_path) as file:
        data = csv.reader(file, delimiter=";")

        x = SortedSet()
        y = {}
        for row in data:
            time = float(row[0])
            link = row[1]
            utilization = float(row[2])
            x.add(time)
            if link not in y:
                y[link] = [utilization]
            else:
                y[link].append(utilization)

        report_values(y)
        if drawPlot:
            draw_plot(x, y)


if __name__ == "__main__":
    args = parse_args()
    read_file(args.file, args.drawPlot)
