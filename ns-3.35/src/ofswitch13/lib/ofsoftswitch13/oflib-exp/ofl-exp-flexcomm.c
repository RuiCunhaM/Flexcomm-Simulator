#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <netinet/in.h>

#include "oflib/ofl-messages.h"
#include "oflib/ofl.h"
#include "openflow/openflow.h"
#include "openflow/flexcomm-ext.h"
#include "ofl-exp-flexcomm.h"
#include "../oflib/ofl-print.h"
#include "../oflib/ofl-log.h"
#include "../oflib/ofl-utils.h"

#define LOG_MODULE ofl_exp_flexcomm
OFL_LOG_INIT (LOG_MODULE)

static void
ofl_exp_flexcomm_stats_req_pack_empty (uint8_t **buf, size_t *buf_len)
{
  *buf_len =
      sizeof (struct ofp_multipart_request) + sizeof (struct ofp_experimenter_multipart_header);
  *buf = (uint8_t *) calloc (1, *buf_len);
}

static void
ofl_exp_flexcomm_stats_req_pack_port (struct ofl_msg_multipart_request_flexcomm_port *preq,
                                      uint8_t **buf, size_t *buf_len)
{
  struct ofp_multipart_request *mreq;
  struct ofp_flexcomm_port_energy_request *stats;

  *buf_len =
      sizeof (struct ofp_multipart_request) + sizeof (struct ofp_flexcomm_port_energy_request);
  *buf = (uint8_t *) calloc (1, *buf_len);
  mreq = (struct ofp_multipart_request *) (*buf);
  stats = (struct ofp_flexcomm_port_energy_request *) mreq->body;
  stats->port_no = htonl (preq->port_no);
  memset (stats->pad, 0x00, 4);
}

int
ofl_exp_flexcomm_stats_req_pack (struct ofl_msg_multipart_request_experimenter *msg, uint8_t **buf,
                                 size_t *buf_len)
{

  if (msg->experimenter_id == FLEXCOMM_VENDOR_ID)
    {
      struct ofp_experimenter_multipart_header *exp_header;
      struct ofl_msg_multipart_request_flexcomm *req =
          (struct ofl_msg_multipart_request_flexcomm *) msg;
      switch (req->subtype)
        {
          case FC_GLOBAL_ENERGY: {
            ofl_exp_flexcomm_stats_req_pack_empty (buf, buf_len);
            break;
          }
          case FC_PORT_ENERGY: {
            ofl_exp_flexcomm_stats_req_pack_port (
                (struct ofl_msg_multipart_request_flexcomm_port *) req, buf, buf_len);
            break;
          }
          default: {
            OFL_LOG_WARN (LOG_MODULE, "Trying to pack unknown FlexComm stat type.");
            return ofl_error (OFPET_BAD_REQUEST, OFPBRC_BAD_EXP_TYPE);
          }
        }

      exp_header =
          (struct ofp_experimenter_multipart_header *) ((struct ofp_multipart_request *) (*buf))
              ->body;
      exp_header->experimenter = htonl (FLEXCOMM_VENDOR_ID);
      exp_header->exp_type = htonl (req->subtype);

      return 0;
    }
  else
    {
      OFL_LOG_WARN (LOG_MODULE, "Trying to pack non-FlexComm Multipart Experimenter request.");
      return ofl_error (OFPET_BAD_REQUEST, OFPBRC_BAD_EXPERIMENTER);
    }
}

static ofl_err
ofl_exp_flexcomm_stats_req_unpack_empty (struct ofl_msg_multipart_request_header **msg)
{
  *msg = (struct ofl_msg_multipart_request_header *) malloc (
      sizeof (struct ofl_msg_multipart_request_flexcomm));
  return 0;
}

static ofl_err
ofl_exp_flexcomm_stats_req_unpack_port (struct ofp_experimenter_multipart_header *exp, size_t *len,
                                        struct ofl_msg_multipart_request_header **msg)
{
  struct ofp_flexcomm_port_energy_request *sm;
  struct ofl_msg_multipart_request_flexcomm_port *dm;

  if (*len < sizeof (struct ofp_flexcomm_port_energy_request))
    {
      OFL_LOG_WARN (LOG_MODULE, "Received FlexComm port stats request has invalid length (%zu).",
                    *len);
      return ofl_error (OFPET_BAD_REQUEST, OFPBRC_BAD_LEN);
    }

