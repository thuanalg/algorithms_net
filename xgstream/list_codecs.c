#include <gst/gst.h>

static void list_codecs() {
    GstRegistry *registry;
    GstPlugin *plugin;
    GstPluginFeature *feature;
    GstElementFactory *factory;

    // Get the global registry of all installed plugins
    registry = gst_registry_get();

    // Iterate through all plugins in the registry
    GList *plugin_list = gst_registry_get_plugin_list(registry);
    for (GList *item = plugin_list; item; item = item->next) {
        plugin = GST_PLUGIN(item->data);

        // Iterate through all features (elements) in the plugin
        GList *feature_list = gst_plugin_get_feature_list(plugin);
        for (GList *f_item = feature_list; f_item; f_item = f_item->next) {
            feature = GST_PLUGIN_FEATURE(f_item->data);

            // Check if the feature is an element factory (which represents codecs)
            if (GST_IS_ELEMENT_FACTORY(feature)) {
                factory = GST_ELEMENT_FACTORY(feature);

                // Get the name of the element (encoder/decoder)
                const gchar *name = gst_element_factory_get_longname(factory);
#if 0
                // Get the element details (e.g., "Decoder", "Encoder")
                GstElementDetails details;
                gst_element_factory_get_details(factory, &details);

                // Check if the element is a decoder or encoder
                if (g_strrstr(details.longname, "Decoder") || g_strrstr(details.longname, "Encoder")) {
                    g_print("Codec: %s\n", name);
                }
#endif
            }
        }
        g_list_free(feature_list);
    }
    g_list_free(plugin_list);
}

int main(int argc, char *argv[]) {
    // Initialize GStreamer
    gst_init(&argc, &argv);

    // List all codecs (encoders and decoders)
    list_codecs();

    // Clean up GStreamer
    gst_deinit();

    return 0;
}

// gcc -g -o list_codecs list_codecs.c `pkg-config --cflags --libs gstreamer-1.0`