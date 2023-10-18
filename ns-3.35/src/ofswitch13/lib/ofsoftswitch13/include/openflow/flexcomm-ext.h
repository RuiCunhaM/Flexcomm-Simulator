/*
 * Complies with draft-cx-green-metrics-01
 */
#ifndef OPENFLOW_FLEXCOMM_EXT_H
#define OPENFLOW_FLEXCOMM_EXT_H 1

#include "openflow/openflow.h"

#define FLEXCOMM_VENDOR_ID 0x0000f82a

enum ofp_flexcomm_subtype {
  /* Global current consumption and power drawn since start
     * The request body is empty
     * The reply body is struct ofp_flexcomm_global_energy_reply */
  FC_GLOBAL_ENERGY = 0,

  /* Port current consumption and power drawn since start
     * The request body is struct ofp_flexcomm_port_energy_request
     * The reply body is an array of struct ofp_flexcomm_port_energy_reply */
  FC_PORT_ENERGY = 1,
};

/* Body for ofp_multipart_request of type OFPMP_EXPERIMENTER,
 * experimenter FLEXCOMM_VENDOR_ID and subtype FC_PORT_ENERGY */
struct ofp_flexcomm_port_energy_request
{
  uint32_t experimenter; /* Experimenter ID which takes the same form
                            as in struct ofp_experimenter_header. */
  uint32_t exp_type; /* Experimenter defined. */
  uint32_t port_no; /* Either for a single port (specified in port_no)
                       * or for all ports (if port_no == OFPP_ANY). */
  uint8_t pad[4];
};
OFP_ASSERT (sizeof (struct ofp_flexcomm_port_energy_request) == 16);

/* Body for ofp_multipart_reply of type OFPMP_EXPERIMENTER,
 * experimenter FLEXCOMM_VENDOR_ID and subtype FC_GLOBAL_ENERGY */
struct ofp_flexcomm_global_energy_reply
{
  uint32_t experimenter; /* Experimenter ID which takes the same form
                            as in struct ofp_experimenter_header. */
  uint32_t exp_type; /* Experimenter defined. */
  uint64_t current_consumption; /* current power consumption (e.g. Watts) */
  uint64_t power_drawn; /* Power drawn since system for the last
                                   * minute (e.g. Watt hours) */
};
OFP_ASSERT (sizeof (struct ofp_flexcomm_global_energy_reply) == 24);

/* Body for ofp_multipart_reply of type OFPMP_EXPERIMENTER,
 * experimenter FLEXCOMM_VENDOR_ID and subtype FC_PORT_ENERGY */
struct ofp_flexcomm_port_energy_reply
{
  uint32_t experimenter; /* Experimenter ID which takes the same form
                            as in struct ofp_experimenter_header. */
  uint32_t exp_type; /* Experimenter defined. */
  uint8_t stats[0]; /* Port stats*/
};
OFP_ASSERT (sizeof (struct ofp_flexcomm_port_energy_reply) == 8);

struct ofp_flexcomm_port
{
  uint32_t port_no;

  uint8_t pad[4];

  uint64_t current_consumption; /* current power consumption (e.g. Watts) */
  uint64_t power_drawn; /* Power drawn since system for the last
                                 * minute (e.g. Watt hours) */
};
OFP_ASSERT (sizeof (struct ofp_flexcomm_port) == 24);

#endif
