/* Copyright (c) 2011, TrafficLab, Ericsson Research, Hungary
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *   * Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the name of the Ericsson Research nor the names of its
 *     contributors may be used to endorse or promote products derived from
 *     this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 *
 * Author: Zoltán Lajos Kis <zoltan.lajos.kis@ericsson.com>
 */

#include <inttypes.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <stdio.h>
#include "ofl-exp.h"
#include "ofl-exp-nicira.h"
#include "ofl-exp-flexcomm.h"
#include "ofl-exp-openflow.h"
#include "../oflib/ofl-messages.h"
#include "../oflib/ofl-log.h"
#include "openflow/openflow.h"
#include "openflow/nicira-ext.h"
#include "openflow/flexcomm-ext.h"
#include "openflow/openflow-ext.h"

#define LOG_MODULE ofl_exp
OFL_LOG_INIT (LOG_MODULE)

int
ofl_exp_msg_pack (struct ofl_msg_experimenter *msg, uint8_t **buf, size_t *buf_len)
{
  switch (msg->experimenter_id)
    {
      case (OPENFLOW_VENDOR_ID): {
        return ofl_exp_openflow_msg_pack (msg, buf, buf_len);
      }
      case (NX_VENDOR_ID): {
        return ofl_exp_nicira_msg_pack (msg, buf, buf_len);
      }
      default: {
        OFL_LOG_WARN (LOG_MODULE, "Trying to pack unknown EXPERIMENTER message (%u).",
                      msg->experimenter_id);
        return ofl_error (OFPET_BAD_REQUEST, OFPBRC_BAD_EXPERIMENTER);
      }
    }
}

ofl_err
ofl_exp_msg_unpack (struct ofp_header *oh, size_t *len, struct ofl_msg_experimenter **msg)
{
  struct ofp_experimenter_header *exp;

  if (*len < sizeof (struct ofp_experimenter_header))
    {
      OFL_LOG_WARN (LOG_MODULE,
                    "Received EXPERIMENTER message is shorter than ofp_experimenter_header.");
      return ofl_error (OFPET_BAD_REQUEST, OFPBRC_BAD_LEN);
    }

  exp = (struct ofp_experimenter_header *) oh;

  switch (htonl (exp->experimenter))
    {
      case (OPENFLOW_VENDOR_ID): {
        return ofl_exp_openflow_msg_unpack (oh, len, msg);
      }
      case (NX_VENDOR_ID): {
        return ofl_exp_nicira_msg_unpack (oh, len, msg);
      }
      default: {
        OFL_LOG_WARN (LOG_MODULE, "Trying to unpack unknown EXPERIMENTER message (%u).",
                      htonl (exp->experimenter));
        return ofl_error (OFPET_BAD_REQUEST, OFPBRC_BAD_EXPERIMENTER);
      }
    }
}

int
ofl_exp_msg_free (struct ofl_msg_experimenter *msg)
{
  switch (msg->experimenter_id)
    {
      case (OPENFLOW_VENDOR_ID): {
        return ofl_exp_openflow_msg_free (msg);
      }
      case (NX_VENDOR_ID): {
        return ofl_exp_nicira_msg_free (msg);
      }
      default: {
        OFL_LOG_WARN (LOG_MODULE, "Trying to free unknown EXPERIMENTER message (%u).",
                      msg->experimenter_id);
        free (msg);
        return ofl_error (OFPET_BAD_REQUEST, OFPBRC_BAD_EXPERIMENTER);
      }
    }
}

char *
ofl_exp_msg_to_string (struct ofl_msg_experimenter *msg)
{
  switch (msg->experimenter_id)
    {
      case (OPENFLOW_VENDOR_ID): {
        return ofl_exp_openflow_msg_to_string (msg);
      }
      case (NX_VENDOR_ID): {
        return ofl_exp_nicira_msg_to_string (msg);
      }
      default: {
        char *str;
        size_t str_size;
        FILE *stream = open_memstream (&str, &str_size);
        OFL_LOG_WARN (LOG_MODULE, "Trying to convert to string unknown EXPERIMENTER message (%u).",
                      msg->experimenter_id);
        fprintf (stream, "exp{id=\"0x%" PRIx32 "\"}", msg->experimenter_id);
        fclose (stream);
        return str;
      }
    }
}

