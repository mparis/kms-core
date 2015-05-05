#include <gst/gst.h>
#include <MediaPipelineImplFactory.hpp>
#include "MediaPipelineImpl.hpp"
#include <jsonrpc/JsonSerializer.hpp>
#include <KurentoException.hpp>
#include <gst/gst.h>
#include <DotGraph.hpp>
#include <GstreamerDotDetails.hpp>
#include <SignalHandler.hpp>

#define GST_CAT_DEFAULT kurento_media_pipeline_impl
GST_DEBUG_CATEGORY_STATIC (GST_CAT_DEFAULT);
#define GST_DEFAULT_NAME "KurentoMediaPipelineImpl"

namespace kurento
{
void
MediaPipelineImpl::busMessage (GstMessage *message)
{
  switch (message->type) {
  case GST_MESSAGE_ERROR: {
    GError *err = NULL;
    gchar *debug = NULL;

    GST_ERROR ("Error on bus: %" GST_PTR_FORMAT, message);
    gst_debug_bin_to_dot_file_with_ts (GST_BIN (pipeline),
                                       GST_DEBUG_GRAPH_SHOW_ALL, "error");
    gst_message_parse_error (message, &err, &debug);
    std::string errorMessage (err->message);

    if (debug != NULL) {
      errorMessage += " -> " + std::string (debug);
    }

    try {
      Error error (shared_from_this(), errorMessage , 0,
                   "UNEXPECTED_PIPELINE_ERROR");

      signalError (error);
    } catch (std::bad_weak_ptr &e) {
    }

    g_error_free (err);
    g_free (debug);
    break;
  }

  default:
    break;
  }
}

static void
stream_status_cb (GstBus *bus, GstMessage *msg, gpointer user_data)
{
  GstTaskPool *pool = (GstTaskPool *) user_data;
  GstStreamStatusType type;
  const gchar *element_name = GST_OBJECT_NAME (GST_PAD_PARENT (msg->src) );

  gst_message_parse_stream_status (msg, &type, NULL);

  if (type == GST_STREAM_STATUS_TYPE_CREATE) {
    GstTask *task = GST_PAD_TASK (GST_MESSAGE_SRC (msg) );
    gboolean ret;

    if (!g_str_has_prefix (element_name, "queue") &&
//        !g_str_has_prefix (element_name, "rtpjitterbuffer") /*&&*/ /* owns 1 th */
        !g_str_has_prefix (element_name, "dtls") &&
        !g_str_has_prefix (element_name, "nicesrc")) {
      GST_ERROR ("%s NOT USING TASK POOL", element_name);
      return;
    }

    /* Improve too much the latency */
    if (g_str_has_prefix (element_name, "nicesrc0")) {
      GST_ERROR ("%s Master nice src not using task pool", element_name);
      return;
    }

    GST_ERROR ("Set pool to: %s", element_name);

    gst_task_set_pool (task, pool);
    ret = gst_task_set_scheduleable (task, TRUE);
    g_assert (ret);
  }
}

void MediaPipelineImpl::postConstructor ()
{
  GstBus *bus;

  MediaObjectImpl::postConstructor ();

  bus = gst_pipeline_get_bus (GST_PIPELINE (pipeline) );
  gst_bus_add_signal_watch (bus);
  busMessageHandler = register_signal_handler (G_OBJECT (bus), "message",
                      std::function <void (GstBus *, GstMessage *) > (std::bind (
                            &MediaPipelineImpl::busMessage, this,
                            std::placeholders::_2) ),
                      std::dynamic_pointer_cast<MediaPipelineImpl>
                      (shared_from_this() ) );

  /* TaskPool management */
  gst_bus_enable_sync_message_emission (bus);
  g_signal_connect (G_OBJECT (bus), "sync-message::stream-status",
                    G_CALLBACK (stream_status_cb), pool);

  g_object_unref (bus);
}

MediaPipelineImpl::MediaPipelineImpl (const boost::property_tree::ptree &config)
  : MediaObjectImpl (config)
{
  GstClock *clock;

  /* inmediate-TODO: do configurable */
  pool = gst_task_pool_new_full (1, FALSE);
  gst_task_pool_prepare (pool, NULL);

  pipeline = gst_pipeline_new (NULL);

  if (pipeline == NULL) {
    throw KurentoException (MEDIA_OBJECT_NOT_AVAILABLE,
                            "Cannot create gstreamer pipeline");
  }

  g_object_set (G_OBJECT (pipeline), "async-handling", TRUE, NULL);

  clock = gst_system_clock_obtain ();
  gst_pipeline_use_clock (GST_PIPELINE (pipeline), clock);
  g_object_unref (clock);

  gst_element_set_state (pipeline, GST_STATE_PLAYING);

  busMessageHandler = 0;
}

MediaPipelineImpl::~MediaPipelineImpl ()
{
  GstBus *bus = gst_pipeline_get_bus (GST_PIPELINE (pipeline) );

  if (busMessageHandler > 0) {
    unregister_signal_handler (bus, busMessageHandler);
  }

  gst_bus_remove_signal_watch (bus);
  g_object_unref (bus);
  gst_element_set_state (pipeline, GST_STATE_NULL);
  g_object_unref (pipeline);
  gst_task_pool_cleanup (pool);
  gst_object_unref (pool);
}

std::string MediaPipelineImpl::getGstreamerDot (
  std::shared_ptr<GstreamerDotDetails> details)
{
  switch (details->getValue() ) {
  case GstreamerDotDetails::SHOW_MEDIA_TYPE:
    return generateDotGraph (GST_BIN (pipeline), GST_DEBUG_GRAPH_SHOW_MEDIA_TYPE);

  case GstreamerDotDetails::SHOW_CAPS_DETAILS:
    return generateDotGraph (GST_BIN (pipeline), GST_DEBUG_GRAPH_SHOW_CAPS_DETAILS);

  case GstreamerDotDetails::SHOW_NON_DEFAULT_PARAMS:
    return generateDotGraph (GST_BIN (pipeline),
                             GST_DEBUG_GRAPH_SHOW_NON_DEFAULT_PARAMS);

  case GstreamerDotDetails::SHOW_STATES:
    return generateDotGraph (GST_BIN (pipeline), GST_DEBUG_GRAPH_SHOW_STATES);

  case GstreamerDotDetails::SHOW_ALL:
  default:
    return generateDotGraph (GST_BIN (pipeline), GST_DEBUG_GRAPH_SHOW_ALL);
  }
}

std::string MediaPipelineImpl::getGstreamerDot()
{
  return generateDotGraph (GST_BIN (pipeline), GST_DEBUG_GRAPH_SHOW_ALL);
}

MediaObjectImpl *
MediaPipelineImplFactory::createObject (const boost::property_tree::ptree &pt)
const
{
  return new MediaPipelineImpl (pt);
}

MediaPipelineImpl::StaticConstructor MediaPipelineImpl::staticConstructor;

MediaPipelineImpl::StaticConstructor::StaticConstructor()
{
  GST_DEBUG_CATEGORY_INIT (GST_CAT_DEFAULT, GST_DEFAULT_NAME, 0,
                           GST_DEFAULT_NAME);
}

} /* kurento */
