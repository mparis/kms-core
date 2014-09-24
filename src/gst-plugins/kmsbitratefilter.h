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

#ifndef __KMS_BITRATE_FILTER_H__
#define __KMS_BITRATE_FILTER_H__

#include <gst/gst.h>
#include <gst/base/gstbasetransform.h>

G_BEGIN_DECLS
/* #defines don't like whitespacey bits */
#define KMS_TYPE_BITRATE_FILTER \
  (kms_bitrate_filter_get_type())
#define KMS_BITRATE_FILTER(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST((obj),KMS_TYPE_BITRATE_FILTER,KmsBitrateFilter))
#define KMS_BITRATE_FILTER_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST((klass),KMS_TYPE_BITRATE_FILTER,KmsBitrateFilterClass))
#define KMS_IS_BITRATE_FILTER(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE((obj),KMS_TYPE_BITRATE_FILTER))
#define KMS_IS_BITRATE_FILTER_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE((klass),KMS_TYPE_BITRATE_FILTER))
#define KMS_BITRATE_FILTER_CAST(obj) ((KmsBitrateFilter*)(obj))

typedef struct _KmsBitrateFilter KmsBitrateFilter;
typedef struct _KmsBitrateFilterClass KmsBitrateFilterClass;
typedef struct _KmsBitrateFilterPrivate KmsBitrateFilterPrivate;

struct _KmsBitrateFilter
{
  GstBaseTransform parent;

  KmsBitrateFilterPrivate *priv;
};

struct _KmsBitrateFilterClass
{
  GstBaseTransformClass parent_class;
};

GType kms_bitrate_filter_get_type (void);

gboolean kms_bitrate_filter_plugin_init (GstPlugin * plugin);

G_END_DECLS
#endif /* __KMS_BITRATE_FILTER_H__ */