  sm = (struct ofp_flexcomm_port_energy_request *) exp;

  if (ntohl (sm->port_no) == 0 ||
      (ntohl (sm->port_no) > OFPP_MAX && ntohl (sm->port_no) != OFPP_ANY))
    {
      OFL_LOG_WARN (LOG_MODULE, "Received FlexComm port stats request has invalid port (%u).",
                    ntohl (sm->port_no));
      return ofl_error (OFPET_BAD_REQUEST, OFPBRC_BAD_PORT);
    }

  *len -= sizeof (struct ofp_flexcomm_port_energy_request);

  dm = (struct ofl_msg_multipart_request_flexcomm_port *) malloc (
      sizeof (struct ofl_msg_multipart_request_flexcomm_port));
  dm->port_no = ntohl (sm->port_no);
  *msg = (struct ofl_msg_multipart_request_header *) dm;
  return 0;
}

ofl_err
ofl_exp_flexcomm_stats_req_unpack (struct ofp_experimenter_multipart_header *exp, size_t *len,
                                   struct ofl_msg_multipart_request_header **msg)
{
  if (ntohl (exp->experimenter) == FLEXCOMM_VENDOR_ID)
    {
      struct ofl_msg_multipart_request_flexcomm *ofl_flex;
      int error;
      switch (ntohl (exp->exp_type))
        {
          case FC_GLOBAL_ENERGY: {
            error = ofl_exp_flexcomm_stats_req_unpack_empty (msg);
            break;
          }
          case FC_PORT_ENERGY: {
            error = ofl_exp_flexcomm_stats_req_unpack_port (exp, len, msg);
            break;
          }
          default: {
            OFL_LOG_WARN (LOG_MODULE, "Trying to unpack unknown FlexComm stat type.");
            error = ofl_error (OFPET_BAD_REQUEST, OFPBRC_BAD_EXP_TYPE);
          }
        }
      if (error)
        {
          return error;
        }
      ofl_flex = (struct ofl_msg_multipart_request_flexcomm *) (*msg);
      ofl_flex->subtype = ntohl (exp->exp_type);
      return 0;
    }
  else
    {
      OFL_LOG_WARN (LOG_MODULE, "Trying to unpack non-FlexComm Multipart Experimenter request.");
      return ofl_error (OFPET_BAD_REQUEST, OFPBRC_BAD_EXPERIMENTER);
    }
}

int
ofl_exp_flexcomm_stats_req_free (struct ofl_msg_multipart_request_experimenter *msg)
{
  if (msg->experimenter_id == FLEXCOMM_VENDOR_ID)
    {
      struct ofl_msg_multipart_request_flexcomm *exp =
          (struct ofl_msg_multipart_request_flexcomm *) msg;
      switch (exp->subtype)
        {
        case FC_GLOBAL_ENERGY:
          case FC_PORT_ENERGY: {
            break;
          }
          default: {
            OFL_LOG_WARN (LOG_MODULE, "Trying to free unknown FlexComm stat type.");
          }
        }
    }
  else
    {
      OFL_LOG_WARN (LOG_MODULE, "Trying to free non-FlexComm Multipart Experimenter request.");
    }
  return 0;
}

char *
ofl_exp_flexcomm_stats_req_to_string (struct ofl_msg_multipart_request_experimenter *msg)
{
  char *str;
  size_t str_size;
  FILE *stream = open_memstream (&str, &str_size);

  if (msg->experimenter_id == FLEXCOMM_VENDOR_ID)
    {
      struct ofl_msg_multipart_request_flexcomm *exp =
          (struct ofl_msg_multipart_request_flexcomm *) msg;
      switch (exp->subtype)
        {
          case FC_GLOBAL_ENERGY: {
            fprintf (stream, "{type=\"fc_global_energy\"}");
            break;
          }
          case FC_PORT_ENERGY: {
            struct ofl_msg_multipart_request_flexcomm_port *p =
                (struct ofl_msg_multipart_request_flexcomm_port *) exp;
            fprintf (stream, "{type=\"fc_port_energy\", port=\"");
            ofl_port_print (stream, p->port_no);
            fprintf (stream, "\"}");
            break;
          }
          default: {
            OFL_LOG_WARN (LOG_MODULE, "Trying to print unknown FlexComm stat type.");
            fprintf (stream, "ofexp{type=\"%u\"}", exp->subtype);
          }
        }
    }
  else
    {
      OFL_LOG_WARN (LOG_MODULE, "Trying to print non-FlexComm Multipart Experimenter request.");
      fprintf (stream, "exp{exp_id=\"%u\"}", msg->experimenter_id);
    }
  fclose (stream);
  return str;
}

