#include <gtk/gtk.h>

GList *filelist = NULL;
GtkWidget *window;

void search_images(GList **list, gchar *dirname) {
    GDir *dir;
    dir = g_dir_open(dirname, 0, NULL);
    if (dir) {
        const gchar *name;
        while (name = g_dir_read_name(dir)) {
            gchar *path;
            path = g_build_filename(dirname, name, NULL);
            if (g_file_test(path, G_FILE_TEST_IS_DIR)) {
                search_images(list, path);
            }
            else {
                *list = g_list_append(*list, path);
            }
        }
        g_dir_close(dir);
    }
}

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

static void next_image(GtkWidget *image) {
    if (g_list_next(filelist) != NULL) {
        filelist = g_list_next(filelist);
    }
    else {
        filelist = g_list_first(filelist);
    }
    gtk_image_set_from_file(GTK_IMAGE(image), filelist->data);
    set_window_title();
}

static void prev_image(GtkWidget *image) {
    if (g_list_previous(filelist) != NULL) {
        filelist = g_list_previous(filelist);
    }
    else {
        filelist = g_list_last(filelist);
    }
    gtk_image_set_from_file(GTK_IMAGE(image), filelist->data);
    set_window_title();
}

static void cb_image_clicked(GtkWidget *event_box, GdkEventButton *event, gpointer data) {
    switch (event->button) {
    case 1: /* Left Click */
        if (filelist == NULL) {
            GtkWidget *dialog;
            gint response;
            dialog = gtk_file_chooser_dialog_new("Open an image",
                                                 GTK_WINDOW(window),
                                                 GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER,
                                                 GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
                                                 GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
                                                 NULL);
        
            response = gtk_dialog_run(GTK_DIALOG(dialog));
            if (response == GTK_RESPONSE_ACCEPT) {
                gchar *dirname;
                dirname = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
                search_images(&filelist, dirname);
                filelist = g_list_last(filelist);
                next_image(GTK_WIDGET(data));
                g_free(dirname);
            }
            gtk_widget_destroy(dialog);
        }
        else {
            next_image(GTK_WIDGET(data));
        }
        break;
    case 2: /* Middle Click */
        if (filelist != NULL) {
            prev_image(GTK_WIDGET(data));
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
        GtkWidget *event_box, *image;
        
        event_box = gtk_event_box_new();
        gtk_container_add(GTK_CONTAINER(window), event_box);
        
        image = gtk_image_new();
        gtk_container_add(GTK_CONTAINER(event_box), image);

        gtk_widget_set_events(event_box, GDK_BUTTON_RELEASE_MASK);
        g_signal_connect(G_OBJECT(event_box), "button_release_event", 
                         G_CALLBACK(cb_image_clicked), image);
    }
    gtk_widget_show_all(window);
}

int main(int argc, char* argv[]) {
    gtk_init(&argc, &argv);
    make_and_show_mainwindow();
    gtk_main();

    return 0;
}
