#include "openflow/openflow.h"
#ifndef OFL_EXP_FLEXCOMM_H
#define OFL_EXP_FLEXCOMM_H 1

#include "../oflib/ofl-structs.h"
#include "../oflib/ofl-messages.h"

struct ofl_msg_multipart_request_flexcomm
{
  struct ofl_msg_multipart_request_experimenter header;

  uint32_t subtype;
};

struct ofl_msg_multipart_request_flexcomm_port
{
  struct ofl_msg_multipart_request_flexcomm header;

  uint32_t port_no; /* Either for a single port (specified in port_no)
                       * or for all ports (if port_no == OFPP_ANY). */
};

struct ofl_msg_multipart_reply_flexcomm
{
  struct ofl_msg_multipart_reply_experimenter header;

  uint32_t subtype;
};

struct ofl_msg_multipart_reply_flexcomm_global
{
  struct ofl_msg_multipart_reply_flexcomm header;

  uint64_t current_consumption;
  uint64_t power_drawn;
};

struct ofl_flexcomm_port_stats
{
  uint32_t port_no;

  uint64_t current_consumption;
  uint64_t power_drawn;
};

struct ofl_msg_multipart_reply_flexcomm_port
{
  struct ofl_msg_multipart_reply_flexcomm header;

  size_t stats_num;
  struct ofl_flexcomm_port_stats **stats;
};

int ofl_exp_flexcomm_stats_req_pack (struct ofl_msg_multipart_request_experimenter *msg,
                                     uint8_t **buf, size_t *buf_len);

ofl_err ofl_exp_flexcomm_stats_req_unpack (struct ofp_experimenter_multipart_header *exp,
                                           size_t *len,
                                           struct ofl_msg_multipart_request_header **msg);

int ofl_exp_flexcomm_stats_req_free (struct ofl_msg_multipart_request_experimenter *msg);

char *ofl_exp_flexcomm_stats_req_to_string (struct ofl_msg_multipart_request_experimenter *msg);

int ofl_exp_flexcomm_stats_reply_pack (struct ofl_msg_multipart_reply_experimenter *msg,
                                       uint8_t **buf, size_t *buf_len);

ofl_err ofl_exp_flexcomm_stats_reply_unpack (struct ofp_experimenter_multipart_header *exp,
                                             size_t *len,
                                             struct ofl_msg_multipart_reply_header **msg);

int ofl_exp_flexcomm_stats_reply_free (struct ofl_msg_multipart_reply_experimenter *msg);

char *ofl_exp_flexcomm_stats_reply_to_string (struct ofl_msg_multipart_reply_experimenter *msg);

#endif
