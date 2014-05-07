#include <gtk/gtk.h>

GList *filelist = NULL;
GdkPixbuf *current;
GtkWidget *window;

void search_images(GList **list, gchar *dirname);
gboolean pick_multi_dir_files(GtkWindow *parent, GList **list);

static void set_window_title() {
    GList *f;
    guint pos, len;
    gchar *title;
    f = g_list_first(filelist);
    pos = g_list_position(f, filelist) + 1;
    len = g_list_length(f);
    title = g_strdup_printf("%d/%d", pos, len);
    gtk_window_set_title(GTK_WINDOW(window), title);
    g_free(title);
}

static void load_image(GtkWidget *draw, const gchar *path) {
    if (current != NULL) {
        g_object_unref(current);
    }
    current = gdk_pixbuf_new_from_file(path, NULL);
    gtk_widget_queue_draw(draw);
    set_window_title();
}

static void next_image(GtkWidget *draw) {
    if (g_list_next(filelist) != NULL) {
        filelist = g_list_next(filelist);
    }
    else {
        filelist = g_list_first(filelist);
    }
    load_image(draw, filelist->data);
}

static void prev_image(GtkWidget *draw) {
    if (g_list_previous(filelist) != NULL) {
        filelist = g_list_previous(filelist);
    }
    else {
        filelist = g_list_last(filelist);
    }
    load_image(draw, filelist->data);
}

static gint cb_expose(GtkWidget *widget, GdkEventExpose *event, gpointer data) {
    if (current != NULL) {
        int w, h;
        float s;
        GdkPixbuf *scaled;
        w = gdk_pixbuf_get_width(current);
        h = gdk_pixbuf_get_height(current);

        if (event->area.width > event->area.height) {
            s = (float)event->area.height / h;
        }
        else {
            s = (float)event->area.width / w;
        }

        w = w * s;
        h = h * s;

        scaled = gdk_pixbuf_scale_simple(current, w, h, GDK_INTERP_BILINEAR);
        gdk_draw_pixbuf(widget->window, widget->style->fg_gc[GTK_STATE_NORMAL], scaled,
                        0, 0, 
                        event->area.x+(event->area.width/2-w/2), 
                        event->area.y+(event->area.height/2-h/2),
                        w, h,
                        GDK_RGB_DITHER_NORMAL, event->area.x, event->area.y);
    
        g_object_unref(scaled);
    }
    return TRUE;
}

static void cb_clicked(GtkWidget *widget, GdkEventButton *event, gpointer data) {
    switch (event->button) {
    case 1: /* Left Click */
        if (filelist == NULL) {
            int response;
            response = pick_multi_dir_files(GTK_WINDOW(window), &filelist);
        }
        else {
            next_image(widget);
        }
        break;
    case 2: /* Middle Click */
        if (filelist != NULL) {
            prev_image(widget);
        }
        break;
    case 3: /* Right Click */
        gtk_window_iconify(GTK_WINDOW(window));
        break;
    }
}

static void make_and_show_mainwindow() {
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "MissionFlip");
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    gtk_widget_set_size_request(window, 640, 480);
    {
        GtkWidget *canvas;
        
        canvas = gtk_drawing_area_new();
        gtk_widget_set_size_request(canvas, 640, 480);
        gtk_container_add(GTK_CONTAINER(window), canvas);

        g_signal_connect(G_OBJECT(canvas), "expose_event",
                         G_CALLBACK(cb_expose), NULL);

        g_signal_connect(G_OBJECT(window), "button_release_event", 
                         G_CALLBACK(cb_clicked), canvas);        
        gtk_widget_add_events(window, GDK_BUTTON_RELEASE_MASK);
    }
    gtk_widget_show_all(window);
}

int main(int argc, char* argv[]) {
    gtk_init(&argc, &argv);
    make_and_show_mainwindow();
    gtk_main();

    return 0;
}
