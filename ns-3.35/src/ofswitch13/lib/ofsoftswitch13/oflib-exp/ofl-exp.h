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

#ifndef OFL_EXP_H
#define OFL_EXP_H 1

#include "../oflib/ofl-messages.h"
#include "openflow/openflow.h"

int ofl_exp_msg_pack (struct ofl_msg_experimenter *msg, uint8_t **buf, size_t *buf_len);

ofl_err ofl_exp_msg_unpack (struct ofp_header *oh, size_t *len, struct ofl_msg_experimenter **msg);

int ofl_exp_msg_free (struct ofl_msg_experimenter *msg);

char *ofl_exp_msg_to_string (struct ofl_msg_experimenter *msg);

int ofl_exp_stats_req_pack (struct ofl_msg_multipart_request_header *msg, uint8_t **buf,
                            size_t *buf_len);

ofl_err ofl_exp_stats_req_unpack (struct ofp_multipart_request *os, size_t *len,
                                  struct ofl_msg_multipart_request_header **msg);

int ofl_exp_stats_req_free (struct ofl_msg_multipart_request_header *msg);

char *ofl_exp_stats_req_to_string (struct ofl_msg_multipart_request_header *msg);

int ofl_exp_stats_reply_pack (struct ofl_msg_multipart_reply_header *msg, uint8_t **buf,
                              size_t *buf_len);

ofl_err ofl_exp_stats_reply_unpack (struct ofp_multipart_reply *os, size_t *len,
                                    struct ofl_msg_multipart_reply_header **msg);

int ofl_exp_stats_reply_free (struct ofl_msg_multipart_reply_header *msg);

char *ofl_exp_stats_reply_to_string (struct ofl_msg_multipart_reply_header *msg);

#endif /* OFL_EXP_H */
