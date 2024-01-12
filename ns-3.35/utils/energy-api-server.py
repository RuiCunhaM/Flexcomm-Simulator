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
import sys
import os.path as path

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


def load_data():
    if len(sys.argv) < 2:
        print("Missing topology path")
        exit(1)
    topo_path = sys.argv[1]
    if not path.exists(topo_path):
        print(f"Path not found {topo_path}")
        exit(1)

    global FLEX, ESTIMATE

    flex_path = f"{topo_path}/flex.json"
    if path.isfile(flex_path):
        with open(flex_path) as flexFile:
            FLEX = json.load(flexFile)

    esti_path = f"{topo_path}/estimate.json"
    if path.isfile(esti_path):
        with open(esti_path) as estimateFile:
            ESTIMATE = json.load(estimateFile)


if __name__ == "__main__":
    load_data()
    app.run()
