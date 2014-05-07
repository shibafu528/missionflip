#include <gtk/gtk.h>

typedef struct {
    GtkWidget *dialog;
    GtkWidget *treeview;
} ButtonArgs;

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

static void cb_add_clicked(GtkButton *button, gpointer data) {
    GtkWidget *dialog;
    gint response;
    ButtonArgs *args = data;
    dialog = gtk_file_chooser_dialog_new("フォルダを選択",
                                         GTK_WINDOW(args->dialog),
                                         GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER,
                                         GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
                                         GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
                                         NULL);
        
    response = gtk_dialog_run(GTK_DIALOG(dialog));
    if (response == GTK_RESPONSE_ACCEPT) {
        gchar *dirname;
        GtkListStore *store;
        GtkTreeIter iter;
        dirname = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
 
        store = GTK_LIST_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(args->treeview)));
        gtk_list_store_append(store, &iter);
        gtk_list_store_set(store, &iter, 0, dirname, -1);

        g_free(dirname);
    }
    gtk_widget_destroy(dialog);
}

static void cb_remove_clicked(GtkButton *button, gpointer data) {
}

gboolean pick_multi_dir_files(GtkWindow *parent, GList **list) {
    gint response;
    GtkWidget *dialog;
    dialog = gtk_dialog_new_with_buttons("フォルダを指定して画像を開く",
                                         parent,
                                         GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
                                         GTK_STOCK_OK, GTK_RESPONSE_ACCEPT,
                                         GTK_STOCK_CANCEL, GTK_RESPONSE_REJECT,
                                         NULL);
    gtk_widget_set_size_request(dialog, 320, 160);
    {
        GtkWidget *hbox;
        GtkWidget *treeview;
        ButtonArgs args;

        hbox = gtk_hbox_new(FALSE, 0);
        gtk_container_add(GTK_CONTAINER(gtk_dialog_get_content_area(GTK_DIALOG(dialog))), hbox);
        
        treeview = gtk_tree_view_new();

        args.dialog = dialog;
        args.treeview = treeview;
        {
            GtkListStore *store;
            GtkTreeViewColumn *column;
            GtkCellRenderer *renderer;
            
            store = gtk_list_store_new(1, G_TYPE_STRING);
            gtk_tree_view_set_model(GTK_TREE_VIEW(treeview), GTK_TREE_MODEL(store));
            g_object_unref(store);

            renderer = gtk_cell_renderer_text_new();
            
            column = gtk_tree_view_column_new_with_attributes("フォルダ", 
                                                              renderer,
                                                              "text", 0,
                                                              NULL);
            gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);

            gtk_box_pack_start(GTK_BOX(hbox), treeview, TRUE, TRUE, 0);
        }
        {
            GtkWidget *vbox;
            GtkWidget *add_button;
            GtkWidget *remove_button;

            vbox = gtk_vbox_new(FALSE, 0);
            gtk_box_pack_start(GTK_BOX(hbox), vbox, FALSE, FALSE, 0);

            add_button = gtk_button_new_from_stock(GTK_STOCK_ADD);
            gtk_box_pack_start(GTK_BOX(vbox), add_button, FALSE, FALSE, 0);
            g_signal_connect(G_OBJECT(add_button), "clicked",
                             G_CALLBACK(cb_add_clicked), &args);

            remove_button = gtk_button_new_from_stock(GTK_STOCK_REMOVE);
            gtk_box_pack_start(GTK_BOX(vbox), remove_button, FALSE, FALSE, 0);
            g_signal_connect(G_OBJECT(remove_button), "clicked",
                             G_CALLBACK(cb_remove_clicked), &args);
        }
        gtk_widget_show_all(hbox);
    }
    response = gtk_dialog_run(GTK_DIALOG(dialog));
    //TODO: ここでレスポンスに応じたあれこれ
    gtk_widget_destroy(dialog);
    return FALSE;
}
