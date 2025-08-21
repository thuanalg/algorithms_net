#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <wayland-client.h>
#include <wayland-egl.h>
#include <EGL/egl.h>
#include <GLES2/gl2.h>

#if 0

#define WIDTH 800
#define HEIGHT 600

struct wl_display *display;
struct wl_compositor *compositor;
struct wl_surface *surface;
struct wl_registry *registry;
struct wl_egl_window *egl_window;

EGLDisplay egl_display;
EGLContext egl_context;
EGLSurface egl_surface;
EGLConfig egl_config;

static void registry_handler(void *data, struct wl_registry *registry,
                             uint32_t id, const char *interface,
                             uint32_t version) {
    if (strcmp(interface, "wl_compositor") == 0) {
        compositor = wl_registry_bind(registry, id, &wl_compositor_interface, 1);
    }
}

static void registry_remove(void *data, struct wl_registry *registry,
                            uint32_t id) {}

static const struct wl_registry_listener registry_listener = {
    .global = registry_handler,
    .global_remove = registry_remove
};

void init_wayland_egl() {
    display = wl_display_connect(NULL);
    if (!display) {
        fprintf(stderr, "Can't connect to Wayland.\n");
        exit(1);
    }

    registry = wl_display_get_registry(display);
    wl_registry_add_listener(registry, &registry_listener, NULL);
    wl_display_roundtrip(display);

    surface = wl_compositor_create_surface(compositor);
    egl_window = wl_egl_window_create(surface, WIDTH, HEIGHT);

    egl_display = eglGetDisplay((EGLNativeDisplayType)display);
    eglInitialize(egl_display, NULL, NULL);

    EGLint attribs[] = {
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        EGL_RED_SIZE, 8, EGL_GREEN_SIZE, 8,
        EGL_BLUE_SIZE, 8, EGL_ALPHA_SIZE, 8,
        EGL_NONE
    };
    EGLint num_configs;
    eglChooseConfig(egl_display, attribs, &egl_config, 1, &num_configs);

    EGLint ctx_attribs[] = { EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE };
    egl_context = eglCreateContext(egl_display, egl_config, EGL_NO_CONTEXT, ctx_attribs);
    egl_surface = eglCreateWindowSurface(egl_display, egl_config, (EGLNativeWindowType)egl_window, NULL);

    eglMakeCurrent(egl_display, egl_surface, egl_surface, egl_context);
}

void render_loop() {
    const char *vs =
        "attribute vec2 pos;\n"
        "void main() {\n"
        "  gl_Position = vec4(pos, 0.0, 1.0);\n"
        "}";
    const char *fs =
        "void main() {\n"
        "  gl_FragColor = vec4(0.3, 1.0, 0.7, 1.0);\n"
        "}";

    GLuint vert = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vert, 1, &vs, NULL);
    glCompileShader(vert);

    GLuint frag = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(frag, 1, &fs, NULL);
    glCompileShader(frag);

    GLuint prog = glCreateProgram();
    glAttachShader(prog, vert);
    glAttachShader(prog, frag);
    glLinkProgram(prog);
    glUseProgram(prog);

    const int N = 500;
    GLfloat points[N * 2];
    for (int i = 0; i < N; ++i) {
        float x = (float)i / (N - 1) * 2.0f - 1.0f;
        float y = sinf(x * 3.14f * 4.0f) * 0.6f;
        points[i * 2] = x;
        points[i * 2 + 1] = y;
    }

    GLint loc = glGetAttribLocation(prog, "pos");
    glEnableVertexAttribArray(loc);

    while (1) {
        glViewport(0, 0, WIDTH, HEIGHT);
        glClearColor(0.1, 0.1, 0.1, 1);
        glClear(GL_COLOR_BUFFER_BIT);

        glVertexAttribPointer(loc, 2, GL_FLOAT, GL_FALSE, 0, points);
        glDrawArrays(GL_LINE_STRIP, 0, N);

        eglSwapBuffers(egl_display, egl_surface);
        wl_display_dispatch_pending(display);
    }
}

