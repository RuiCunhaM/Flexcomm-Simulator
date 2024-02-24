#!/usr/bin/env python3

#
# The GPLv2 License (GPLv2)
#
# Copyright (c) 2023 Rui Pedro C. Monteiro
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http: //www.gnu.org/licenses/>.
#
# Author: Rui Pedro C. Monteiro <rui.p.monteiro@inesctec.pt>
#

from flask import Flask, jsonify, request, abort
import json
import argparse

FLEX = {}
ESTIMATE = {}

app = Flask(__name__)


@app.route("/flex", methods=["GET"])
def get_flex():
    args = request.args
    id = args.get("id", "")

    if not id:
        return jsonify(FLEX)

    if id not in FLEX:
        abort(404)

    return jsonify({id: FLEX[id]})


@app.route("/estimate", methods=["GET"])
def get_estimate():
    args = request.args
    id = args.get("id", "")

    if not id:
        return jsonify(ESTIMATE)

    if id not in ESTIMATE:
        abort(404)

    return jsonify({id: ESTIMATE[id]})


def parse_args():
    parser = argparse.ArgumentParser()
    parser.add_argument("-f", "--flexibility", type=str, required=True)
    parser.add_argument("-e", "--estimate", type=str, required=True)

    return parser.parse_args()


def load_data():
    args = parse_args()

    global FLEX, ESTIMATE

    try:
        with open(args.flexibility) as flexFile, open(args.estimate) as estimateFile:
            FLEX = json.load(flexFile)
            ESTIMATE = json.load(estimateFile)
    except OSError as e:
        print(e.strerror, e.filename)
        exit(1)


if __name__ == "__main__":
    load_data()
    app.run()