int
ofl_exp_stats_req_pack (struct ofl_msg_multipart_request_header *msg, uint8_t **buf,
                        size_t *buf_len)

{
  struct ofl_msg_multipart_request_experimenter *multipart_exp =
      (struct ofl_msg_multipart_request_experimenter *) msg;
  switch (multipart_exp->experimenter_id)
    {
      case (FLEXCOMM_VENDOR_ID): {
        return ofl_exp_flexcomm_stats_req_pack (multipart_exp, buf, buf_len);
      }
      default: {
        OFL_LOG_WARN (LOG_MODULE, "Trying to pack unknown MULTIPART EXPERIMENTER request (%u).",
                      multipart_exp->experimenter_id);
        return ofl_error (OFPET_BAD_REQUEST, OFPBRC_BAD_EXPERIMENTER);
      }
    }
}

ofl_err
ofl_exp_stats_req_unpack (struct ofp_multipart_request *os, size_t *len,
                          struct ofl_msg_multipart_request_header **msg)
{
  struct ofl_msg_multipart_request_experimenter *ofl_exp;
  struct ofp_experimenter_multipart_header *exp;
  int error;

  if (*len < sizeof (struct ofp_experimenter_multipart_header))
    {
      OFL_LOG_WARN (LOG_MODULE, "Received MULTIPART EXPERIMENTER request is shorter than "
                                "ofp_experimenter_multipart_header.");
      return ofl_error (OFPET_BAD_REQUEST, OFPBRC_BAD_LEN);
    }

  exp = (struct ofp_experimenter_multipart_header *) os->body;

  switch (ntohl (exp->experimenter))
    {
      case (FLEXCOMM_VENDOR_ID): {
        error = ofl_exp_flexcomm_stats_req_unpack (exp, len, msg);
        break;
      }
      default: {
        OFL_LOG_WARN (LOG_MODULE, "Trying to unpack unknown MULTIPART EXPERIMENTER request (%u).",
                      htonl (exp->experimenter));
        error = ofl_error (OFPET_BAD_REQUEST, OFPBRC_BAD_EXPERIMENTER);
      }
    }

  if (error)
    {
      return error;
    }

  ofl_exp = (struct ofl_msg_multipart_request_experimenter *) (*msg);
  ofl_exp->experimenter_id = ntohl (exp->experimenter);
  return 0;
}

int
ofl_exp_stats_req_free (struct ofl_msg_multipart_request_header *msg)
{
  struct ofl_msg_multipart_request_experimenter *multipart_exp =
      (struct ofl_msg_multipart_request_experimenter *) msg;
  int error;
  switch (multipart_exp->experimenter_id)
    {
      case (FLEXCOMM_VENDOR_ID): {
        error = ofl_exp_flexcomm_stats_req_free (multipart_exp);
        break;
      }
      default: {
        OFL_LOG_WARN (LOG_MODULE, "Trying to free unknown MULTIPART EXPERIMENTER request (%u).",
                      multipart_exp->experimenter_id);
        error = ofl_error (OFPET_BAD_REQUEST, OFPBRC_BAD_EXPERIMENTER);
      }
    }
  free (msg);
  return error;
}

char *
ofl_exp_stats_req_to_string (struct ofl_msg_multipart_request_header *msg)
{
  struct ofl_msg_multipart_request_experimenter *multipart_exp =
      (struct ofl_msg_multipart_request_experimenter *) msg;
  switch (multipart_exp->experimenter_id)
    {
      case (FLEXCOMM_VENDOR_ID): {
        return ofl_exp_flexcomm_stats_req_to_string (multipart_exp);
      }
      default: {
        char *str;
        size_t str_size;
        FILE *stream = open_memstream (&str, &str_size);
        OFL_LOG_WARN (LOG_MODULE,
                      "Trying to convert to string unknown MULTIPART EXPERIMENTER request (%u).",
                      multipart_exp->experimenter_id);
        fprintf (stream, "exp{id=\"0x%" PRIx32 "\"}", multipart_exp->experimenter_id);
        fclose (stream);
        return str;
      }
    }
}

