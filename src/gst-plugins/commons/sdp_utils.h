/*
 * (C) Copyright 2013 Kurento (http://kurento.org/)
 *
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the GNU Lesser General Public License
 * (LGPL) version 2.1 which accompanies this distribution, and is available at
 * http://www.gnu.org/licenses/lgpl-2.1.html
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 */

#ifndef __SDP_H__
#define __SDP_H__

#include <gst/sdp/gstsdpmessage.h>

#define RTCP_FB "rtcp-fb"
#define RTCP_FB_FIR "ccm fir"
#define RTCP_FB_NACK "nack"
#define RTCP_FB_PLI "nack pli"
#define RTCP_FB_REMB "goog-remb"

#define EXT_MAP "extmap"

typedef enum GstSDPDirection
{
  SENDONLY,
  RECVONLY,
  SENDRECV,
  INACTIVE
} GstSDPDirection;

typedef gboolean (*GstSDPMediaFunc) (const GstSDPMedia *media, gpointer user_data);
typedef gboolean (*GstSDPIntersectMediaFunc) (const GstSDPAttribute *attr, gpointer user_data);

gboolean sdp_utils_is_attribute_in_media (const GstSDPMedia * media, const GstSDPAttribute * attr);
gboolean sdp_utils_attribute_is_direction (const GstSDPAttribute * attr, GstSDPDirection * direction);
GstSDPDirection sdp_utils_media_get_direction (const GstSDPMedia * media);
const gchar *sdp_utils_get_direction_str (GstSDPDirection direction);
guint sdp_utils_media_get_ssrc (const GstSDPMedia * media);

GstSDPResult sdp_utils_intersect_sdp_messages (const GstSDPMessage * offer,
    const GstSDPMessage * answer, GstSDPMessage ** offer_result,
    GstSDPMessage ** answer_result);

const gchar *sdp_utils_sdp_media_get_rtpmap (const GstSDPMedia * media,
    const gchar * format);

gchar *gst_sdp_media_format_get_encoding_name (const GstSDPMedia * media,
    const gchar * format);

void sdp_utils_set_max_video_recv_bw (GstSDPMessage * msg, gint max_video_recv_bw);

gboolean sdp_utils_intersect_session_attributes (const GstSDPMessage * msg, GstSDPIntersectMediaFunc func, gpointer user_data);
gboolean sdp_utils_intersect_media_attributes (const GstSDPMedia * offer, GstSDPIntersectMediaFunc func, gpointer user_data);

const gchar *sdp_utils_get_attr_map_value (const GstSDPMedia * media, const gchar *name, const gchar * fmt);

gboolean sdp_utils_for_each_media (const GstSDPMessage * msg, GstSDPMediaFunc func, gpointer user_data);

#endif /* __SDP_H__ */
