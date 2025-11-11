#if 1
    #ifndef UNIX_LINUX
        #define UNIX_LINUX
    #endif
#endif
#include <stdio.h>
#include <stdlib.h>
#include <gtk/gtk.h>
#include <gdk/gdkx.h>       // X11 macros
#include <gdk/gdkwayland.h> // Wayland macros
#include <simplelog.h>

GtkWidget* create_textview_fixed(const gchar *initial_text, gint width, gint height);

int main(int argc, char *argv[]) {
    int ret = 0;
    char cfgpath[1024] = {0};
    SPL_INPUT_ARG input = {0};

    snprintf(cfgpath, 1024, "z.cfg");
    snprintf(input.folder, SPL_PATH_FOLDER, "%s", cfgpath);
    snprintf(input.id_name, 100, "vk_window");
    ret = spl_init_log_ext(&input);
    if (ret) {
        exit(1);
    }

    gtk_init(&argc, &argv);

    GdkDisplay *display = gdk_display_get_default();
    if (!display) {
        spllog(1, "No display found.\n");
        return 1;
    }

#ifdef GDK_WINDOWING_X11
    if (GDK_IS_X11_DISPLAY(display)) {
        spllog(1, "GTK is running on X11\n");
    }
#endif

#ifdef GDK_WINDOWING_WAYLAND
    if (GDK_IS_WAYLAND_DISPLAY(display)) {
        spllog(1, "GTK is running on Wayland\n");
    }
#endif

    // Tạo cửa sổ chính
    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "GTK Two TextViews Example");
    gtk_window_set_default_size(GTK_WINDOW(window), 1200, 480);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    // Dùng GtkFixed để đặt vị trí tuyệt đối
    GtkWidget *fixed = gtk_fixed_new();
    gtk_container_add(GTK_CONTAINER(window), fixed);

    // Tạo 2 TextView cố định, mỗi cái 600x480
    GtkWidget *textview1 = create_textview_fixed(
        "TextView 1: Nội dung hiển thị tĩnh giống CStatic.",
        600, 480
    );

    GtkWidget *textview2 = create_textview_fixed(
        "TextView 2: Nội dung TextView thứ 2.",
        600, 480
    );

    // Đặt vị trí tuyệt đối
    gtk_fixed_put(GTK_FIXED(fixed), textview1, 0, 0);     // TextView 1 bên trái
    gtk_fixed_put(GTK_FIXED(fixed), textview2, 600 + 10, 0);   // TextView 2 bên phải

    gtk_widget_show_all(window);
    gtk_main();

    spllog(1, "testtt");
    ret = spl_finish_log();
    return 0;
}

// Hàm tạo TextView cố định
GtkWidget* create_textview_fixed(const gchar *initial_text, gint width, gint height) {
    GtkWidget *scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_widget_set_size_request(scrolled_window, width, height);

    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window),
                                   GTK_POLICY_AUTOMATIC,
                                   GTK_POLICY_AUTOMATIC);

    GtkWidget *textview = gtk_text_view_new();
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(textview), GTK_WRAP_WORD);
    gtk_text_view_set_editable(GTK_TEXT_VIEW(textview), FALSE); // giống CStatic
    gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(textview), FALSE);

    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(textview));
    gtk_text_buffer_set_text(buffer, initial_text, -1);

    gtk_container_add(GTK_CONTAINER(scrolled_window), textview);
    return scrolled_window;
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
/* 
✅ Works in both X11 and Wayland.
✅ Similar to PostMessage(hwnd, WM_USER, …) — posts asynchronously to the main loop.
⚠️ Limited to your process; can’t target another app’s window.
*/
#endif