int
ofl_exp_stats_reply_pack (struct ofl_msg_multipart_reply_header *msg, uint8_t **buf,
                          size_t *buf_len)
{
  struct ofl_msg_multipart_reply_experimenter *multipart_exp =
      (struct ofl_msg_multipart_reply_experimenter *) msg;
  switch (multipart_exp->experimenter_id)
    {
      case (FLEXCOMM_VENDOR_ID): {
        return ofl_exp_flexcomm_stats_reply_pack (multipart_exp, buf, buf_len);
      }
      default: {
        OFL_LOG_WARN (LOG_MODULE, "Trying to pack unknown MULTIPART EXPERIMENTER reply (%u).",
                      multipart_exp->experimenter_id);
        return ofl_error (OFPET_BAD_REQUEST, OFPBRC_BAD_EXPERIMENTER);
      }
    }
}

ofl_err
ofl_exp_stats_reply_unpack (struct ofp_multipart_reply *os, size_t *len,
                            struct ofl_msg_multipart_reply_header **msg)
{
  struct ofl_msg_multipart_reply_experimenter *ofl_exp;
  struct ofp_experimenter_multipart_header *exp;
  int error;

  if (*len < sizeof (struct ofp_experimenter_multipart_header))
    {
      OFL_LOG_WARN (LOG_MODULE, "Received MULTIPART EXPERIMENTER reply is shorter than "
                                "ofp_experimenter_multipart_header.");
      return ofl_error (OFPET_BAD_REQUEST, OFPBRC_BAD_LEN);
    }

  exp = (struct ofp_experimenter_multipart_header *) os->body;

  switch (ntohl (exp->experimenter))
    {
      case (FLEXCOMM_VENDOR_ID): {
        error = ofl_exp_flexcomm_stats_reply_unpack (exp, len, msg);
        break;
      }
      default: {
        OFL_LOG_WARN (LOG_MODULE, "Trying to unpack unknown MULTIPART EXPERIMENTER reply (%u).",
                      htonl (exp->experimenter));
        error = ofl_error (OFPET_BAD_REQUEST, OFPBRC_BAD_EXPERIMENTER);
      }
    }

  if (error)
    {
      return error;
    }

  ofl_exp = (struct ofl_msg_multipart_reply_experimenter *) (*msg);
  ofl_exp->experimenter_id = ntohl (exp->experimenter);
  return 0;
}

int
ofl_exp_stats_reply_free (struct ofl_msg_multipart_reply_header *msg)
{
  struct ofl_msg_multipart_reply_experimenter *multipart_exp =
      (struct ofl_msg_multipart_reply_experimenter *) msg;
  int error;
  switch (multipart_exp->experimenter_id)
    {
      case (FLEXCOMM_VENDOR_ID): {
        error = ofl_exp_flexcomm_stats_reply_free (multipart_exp);
        break;
      }
      default: {
        OFL_LOG_WARN (LOG_MODULE, "Trying to free unknown MULTIPART EXPERIMENTER reply (%u).",
                      multipart_exp->experimenter_id);
        error = ofl_error (OFPET_BAD_REQUEST, OFPBRC_BAD_EXPERIMENTER);
      }
    }
  free (msg);
  return error;
}

char *
ofl_exp_stats_reply_to_string (struct ofl_msg_multipart_reply_header *msg)
{
  struct ofl_msg_multipart_reply_experimenter *multipart_exp =
      (struct ofl_msg_multipart_reply_experimenter *) msg;
  switch (multipart_exp->experimenter_id)
    {
      case (FLEXCOMM_VENDOR_ID): {
        return ofl_exp_flexcomm_stats_reply_to_string (multipart_exp);
      }
      default: {
        char *str;
        size_t str_size;
        FILE *stream = open_memstream (&str, &str_size);
        OFL_LOG_WARN (LOG_MODULE,
                      "Trying to convert to string unknown MULTIPART EXPERIMENTER reply (%u).",
                      multipart_exp->experimenter_id);
        fprintf (stream, "exp{id=\"0x%" PRIx32 "\"}", multipart_exp->experimenter_id);
        fclose (stream);
        return str;
      }
    }
}
