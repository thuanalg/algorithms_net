#if 1
    #ifndef UNIX_LINUX
        #define UNIX_LINUX
    #endif
#endif
#include <stdio.h>
#include <stdlib.h>
#include <gtk/gtk.h>
#include <simplelog.h>

int main(int argc, char *argv[]) {
    return 0;
}

#if 0
#include <gtk/gtk.h>

void send_custom_event(GtkWidget *widget) {
    // Make sure the widget has a GdkWindow (realized)
    gtk_widget_realize(widget);

    // Create a simple client (custom) event
    GdkEvent *event = gdk_event_new(GDK_CLIENT_EVENT);
    event->any.window = gtk_widget_get_window(widget);
    event->any.send_event = TRUE;

    // Optional: you can attach custom data if you subclass or use a signal

    gdk_event_put(event);  // <--- This "posts" it to the GTK main loop

    gdk_event_free(event);
}

gboolean on_event(GtkWidget *widget, GdkEvent *event, gpointer user_data) {
    if (event->type == GDK_CLIENT_EVENT) {
        g_print("Custom event received by window!\n");
        return TRUE; // handled
    }
    return FALSE;
}

int main(int argc, char *argv[]) {
    gtk_init();

    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_widget_show(window);

    g_signal_connect(window, "event", G_CALLBACK(on_event), NULL);

    // Send a custom event after a delay
    g_timeout_add(1000, (GSourceFunc)send_custom_event, window);

    gtk_main();
}

#endif