# Flexcomm Simulator

A Simulation Tool to help develop and assess multiple strategies towards green networks and communications driven by real-time information from the power grid (*Energy Flexibility*). 

## Citing us

Please cite the following paper if you use Flexcomm Simulator:

```bibtex
@inproceedings{Flexcomm-Simulator,
    title = {Flexcomm Simulator: Exploring Energy Flexibility in Software Defined Networks with Ns-3},
    author = {Monteiro, Rui Pedro C. and Silva, Jo\~{a}o Marco C.},
    booktitle = {Proceedings of the 2023 Workshop on Ns-3},
    year = {2023},
    url = {https://doi.org/10.1145/3592149.3592160},
}
```

---

# Getting started with Flexcomm Simulator

This README contains the basic instructions to build and run the Flexcomm Simulator. For more details on the tool and how to create simulation scenarios refer to the [User Manual/Documentation](https://github.com/RuiCunhaM/Flexcomm-Simulator-User-Manual).

## Building the Simulator

1. Install dependencies (Ubuntu):
    ```
    sudo apt-get install build-essential gcc g++ python3 cmake pkg-config autoconf libtool libboost-dev
    ```

    <details open>
    <summary>1(a). Install optional dependencies:</summary>

    - To use the external `EnergyAPI` 
        ```
        pip3 install flask 
        ```

    - To use the experimental SNMP support 
        ```
        git clone -b file-lock https://github.com/RuiCunhaM/snmpsim
        cd snmpsim
        pip3 install .
        ```

    - To run distributed simulations:
        ```
        sudo apt-get install mpi 
        ```

    - To use the utility scripts for visualizing results 
        ```
        pip3 install matplotlib toml scipy tabulate networkx 
        ```
    </details>

3. Clone the repository:
    ```
    git clone https://github.com/RuiCunhaM/Flexcomm-Simulator
    ```

4. Build the simulator 
    ```
    ./configure
    make configure 
    make
    ```

**Note: Currently the support for [Distributed Simulations](https://github.com/RuiCunhaM/Flexcomm-Simulator/tree/mpi) and [SNMP](https://github.com/RuiCunhaM/Flexcomm-Simulator/snmp) are detached in their own branches. If you intend to use any of these features please checkout the correspondent branch. Further details on how to use these features are also available in the [manual](https://github.com/RuiCunhaM/Flexcomm-Simulator-User-Manual).**

## Running a simulation

For any `topology` in the `topologies` folder run:
```
make run TOPO=<topology> <flags>
```

For a complete list of the available `flags` see the [full documentation](https://github.com/RuiCunhaM/Flexcomm-Simulator-User-Manual#runtime-flags)

## Example Topology

The `example` scenario included with the simulator consists of a very simple network topology with two alternative paths connecting two hosts. Traffic is generated from host `H1` towards host `H2` for 5 minutes. In one of the paths, the energy flexibility of the switches decreases over time while in the other one, it increases. 

The `SimpleController` calculates each minute the shortest paths using Dijkstra's Algorithm and the number of hops applying the routing rules accordingly. 

The `SimpleControllerFlex` also calculates each minute the shortest paths using Dijkstra's Algorithm but considering the energy flexibility of each switch.

#### Simple Controller

```
make run TOPO=example CONTROLLER=ns3::SimpleController OUTPUTS=simple
```

#### Simple Controller (Flexibility Aware)

```
make run TOPO=example CONTROLLER=ns3::SimpleControllerFlex OUTPUTS=simple_flex
```

## Interpret results

- Flow statistics:
    ```
    ./utils/flowmo-parser.py <path to 'flow-monitor.xml'>
    ```

-  Energy consumption:
    ```
    ./utils/ecofen-parser.py <path to 'ecofen-trace'>
    ```

-  Switch statistics:
    ```
    ./utils/switch-stats_parser.py <path to 'switch-stats-trace'>
    ```

-  Link statistics:
    ```
    ./utils/link-stats-parser.py <path to 'link-stats-trace'>
    ```

---

## Built with:

- [ns-3](https://www.nsnam.org/)
- [OFSwitch13](http://www.lrc.ic.unicamp.br/ofswitch13/)
- [ECOFEN](https://people.irisa.fr/Anne-Cecile.Orgerie/ECOFEN)
- [toml++](https://marzer.github.io/tomlplusplus/)
- [nlohmann/json](https://json.nlohmann.me/)