static void
ofl_exp_flexcomm_stats_reply_pack_global (struct ofl_msg_multipart_reply_flexcomm_global *grep,
                                          uint8_t **buf, size_t *buf_len)
{
  struct ofp_multipart_reply *mrep;
  struct ofp_flexcomm_global_energy_reply *stats;

  *buf_len = sizeof (struct ofp_multipart_reply) + sizeof (struct ofp_flexcomm_global_energy_reply);
  *buf = (uint8_t *) calloc (1, *buf_len);

  mrep = (struct ofp_multipart_reply *) (*buf);
  stats = (struct ofp_flexcomm_global_energy_reply *) mrep->body;
  stats->current_consumption = hton64 (grep->current_consumption);
  stats->power_drawn = hton64 (grep->power_drawn);
}

static void
ofl_exp_flexcomm_stats_reply_pack_port (struct ofl_msg_multipart_reply_flexcomm_port *prep,
                                        uint8_t **buf, size_t *buf_len)
{
  struct ofp_multipart_reply *mrep;
  struct ofp_flexcomm_port_energy_reply *pstats;
  uint8_t *stats;
  size_t i;

  *buf_len = sizeof (struct ofp_multipart_reply) + sizeof (struct ofp_flexcomm_port_energy_reply) +
             prep->stats_num * sizeof (struct ofp_flexcomm_port);
  *buf = (uint8_t *) calloc (1, *buf_len);
  mrep = (struct ofp_multipart_reply *) (*buf);
  pstats = (struct ofp_flexcomm_port_energy_reply *) mrep->body;

  stats = (uint8_t *) pstats->stats;
  for (i = 0; i < prep->stats_num; i++)
    {
      struct ofp_flexcomm_port *pstat = (struct ofp_flexcomm_port *) stats;
      pstat->port_no = htonl (prep->stats[i]->port_no);
      pstat->current_consumption = hton64 (prep->stats[i]->current_consumption);
      pstat->power_drawn = hton64 (prep->stats[i]->power_drawn);
      stats += sizeof (struct ofp_flexcomm_port);
    }
}

int
ofl_exp_flexcomm_stats_reply_pack (struct ofl_msg_multipart_reply_experimenter *msg, uint8_t **buf,
                                   size_t *buf_len)
{

  if (msg->experimenter_id == FLEXCOMM_VENDOR_ID)
    {
      struct ofp_experimenter_multipart_header *exp_header;
      struct ofl_msg_multipart_reply_flexcomm *rep =
          (struct ofl_msg_multipart_reply_flexcomm *) msg;

      switch (rep->subtype)
        {
          case FC_GLOBAL_ENERGY: {
            ofl_exp_flexcomm_stats_reply_pack_global (
                (struct ofl_msg_multipart_reply_flexcomm_global *) rep, buf, buf_len);
            break;
          }
          case FC_PORT_ENERGY: {
            ofl_exp_flexcomm_stats_reply_pack_port (
                (struct ofl_msg_multipart_reply_flexcomm_port *) rep, buf, buf_len);
            break;
          }
          default: {
            OFL_LOG_WARN (LOG_MODULE, "Trying to pack unknown FlexComm stat type.");
            return -1;
          }
        }
      exp_header =
          (struct ofp_experimenter_multipart_header *) ((struct ofp_multipart_request *) (*buf))
              ->body;
      exp_header->experimenter = htonl (FLEXCOMM_VENDOR_ID);
      exp_header->exp_type = htonl (rep->subtype);

      return 0;
    }
  else
    {
      OFL_LOG_WARN (LOG_MODULE, "Trying to pack non-FlexComm Multipart Experimenter reply.");
      return -1;
    }
}

static ofl_err
ofl_exp_flexcomm_stats_reply_unpack_global (struct ofp_experimenter_multipart_header *exp,
                                            size_t *len,
                                            struct ofl_msg_multipart_reply_header **msg)
{
  struct ofp_flexcomm_global_energy_reply *sm;
  struct ofl_msg_multipart_reply_flexcomm_global *dm;