int main() {
    init_wayland_egl();
    render_loop();
    return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wayland-client.h>
#include <wayland-egl.h>
#include <EGL/egl.h>
#include <GLES2/gl2.h>

#define WIDTH 800
#define HEIGHT 600

struct wl_display *display;
struct wl_compositor *compositor;
struct wl_surface *surface;
struct wl_registry *registry;
struct wl_egl_window *egl_window;

EGLDisplay egl_display;
EGLContext egl_context;
EGLSurface egl_surface;
EGLConfig egl_config;

static void registry_handler(void *data, struct wl_registry *registry,
                             uint32_t id, const char *interface,
                             uint32_t version) {
    if (strcmp(interface, "wl_compositor") == 0) {
        compositor = wl_registry_bind(registry, id, &wl_compositor_interface, 1);
    }
}

static void registry_remove(void *data, struct wl_registry *registry,
                            uint32_t id) {}

static const struct wl_registry_listener registry_listener = {
    .global = registry_handler,
    .global_remove = registry_remove
};

void init_wayland_egl() {
    display = wl_display_connect(NULL);
    if (!display) {
        fprintf(stderr, "Can't connect to Wayland.\n");
        exit(1);
    }

    registry = wl_display_get_registry(display);
    wl_registry_add_listener(registry, &registry_listener, NULL);
    wl_display_roundtrip(display);

    if (!compositor) {
        fprintf(stderr, "Can't get wl_compositor\n");
        exit(1);
    }

    surface = wl_compositor_create_surface(compositor);
    egl_window = wl_egl_window_create(surface, WIDTH, HEIGHT);

    wl_surface_commit(surface);
    wl_display_flush(display);

    egl_display = eglGetDisplay((EGLNativeDisplayType)display);
    eglInitialize(egl_display, NULL, NULL);

    EGLint attribs[] = {
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        EGL_RED_SIZE, 8, EGL_GREEN_SIZE, 8,
        EGL_BLUE_SIZE, 8, EGL_ALPHA_SIZE, 8,
        EGL_NONE
    };

    EGLint num_configs;
    eglChooseConfig(egl_display, attribs, &egl_config, 1, &num_configs);

    EGLint ctx_attribs[] = { EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE };
    egl_context = eglCreateContext(egl_display, egl_config, EGL_NO_CONTEXT, ctx_attribs);

    egl_surface = eglCreateWindowSurface(egl_display, egl_config, (EGLNativeWindowType)egl_window, NULL);

    eglMakeCurrent(egl_display, egl_surface, egl_surface, egl_context);
}

void render() {
    const char *vertex_shader_src =
        "attribute vec2 position;\n"
        "void main() {\n"
        "  gl_Position = vec4(position, 0.0, 1.0);\n"
        "}\n";

    const char *fragment_shader_src =
        "void main() {\n"
        "  gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);\n"
        "}\n";

    GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_shader_src, NULL);
    glCompileShader(vertex_shader);

    GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_shader_src, NULL);
    glCompileShader(fragment_shader);

    GLuint program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);
    glUseProgram(program);

    // 2 điểm của đường thẳng (-0.5,0) -> (0.5,0)
    GLfloat vertices[] = {
        -0.5f, 0.0f,
         0.5f, 0.0f
    };

    GLint pos_attrib = glGetAttribLocation(program, "position");
    glEnableVertexAttribArray(pos_attrib);
    glVertexAttribPointer(pos_attrib, 2, GL_FLOAT, GL_FALSE, 0, vertices);

    // Vòng render đơn giản
    while (1) {
        glViewport(0, 0, WIDTH, HEIGHT);
        glClearColor(0, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT);

        glDrawArrays(GL_LINES, 0, 2);

        eglSwapBuffers(egl_display, egl_surface);
        wl_display_dispatch_pending(display);
    }
}

int main() {
    init_wayland_egl();
    render();
    return 0;
}
#endif










#if 0

#include <gtk/gtk.h>
#include <epoxy/gl.h>

static void on_realize(GtkGLArea *area) {
    gtk_gl_area_make_current(area);
}

