#include <gst/gst.h>
#include <stdio.h>
#include <signal.h>

static GstElement *pipeline;
static GMainLoop *main_loop;

static void bus_message_handler(GstBus *bus, GstMessage *msg, gpointer data) {
    switch (GST_MESSAGE_TYPE(msg)) {
        case GST_MESSAGE_EOS:
            g_print("Recording finished.\n");
            g_main_loop_quit((GMainLoop *)data);
            break;
        case GST_MESSAGE_ERROR: {
            GError *err;
            gchar *debug_info;
            gst_message_parse_error(msg, &err, &debug_info);
            g_printerr("Error: %s\n", err->message);
            g_error_free(err);
            g_free(debug_info);
            g_main_loop_quit((GMainLoop *)data);
            break;
        }
        default:
            break;
    }
}

void handle_sigint(int sig) {
    g_print("Stopping pipeline...\n");
    gst_element_send_event(pipeline, gst_event_new_eos());
}

int main(int argc, char *argv[]) {
    gst_init(&argc, &argv);

    // Pipeline quay video từ camera và âm thanh từ mic, ghi ra MP4
const char *pipeline_description =
    "v4l2src device=/dev/video0 ! videoconvert ! video/x-raw,format=I420 ! x264enc tune=zerolatency bitrate=2048 speed-preset=superfast ! queue ! mux. "
    "pulsesrc ! audioconvert ! audioresample ! audio/x-raw,rate=44100,channels=2 ! avenc_aac bitrate=128000 ! queue ! mux. "
    "mp4mux name=mux ! filesink location=output.mp4";

    GError *error = NULL;
    pipeline = gst_parse_launch(pipeline_description, &error);

    if (!pipeline) {
        g_printerr("Pipeline creation failed: %s\n", error->message);
        g_error_free(error);
        return -1;
    }

    signal(SIGINT, handle_sigint);

    gst_element_set_state(pipeline, GST_STATE_PLAYING);
    g_print("Recording video + audio... Press Ctrl+C to stop.\n");

    main_loop = g_main_loop_new(NULL, FALSE);

    GstBus *bus = gst_element_get_bus(pipeline);
    gst_bus_add_signal_watch(bus);
    g_signal_connect(bus, "message", G_CALLBACK(bus_message_handler), main_loop);

    g_main_loop_run(main_loop);

    gst_element_set_state(pipeline, GST_STATE_NULL);
    gst_object_unref(pipeline);
    gst_object_unref(bus);
    g_main_loop_unref(main_loop);

    return 0;
}
