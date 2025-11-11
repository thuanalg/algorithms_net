#if 1
    #ifndef UNIX_LINUX
        #define UNIX_LINUX
        #define GDK_WINDOWING_WAYLAND
    #endif
#endif
#include <stdio.h>
#include <stdlib.h>
#include <gtk/gtk.h>
#include <gdk/gdkx.h>       // X11 macros
#include <gdk/gdkwayland.h> // Wayland macros
#include <simplelog.h>

GtkWidget* create_textview_fixed(const gchar *initial_text, gint width, gint height);
void *gtk_sendmessage(void *obj);
void on_realize(GtkWidget *widget, gpointer data);
// Tạo tín hiệu mới "custom_event"
#define MY_CUSTOM_EVENT_NAME        "custom_event"

static guint custom_event_signal = 0;

typedef struct _CustomEventData {
    int custom_data;
} CustomEventData;

void trigger_custom_event(GtkWidget *widget) {
    CustomEventData data = { .custom_data = 42 };
    g_signal_emit_by_name(widget, "custom_event", &data);
}

// Callback khi nhận custom_event
void on_custom_event(GtkWidget *widget, CustomEventData *data, gpointer user_data) {
    spllog(1, ">>> [on_custom_event] Custom data: %d, datadata 0x%p\n", 
        data->custom_data, data);
}

// Hàm phát tín hiệu
gboolean emit_event_later(gpointer widget) {
    CustomEventData data = { .custom_data = 100 };
    spllog(1, "datadata 0x%p", &data);
    g_signal_emit_by_name(widget, MY_CUSTOM_EVENT_NAME, &data);
    return FALSE; // chỉ chạy 1 lần
}

int on_send_gui_widget(GtkWidget *widget, void *data) {
    spllog(1, "(widget, data)=(0x%p, 0x%p)", widget, data);
    return 0;
}
static void on_drag_data_received(GtkWidget *widget, GdkDragContext *context, int x, int y,
                                  GtkSelectionData *data, unsigned int info, unsigned int time, gpointer user_data)
{
    spllog(1, ">>> Drag data received at position (%d, %d)\n", x, y);
    const gchar *text = (const gchar *)gtk_selection_data_get_data(data);
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(widget));
    GtkTextIter end_iter;

    gtk_text_buffer_get_end_iter(buffer, &end_iter);
    gtk_text_buffer_insert(buffer, &end_iter, text, -1);
}
// Hàm bắt đầu kéo dữ liệu
static gboolean on_drag_begin(GtkWidget *widget, GdkDragContext *context, gpointer user_data) {
    spllog(1, "Drag begin!\n");
    return FALSE;
}

// Hàm khi kéo thả dữ liệu vào một widget
static gboolean on_drag_motion(GtkWidget *widget, GdkDragContext *context, int x, int y, guint time, gpointer user_data) {
    spllog(1, "Dragging over %d, %d\n", x, y);
    return TRUE;
}

// Hàm xử lý khi thả
static void on_drag_drop(GtkWidget *widget, GdkDragContext *context, int x, int y, guint time, gpointer user_data) {
    spllog(1, "Drop event at %d, %d\n", x, y);
}
GtkWidget *textview1 = 0;
GtkWidget *textview2 = 0;