  if (*len < sizeof (struct ofp_flexcomm_global_energy_reply))
    {
      OFL_LOG_WARN (LOG_MODULE, "Received FlexComm global energy reply has invalid length (%zu).",
                    *len);
      return ofl_error (OFPET_BAD_REQUEST, OFPBRC_BAD_LEN);
    }
  *len -= sizeof (struct ofp_flexcomm_global_energy_reply);

  sm = (struct ofp_flexcomm_global_energy_reply *) exp;
  dm = (struct ofl_msg_multipart_reply_flexcomm_global *) malloc (
      sizeof (struct ofl_msg_multipart_reply_flexcomm_global));

  dm->current_consumption = ntoh64 (sm->current_consumption);
  dm->power_drawn = ntoh64 (sm->power_drawn);

  *msg = (struct ofl_msg_multipart_reply_header *) dm;
  return 0;
}

static ofl_err
ofl_exp_flexcomm_stats_reply_unpack_port (struct ofp_experimenter_multipart_header *exp,
                                          size_t *len, struct ofl_msg_multipart_reply_header **msg)
{
  struct ofp_flexcomm_port_energy_reply *sm;
  struct ofl_msg_multipart_reply_flexcomm_port *dm;
  struct ofp_flexcomm_port *stat;

  size_t i;

  if (*len < sizeof (struct ofp_flexcomm_port_energy_reply))
    {
      OFL_LOG_WARN (LOG_MODULE, "Received FlexComm port stats reply has invalid length (%zu).",
                    *len);
      return ofl_error (OFPET_BAD_REQUEST, OFPBRC_BAD_LEN);
    }
  *len -= sizeof (struct ofp_flexcomm_port_energy_reply);

  sm = (struct ofp_flexcomm_port_energy_reply *) exp;
  stat = (struct ofp_flexcomm_port *) sm->stats;
  dm = (struct ofl_msg_multipart_reply_flexcomm_port *) malloc (
      sizeof (struct ofl_msg_multipart_reply_flexcomm_port));

  dm->stats_num = *len / sizeof (struct ofp_flexcomm_port);
  dm->stats = (struct ofl_flexcomm_port_stats **) malloc (
      dm->stats_num * sizeof (struct ofl_flexcomm_port_stats *));

  for (i = 0; i < dm->stats_num; i++)
    {
      if (*len < sizeof (struct ofp_flexcomm_port))
        {
          OFL_LOG_WARN (LOG_MODULE, "Received FlexComm port stats has invalid length (%zu).", *len);
          OFL_UTILS_FREE_ARR (dm->stats, i);
          free (dm);
          return ofl_error (OFPET_BAD_REQUEST, OFPBRC_BAD_LEN);
        }

      if (ntohl (stat->port_no) == 0 ||
          (ntohl (stat->port_no) > OFPP_MAX && ntohl (stat->port_no) != OFPP_LOCAL))
        {
          OFL_LOG_WARN (LOG_MODULE, "Received FlexComm port stats has invalid port (%u).",
                        ntohl (stat->port_no));
          OFL_UTILS_FREE_ARR (dm->stats, i);
          free (dm);
          return ofl_error (OFPET_BAD_REQUEST, OFPBRC_BAD_PORT);
        }
      *len -= sizeof (struct ofp_flexcomm_port);

      dm->stats[i] =
          (struct ofl_flexcomm_port_stats *) malloc (sizeof (struct ofl_flexcomm_port_stats));
      dm->stats[i]->port_no = ntohl (stat->port_no);
      dm->stats[i]->current_consumption = ntoh64 (stat->current_consumption);
      dm->stats[i]->power_drawn = ntoh64 (stat->power_drawn);

      stat = (struct ofp_flexcomm_port *) ((uint8_t *) stat + sizeof (struct ofp_flexcomm_port));
    }

  *msg = (struct ofl_msg_multipart_reply_header *) dm;
  return 0;
}

