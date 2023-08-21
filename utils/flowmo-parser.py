#!/usr/bin/env python3

from __future__ import division
import sys

try:
    from xml.etree import cElementTree as ElementTree
except ImportError:
    from xml.etree import ElementTree


def parse_time_ns(tm):
    if tm.endswith("ns"):
        return float(tm[:-2])
    raise ValueError(tm)


## FiveTuple
class FiveTuple(object):
    ## class variables
    ## @var sourceAddress
    #  source address
    ## @var destinationAddress
    #  destination address
    ## @var protocol
    #  network protocol
    ## @var sourcePort
    #  source port
    ## @var destinationPort
    #  destination port
    ## @var __slots_
    #  class variable list
    __slots_ = [
        "sourceAddress",
        "destinationAddress",
        "protocol",
        "sourcePort",
        "destinationPort",
    ]

    def __init__(self, el):
        """! The initializer.
        @param self The object pointer.
        @param el The element.
        """
        self.sourceAddress = el.get("sourceAddress")
        self.destinationAddress = el.get("destinationAddress")
        self.sourcePort = int(el.get("sourcePort"))
        self.destinationPort = int(el.get("destinationPort"))
        self.protocol = int(el.get("protocol"))


## Histogram
class Histogram(object):
    ## class variables
    ## @var bins
    #  histogram bins
    ## @var nbins
    #  number of bins
    ## @var number_of_flows
    #  number of flows
    ## @var __slots_
    #  class variable list
    __slots_ = "bins", "nbins", "number_of_flows"

    def __init__(self, el=None):
        """! The initializer.
        @param self The object pointer.
        @param el The element.
        """
        self.bins = []
        if el is not None:
            # self.nbins = int(el.get('nBins'))
            for bin in el.findall("bin"):
                self.bins.append(
                    (
                        float(bin.get("start")),
                        float(bin.get("width")),
                        int(bin.get("count")),
                    )
                )


## Flow
class Flow(object):
    ## class variables
    ## @var flowId
    #  delay ID
    ## @var delayMean
    #  mean delay
    ## @var jitterMean
    #  mean jitter
    ## @var packetLossRatio
    #  packet loss ratio
    ## @var rxBitrate
    #  receive bit rate
    ## @var txBitrate
    #  transmit bit rate
    ## @var fiveTuple
    #  five tuple
    ## @var packetSizeMean
    #  packet size mean
    ## @var probe_stats_unsorted
    #  unsirted probe stats
    ## @var hopCount
    #  hop count
    ## @var flowInterruptionsHistogram
    #  flow histogram
    ## @var delayHistogram
    #  delay histogram
    ## @var jitterHistogram
    #  jitter histogram
    ## @var rx_duration
    #  receive duration
    ## @var __slots_
    #  class variable list
    __slots_ = [
        "flowId",
        "delayMean",
        "jitterMean",
        "packetLossRatio",
        "rxBitrate",
        "txBitrate",
        "fiveTuple",
        "packetSizeMean",
        "probe_stats_unsorted",
        "hopCount",
        "flowInterruptionsHistogram",
        "delayHistogram",
        "jitterHistogram",
        "rx_duration",
    ]

    def __init__(self, flow_el):
        """! The initializer.
        @param self The object pointer.
        @param flow_el The element.
        """
        self.flowId = int(flow_el.get("flowId"))
        rxPackets = float(flow_el.get("rxPackets"))

        tx_duration = (
            parse_time_ns(flow_el.get("timeLastTxPacket"))
            - parse_time_ns(flow_el.get("timeFirstTxPacket"))
        ) * 1e-9
        rx_duration = (
            parse_time_ns(flow_el.get("timeLastRxPacket"))
            - parse_time_ns(flow_el.get("timeFirstRxPacket"))
        ) * 1e-9
        self.rx_duration = rx_duration
        self.probe_stats_unsorted = []
        if rxPackets:
            self.hopCount = (
                float(flow_el.get("timesForwarded")) / rxPackets + 1
            )
        else:
            self.hopCount = -1000
        if rxPackets:
            self.delayMean = (
                float(flow_el.get("delaySum")[:-2]) / rxPackets * 1e-9
            )
            self.jitterMean = (
                float(flow_el.get("jitterSum")[:-2]) / rxPackets * 1e-9
            )
            self.packetSizeMean = float(flow_el.get("rxBytes")) / rxPackets
        else:
            self.delayMean = None
            self.packetSizeMean = None
        if rx_duration > 0:
            self.rxBitrate = float(flow_el.get("rxBytes")) * 8 / rx_duration
        else:
            self.rxBitrate = None
        if tx_duration > 0:
            self.txBitrate = float(flow_el.get("txBytes")) * 8 / tx_duration
        else:
            self.txBitrate = None
        lost = float(flow_el.get("lostPackets"))
        # print "rxBytes: %s; txPackets: %s; rxPackets: %s; lostPackets: %s" % (flow_el.get('rxBytes'), txPackets, rxPackets, lost)
        if rxPackets == 0:
            self.packetLossRatio = None
        else:
            self.packetLossRatio = lost / (rxPackets + lost)

        interrupt_hist_elem = flow_el.find("flowInterruptionsHistogram")
        if interrupt_hist_elem is None:
            self.flowInterruptionsHistogram = None
        else:
            self.flowInterruptionsHistogram = Histogram(interrupt_hist_elem)

        delay_hist_elem = flow_el.find("delayHistogram")
        if delay_hist_elem is None:
            self.delayHistogram = None
        else:
            self.delayHistogram = Histogram(delay_hist_elem)

        jitter_hist_elem = flow_el.find("jitterHistogram")
        if jitter_hist_elem is None:
            self.jitterHistogram = None
        else:
            self.jitterHistogram = Histogram(jitter_hist_elem)