int main(int argc, char *argv[]) {
    int ret = 0;
    GdkWindow *gdk_window = 0;
    char cfgpath[1024] = {0};
    SPL_INPUT_ARG input = {0};

    snprintf(cfgpath, 1024, "z.cfg");
    snprintf(input.folder, sizeof(input.folder), "%s", cfgpath);
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

    // Đăng ký tín hiệu custom cho tất cả widget GtkTextView
    custom_event_signal = g_signal_new(
        MY_CUSTOM_EVENT_NAME,
        GTK_TYPE_TEXT_VIEW,   // dùng hẳn macro type của GtkTextView
        G_SIGNAL_RUN_LAST,
        0, NULL, NULL, g_cclosure_marshal_VOID__POINTER,
        G_TYPE_NONE, 1, G_TYPE_POINTER);  // Sử dụng G_TYPE_POINTER để truyền kiểu CustomEventData

    // Tạo cửa sổ chính
    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "GTK Two TextViews Example");
    gtk_window_set_default_size(GTK_WINDOW(window), 1200, 480);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    // Dùng GtkFixed để đặt vị trí tuyệt đối
    GtkWidget *fixed = gtk_fixed_new();
    gtk_container_add(GTK_CONTAINER(window), fixed);

    // Tạo 2 TextView cố định, mỗi cái 600x480
    textview1 = create_textview_fixed(
        "TextView 1: Nội dung hiển thị tĩnh giống CStatic.",
        600, 480
    );

    textview2 = create_textview_fixed(
        "TextView 2: Nội dung TextView thứ 2.",
        600, 480
    );

    // Đặt vị trí tuyệt đối
    gtk_fixed_put(GTK_FIXED(fixed), textview1, 0, 0);     // TextView 1 bên trái
    gtk_widget_set_has_window(textview1, TRUE);
    g_signal_connect(textview1, "realize", G_CALLBACK(on_realize), NULL);
    //g_signal_connect(textview1, MY_CUSTOM_EVENT_NAME, G_CALLBACK(on_custom_event), NULL);
    gtk_drag_dest_set(textview1, GTK_DEST_DEFAULT_ALL, NULL, 0, GDK_ACTION_COPY);
    g_signal_connect(textview1, "drag_data_received", G_CALLBACK(on_drag_data_received), NULL);
    g_signal_connect(textview1, "drag_motion", G_CALLBACK(on_drag_motion), NULL);
    g_signal_connect(textview1, "drag_drop", G_CALLBACK(on_drag_drop), NULL);
    g_signal_connect(textview1, "drag_begin", G_CALLBACK(on_drag_begin), NULL);

    gtk_fixed_put(GTK_FIXED(fixed), textview2, 600 + 10, 0);   // TextView 2 bên phải
    gtk_widget_set_has_window(textview2, TRUE);
    //g_signal_connect(textview2, "realize", G_CALLBACK(on_realize), NULL);
    //g_signal_connect(textview2, MY_CUSTOM_EVENT_NAME, G_CALLBACK(on_custom_event), NULL);
    gtk_drag_dest_set(textview2, GTK_DEST_DEFAULT_ALL, NULL, 0, GDK_ACTION_COPY);
    g_signal_connect(textview2, "drag_data_received", G_CALLBACK(on_drag_data_received), NULL);
    g_signal_connect(textview2, "drag_motion", G_CALLBACK(on_drag_motion), NULL);
    g_signal_connect(textview2, "drag_drop", G_CALLBACK(on_drag_drop), NULL);
    g_signal_connect(textview2, "drag_begin", G_CALLBACK(on_drag_begin), NULL);

    gtk_widget_show_all(window);

    // Tạo một thread để gửi tín hiệu sau mỗi 1 giây
    pthread_t threadid = 0;
    //pthread_create(&threadid, 0, gtk_sendmessage, 0);

    gtk_main();
    spllog(1, "testtt");
    ret = spl_finish_log();
    return 0;
}

GtkWidget* create_textview_fixed(const gchar *initial_text, gint width, gint height)
{
    // Tạo TextView
    GtkWidget *textview = gtk_text_view_new();
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(textview), GTK_WRAP_WORD);
    gtk_text_view_set_editable(GTK_TEXT_VIEW(textview), FALSE); // giống CStatic
    gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(textview), FALSE);

    // Set kích thước cố định
    gtk_widget_set_size_request(textview, width, height);

    // Thiết lập GdkWindow riêng (nếu cần)
    gtk_widget_set_has_window(textview, TRUE);

    // Ghi nội dung vào buffer
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(textview));
    gtk_text_buffer_set_text(buffer, initial_text, -1);

    return textview; // Trả luôn TextView, không phải ScrolledWindow
}
gboolean trigger_custom_event_in_gui_thread(void *widget) {
    // Gửi event đến GUI thread
    spllog(1, "(widget)=(0x%p)", widget);
    return FALSE; // Chạy 1 lần
}
void *gtk_sendmessage(void *obj) {
    while(1) {
        spl_milli_sleep(500);
        spllog(1, "================(widget)=(0x%p)", textview1);
        g_idle_add((GSourceFunc)trigger_custom_event_in_gui_thread, textview1);
        spl_milli_sleep(500);
        spllog(1, "================(widget)=(0x%p)", textview2);
        g_idle_add((GSourceFunc)trigger_custom_event_in_gui_thread, textview2);
    }
    return 0;
}

void on_realize(GtkWidget *widget, gpointer data) {
    GdkWindow *gdk_window = gtk_widget_get_window(widget);
    if (gdk_window) {
        spllog(1, "GtkTextView now has its own GdkWindow: %p\n", gdk_window);
        
        // Nếu sử dụng Wayland, lấy wl_surface từ GdkWindow
        #ifdef GDK_WINDOWING_WAYLAND
        if (GDK_IS_WAYLAND_DISPLAY(gdk_display_get_default())) {
            struct wl_surface *surface = gdk_wayland_window_get_wl_surface(gdk_window);
            spllog(1, "Native Wayland wl_surface: %p\n", surface);
        }
        #endif
    } else {
        spllog(1, "GtkTextView does not have a GdkWindow.\n");
    }
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