ofl_err
ofl_exp_flexcomm_stats_reply_unpack (struct ofp_experimenter_multipart_header *exp, size_t *len,
                                     struct ofl_msg_multipart_reply_header **msg)
{
  if (ntohl (exp->experimenter) == FLEXCOMM_VENDOR_ID)
    {
      struct ofl_msg_multipart_reply_flexcomm *ofl_flex;
      int error;
      switch (ntohl (exp->exp_type))
        {
          case FC_GLOBAL_ENERGY: {
            error = ofl_exp_flexcomm_stats_reply_unpack_global (exp, len, msg);
            break;
          }
          case FC_PORT_ENERGY: {
            error = ofl_exp_flexcomm_stats_reply_unpack_port (exp, len, msg);
            break;
          }
          default: {
            OFL_LOG_WARN (LOG_MODULE, "Trying to unpack unknown FlexComm stat type.");
            error = ofl_error (OFPET_BAD_REQUEST, OFPBRC_BAD_EXP_TYPE);
          }
        }
      if (error)
        {
          return error;
        }
      ofl_flex = (struct ofl_msg_multipart_reply_flexcomm *) (*msg);
      ofl_flex->subtype = ntohl (exp->exp_type);
      return 0;
    }
  else
    {
      OFL_LOG_WARN (LOG_MODULE, "Trying to unpack non-FlexComm Multipart Experimenter reply.");
      return ofl_error (OFPET_BAD_REQUEST, OFPBRC_BAD_EXPERIMENTER);
    }
}

int
ofl_exp_flexcomm_stats_reply_free (struct ofl_msg_multipart_reply_experimenter *msg)
{
  if (msg->experimenter_id == FLEXCOMM_VENDOR_ID)
    {
      struct ofl_msg_multipart_reply_flexcomm *exp =
          (struct ofl_msg_multipart_reply_flexcomm *) msg;
      switch (exp->subtype)
        {
          case FC_GLOBAL_ENERGY: {
            break;
          }
          case FC_PORT_ENERGY: {
            struct ofl_msg_multipart_reply_flexcomm_port *stat =
                (struct ofl_msg_multipart_reply_flexcomm_port *) exp;
            OFL_UTILS_FREE_ARR (stat->stats, stat->stats_num);
            break;
          }
          default: {
            OFL_LOG_WARN (LOG_MODULE, "Trying to free unknown FlexComm stat type.");
          }
        }
    }
  else
    {
      OFL_LOG_WARN (LOG_MODULE, "Trying to free non-FlexComm Multipart Experimenter reply.");
    }
  return 0;
}

char *
ofl_exp_flexcomm_stats_reply_to_string (struct ofl_msg_multipart_reply_experimenter *msg)
{
  char *str;
  size_t str_size;
  FILE *stream = open_memstream (&str, &str_size);

  if (msg->experimenter_id == FLEXCOMM_VENDOR_ID)
    {
      struct ofl_msg_multipart_reply_flexcomm *exp =
          (struct ofl_msg_multipart_reply_flexcomm *) msg;
      switch (exp->subtype)
        {
          case FC_GLOBAL_ENERGY: {
            struct ofl_msg_multipart_reply_flexcomm_global *global =
                (struct ofl_msg_multipart_reply_flexcomm_global *) exp;
            fprintf (stream,
                     "{type=\"fc_global_energy\", current_consumption=\"%" PRIu64
                     "\", power_drawn=\"%" PRIu64 "\"}",
                     global->current_consumption, global->power_drawn);
            break;
          }
          case FC_PORT_ENERGY: {
            struct ofl_msg_multipart_reply_flexcomm_port *port =
                (struct ofl_msg_multipart_reply_flexcomm_port *) exp;
            size_t i;
            fprintf (stream, "{type=\"fc_port_energy\", stats=[");
            for (i = 0; i < port->stats_num; i++)
              {
                fprintf (stream, "{port=\"");
                ofl_port_print (stream, port->stats[i]->port_no);
                fprintf (stream,
                         ", current_consumption=\"%" PRIu64 "\", power_drawn=\"%" PRIu64 "\"}",
                         port->stats[i]->current_consumption, port->stats[i]->power_drawn);

                if (i < port->stats_num - 1)
                  {
                    fprintf (stream, ",\n");
                  };
              }
            fprintf (stream, "]");
            break;
          }
          default: {
            OFL_LOG_WARN (LOG_MODULE, "Trying to print unknown FlexComm stat type.");
            fprintf (stream, "ofexp{type=\"%u\"}", exp->subtype);
          }
        }
    }
  else
    {
      OFL_LOG_WARN (LOG_MODULE, "Trying to print non-FlexComm Multipart Experimenter reply.");
      fprintf (stream, "exp{exp_id=\"%u\"}", msg->experimenter_id);
    }
  fclose (stream);
  return str;
}