static gboolean on_render(GtkGLArea *area, GdkGLContext *context) {
    static int count = 0;
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);
    fprintf(stdout, "-----count: %d\n", count++);
    return TRUE;
}

int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);

    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_default_size(GTK_WINDOW(window), 400, 400);

    GtkWidget *gl_area = gtk_gl_area_new();
    g_signal_connect(gl_area, "realize", G_CALLBACK(on_realize), NULL);
    g_signal_connect(gl_area, "render", G_CALLBACK(on_render), NULL);

    gtk_container_add(GTK_CONTAINER(window), gl_area);

    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    gtk_widget_show_all(window);
    gtk_main();
    return 0;
}

#else

#include <gtk/gtk.h>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_wayland.h>
#include <gdk/wayland/gdkwayland.h>
#include <stdio.h>
#include <stdlib.h>

static VkInstance instance;
static VkSurfaceKHR surface;

static void create_vulkan_instance() {
    VkApplicationInfo app_info = {0};
    app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    app_info.pApplicationName = "GTK4 Vulkan Wayland Example";
    app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.pEngineName = "No Engine";
    app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.apiVersion = VK_API_VERSION_1_0;

    const char *extensions[] = {
        VK_KHR_SURFACE_EXTENSION_NAME,
        VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME,
    };

    VkInstanceCreateInfo create_info = {0};
    create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    create_info.pApplicationInfo = &app_info;
    create_info.enabledExtensionCount = 2;
    create_info.ppEnabledExtensionNames = extensions;

    VkResult res = vkCreateInstance(&create_info, NULL, &instance);
    if (res != VK_SUCCESS) {
        fprintf(stderr, "Failed to create Vulkan instance\n");
        exit(1);
    }
}

static void create_vulkan_surface(GtkWidget *widget) {
    // Lấy GtkWindow từ widget
    GtkWindow *window = GTK_WINDOW(gtk_widget_get_root(widget));
    if (!GTK_IS_WINDOW(window)) {
        fprintf(stderr, "Root widget is not a GtkWindow\n");
        exit(1);
    }

    GdkSurface *gdk_surface = gtk_native_get_surface(GTK_NATIVE(window));
    if (!gdk_surface) {
        fprintf(stderr, "Failed to get GdkSurface from GtkWindow\n");
        exit(1);
    }

    struct wl_display *wl_display = gdk_wayland_display_get_wl_display(gdk_surface_get_display(gdk_surface));
    struct wl_surface *wl_surface = gdk_wayland_surface_get_wl_surface(gdk_surface);

    if (!wl_display || !wl_surface) {
        fprintf(stderr, "Failed to get wl_display or wl_surface\n");
        exit(1);
    }

    VkWaylandSurfaceCreateInfoKHR create_info = {0};
    create_info.sType = VK_STRUCTURE_TYPE_WAYLAND_SURFACE_CREATE_INFO_KHR;
    create_info.display = wl_display;
    create_info.surface = wl_surface;

    VkResult res = vkCreateWaylandSurfaceKHR(instance, &create_info, NULL, &surface);
    if (res != VK_SUCCESS) {
        fprintf(stderr, "Failed to create Vulkan Wayland surface\n");
        exit(1);
    }
}

static void activate(GtkApplication *app, gpointer user_data) {
    GtkWidget *window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "GTK4 Vulkan Wayland Example");
    gtk_window_set_default_size(GTK_WINDOW(window), 800, 600);

    GtkWidget *drawing_area = gtk_drawing_area_new();
    gtk_window_set_child(GTK_WINDOW(window), drawing_area);

    // Đảm bảo widget đã được realize (cần để surface native có thể lấy được)
    gtk_widget_realize(window);

    create_vulkan_instance();
    create_vulkan_surface(window);

    gtk_window_present(GTK_WINDOW(window));
}

int main(int argc, char **argv) {
    GtkApplication *app = gtk_application_new("org.example.GtkVulkanWayland", 0);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);

    int status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);

    if (surface)
        vkDestroySurfaceKHR(instance, surface, NULL);
    if (instance)
        vkDestroyInstance(instance, NULL);

    return status;
}
#endif