## ProbeFlowStats
class ProbeFlowStats(object):
    ## class variables
    ## @var probeId
    #  probe ID
    ## @var packets
    #  network packets
    ## @var bytes
    #  bytes
    ## @var delayFromFirstProbe
    #  delay from first probe
    ## @var __slots_
    #  class variable list
    __slots_ = ["probeId", "packets", "bytes", "delayFromFirstProbe"]


## Simulation
class Simulation(object):
    ## class variables
    ## @var flows
    #  list of flows
    def __init__(self, simulation_el):
        """! The initializer.
        @param self The object pointer.
        @param simulation_el The element.
        """
        self.flows = []
        (FlowClassifier_el,) = simulation_el.findall("Ipv4FlowClassifier")
        flow_map = {}
        for flow_el in simulation_el.findall("FlowStats/Flow"):
            flow = Flow(flow_el)
            flow_map[flow.flowId] = flow
            self.flows.append(flow)
        for flow_cls in FlowClassifier_el.findall("Flow"):
            flowId = int(flow_cls.get("flowId"))
            flow_map[flowId].fiveTuple = FiveTuple(flow_cls)

        for probe_elem in simulation_el.findall("FlowProbes/FlowProbe"):
            probeId = int(probe_elem.get("index"))
            for stats in probe_elem.findall("FlowStats"):
                flowId = int(stats.get("flowId"))
                s = ProbeFlowStats()
                s.packets = int(stats.get("packets"))
                s.bytes = float(stats.get("bytes"))
                s.probeId = probeId
                if s.packets > 0:
                    s.delayFromFirstProbe = parse_time_ns(
                        stats.get("delayFromFirstProbeSum")
                    ) / float(s.packets)
                else:
                    s.delayFromFirstProbe = 0
                flow_map[flowId].probe_stats_unsorted.append(s)


def main(argv):
    file_obj = open(argv[1])
    print("Reading XML file ", end=" ")

    sys.stdout.flush()
    level = 0
    sim_list = []
    for event, elem in ElementTree.iterparse(
        file_obj, events=("start", "end")
    ):
        if event == "start":
            level += 1
        if event == "end":
            level -= 1
            if level == 0 and elem.tag == "FlowMonitor":
                sim = Simulation(elem)
                sim_list.append(sim)
                elem.clear()  # won't need this any more
                sys.stdout.write(".")
                sys.stdout.flush()
    print(" done.")

    for sim in sim_list:
        for flow in sim.flows:
            t = flow.fiveTuple
            proto = {6: "TCP", 17: "UDP"}[t.protocol]
            print(
                "\nFlowID: %i (%s %s/%s --> %s/%i)"
                % (
                    flow.flowId,
                    proto,
                    t.sourceAddress,
                    t.sourcePort,
                    t.destinationAddress,
                    t.destinationPort,
                )
            )
            print("\tBitrate:")
            if flow.txBitrate is None:
                print("\t\tTX bitrate: None")
            else:
                print("\t\tTX bitrate: %.2f kbit/s" % (flow.txBitrate * 1e-3,))
            if flow.rxBitrate is None:
                print("\t\tRX bitrate: None")
            else:
                print("\t\tRX bitrate: %.2f kbit/s" % (flow.rxBitrate * 1e-3,))
            print("\tDelay:")
            if flow.delayMean is None:
                print("\t\tMean Delay: None")
            else:
                print("\t\tMean Delay: %.2f ms" % (flow.delayMean * 1e3,))
            if flow.delayHistogram:
                print(
                    "\t\tMax Delay: %.2f ms"
                    % (
                        max(map(lambda x: x[0], flow.delayHistogram.bins))
                        * 1e3
                    )
                )
                print(
                    "\t\tMin Delay: %.2f ms"
                    % (
                        min(map(lambda x: x[0], flow.delayHistogram.bins))
                        * 1e3
                    )
                )
            print("\tJitter:")
            if flow.jitterMean is None:
                print("\t\tMean Jitter: None")
            else:
                print("\t\tMean Jitter: %.2f ms" % (flow.jitterMean * 1e3,))
            if flow.jitterHistogram:
                print(
                    "\t\tMax Jitter: %.2f ms"
                    % (
                        max(map(lambda x: x[0], flow.jitterHistogram.bins))
                        * 1e3
                    )
                )
                print(
                    "\t\tMin Jitter: %.2f ms"
                    % (
                        min(map(lambda x: x[0], flow.jitterHistogram.bins))
                        * 1e3
                    )
                )
            print("\tPacket Loss:")
            if flow.packetLossRatio is None:
                print("\t\tPacket Loss Ratio: None")
            else:
                print(
                    "\t\tPacket Loss Ratio: %.2f %%"
                    % (flow.packetLossRatio * 100)
                )


if __name__ == "__main__":
    main(sys.argv)
