#include <gtk/gtk.h>
#include <sodium.h>
#include <glib/gstdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#define MAGIC "PSAFE01"
#define MAGIC_LEN 7
#define SALT_LEN crypto_pwhash_SALTBYTES
#define NONCE_LEN crypto_secretbox_NONCEBYTES
#define MAC_LEN crypto_secretbox_MACBYTES

typedef struct { char *app, *type, *username, *email, *phone, *password; } Item;
typedef struct { GtkWidget *win, *list, *status; GPtrArray *items; char *path; unsigned char salt[SALT_LEN], key[crypto_secretbox_KEYBYTES]; gboolean unlocked; } State;
static State s = {0};

static void item_free(gpointer p) {
    Item *i=p; if(!i) return;
    g_free(i->app); g_free(i->type); g_free(i->username); g_free(i->email);
    g_free(i->phone); if(i->password) { sodium_memzero(i->password,strlen(i->password)); g_free(i->password); } g_free(i);
}
static void alert(const char *msg) {
    GtkWidget *w=gtk_window_new(); gtk_window_set_title(GTK_WINDOW(w),"Password Safe");
    gtk_window_set_transient_for(GTK_WINDOW(w),GTK_WINDOW(s.win)); gtk_window_set_modal(GTK_WINDOW(w),TRUE);
    GtkWidget *v=gtk_box_new(GTK_ORIENTATION_VERTICAL,12);
    gtk_widget_set_margin_top(v,22);gtk_widget_set_margin_bottom(v,22);
    gtk_widget_set_margin_start(v,22);gtk_widget_set_margin_end(v,22);
    GtkWidget *l=gtk_label_new(msg);gtk_label_set_wrap(GTK_LABEL(l),TRUE);
    gtk_box_append(GTK_BOX(v),l);
    GtkWidget *b=gtk_button_new_with_label("OK");g_signal_connect_swapped(b,"clicked",G_CALLBACK(gtk_window_destroy),w);
    gtk_box_append(GTK_BOX(v),b);gtk_window_set_child(GTK_WINDOW(w),v);gtk_window_present(GTK_WINDOW(w));
}
static gboolean derive(const char *master) {
    return crypto_pwhash(s.key,sizeof s.key,master,strlen(master),s.salt,
        crypto_pwhash_OPSLIMIT_MODERATE,crypto_pwhash_MEMLIMIT_MODERATE,
        crypto_pwhash_ALG_ARGON2ID13)==0;
}
static gboolean write_private(const unsigned char *buf, size_t len) {
    char *tmp=g_strdup_printf("%s.tmp",s.path);
    int fd=g_open(tmp,O_WRONLY|O_CREAT|O_TRUNC,0600);
    if(fd<0) {g_free(tmp);return FALSE;}
    size_t pos=0;gboolean ok=TRUE;
    while(pos<len) {ssize_t n=write(fd,buf+pos,len-pos);if(n<0 && errno==EINTR)continue;if(n<=0){ok=FALSE;break;}pos+=(size_t)n;}
    if(ok && fsync(fd)!=0)ok=FALSE;
    if(close(fd)!=0)ok=FALSE;
    if(ok && g_rename(tmp,s.path)!=0)ok=FALSE;
    if(!ok)g_unlink(tmp);
    g_free(tmp);return ok;
}
static gboolean save_vault(void) {
    GKeyFile *kf=g_key_file_new();
    for(guint x=0;x<s.items->len;x++) {
        Item *i=g_ptr_array_index(s.items,x);char *section=g_strdup_printf("account_%u",x);
        g_key_file_set_string(kf,section,"app",i->app);
        g_key_file_set_string(kf,section,"type",i->type);
        g_key_file_set_string(kf,section,"username",i->username);
        g_key_file_set_string(kf,section,"email",i->email);
        g_key_file_set_string(kf,section,"phone",i->phone);
        g_key_file_set_string(kf,section,"password",i->password);
        g_free(section);
    }
    gsize plain_len=0;char *plain=g_key_file_to_data(kf,&plain_len,NULL);g_key_file_unref(kf);
    size_t header=MAGIC_LEN+SALT_LEN+NONCE_LEN;
    size_t total=header+MAC_LEN+plain_len;
    unsigned char *buf=g_malloc(total);
    memcpy(buf,MAGIC,MAGIC_LEN);memcpy(buf+MAGIC_LEN,s.salt,SALT_LEN);
    unsigned char *nonce=buf+MAGIC_LEN+SALT_LEN;randombytes_buf(nonce,NONCE_LEN);
    crypto_secretbox_easy(buf+header,(const unsigned char *)plain,plain_len,nonce,s.key);
    gboolean ok=write_private(buf,total);
    sodium_memzero(plain,plain_len);g_free(plain);sodium_memzero(buf,total);g_free(buf);
    return ok;
}
static gboolean load_vault(void) {
    gchar *contents=NULL;gsize len=0;
    if(!g_file_get_contents(s.path,&contents,&len,NULL))return FALSE;
    size_t header=MAGIC_LEN+SALT_LEN+NONCE_LEN;
    if(len<header+MAC_LEN || memcmp(contents,MAGIC,MAGIC_LEN)!=0) {g_free(contents);return FALSE;}
    memcpy(s.salt,contents+MAGIC_LEN,SALT_LEN);
    /* Key has already been derived by the unlock handler. */
    size_t plain_len=len-header-MAC_LEN;
    unsigned char *plain=g_malloc(plain_len+1);
    gboolean ok=crypto_secretbox_open_easy(plain,(unsigned char *)contents+header,
        len-header,(unsigned char *)contents+MAGIC_LEN+SALT_LEN,s.key)==0;
    if(ok) {
        plain[plain_len]=0;
        GKeyFile *kf=g_key_file_new();
        if(plain_len && !g_key_file_load_from_data(kf,(char *)plain,plain_len,G_KEY_FILE_NONE,NULL))ok=FALSE;
        if(ok){
            gsize n=0;gchar **groups=g_key_file_get_groups(kf,&n);
            for(gsize x=0;x<n;x++) {
                Item *i=g_new0(Item,1);
                i->app=g_key_file_get_string(kf,groups[x],"app",NULL);
                i->type=g_key_file_get_string(kf,groups[x],"type",NULL);
                i->username=g_key_file_get_string(kf,groups[x],"username",NULL);
                i->email=g_key_file_get_string(kf,groups[x],"email",NULL);
                i->phone=g_key_file_get_string(kf,groups[x],"phone",NULL);
                i->password=g_key_file_get_string(kf,groups[x],"password",NULL);
                if(!i->app||!i->type||!i->username||!i->email||!i->phone||!i->password){item_free(i);ok=FALSE;break;}
                g_ptr_array_add(s.items,i);
            }
            g_strfreev(groups);
        }
        g_key_file_unref(kf);
    }
    sodium_memzero(plain,plain_len+1);g_free(plain);
    sodium_memzero(contents,len);g_free(contents);
    if(!ok)g_ptr_array_set_size(s.items,0);
    return ok;
}
static GtkWidget *field(GtkWidget *box,const char *caption,const char *placeholder,gboolean secret) {
    GtkWidget *l=gtk_label_new(caption);gtk_widget_set_halign(l,GTK_ALIGN_START);gtk_box_append(GTK_BOX(box),l);
    GtkWidget *e=secret?gtk_password_entry_new():gtk_entry_new();
    if(secret)gtk_password_entry_set_show_peek_icon(GTK_PASSWORD_ENTRY(e),TRUE);
    gtk_editable_set_text(GTK_EDITABLE(e),"");
    if(placeholder) { if(secret)g_object_set(e,"placeholder-text",placeholder,NULL);
                      else gtk_entry_set_placeholder_text(GTK_ENTRY(e),placeholder); }
    gtk_box_append(GTK_BOX(box),e);return e;
}
static GtkWidget *vertical(int gap) {return gtk_box_new(GTK_ORIENTATION_VERTICAL,gap);}
static void margins(GtkWidget *w,int m){gtk_widget_set_margin_top(w,m);gtk_widget_set_margin_bottom(w,m);gtk_widget_set_margin_start(w,m);gtk_widget_set_margin_end(w,m);}
static GtkWidget *page(void){GtkWidget *v=vertical(12);margins(v,24);return v;}
static void refresh(void);
static void detail_click(GtkButton *b,gpointer data) {
    (void)b;Item *i=data;
    GtkWidget *w=gtk_window_new();gtk_window_set_title(GTK_WINDOW(w),i->app);
    gtk_window_set_default_size(GTK_WINDOW(w),420,380);
    gtk_window_set_transient_for(GTK_WINDOW(w),GTK_WINDOW(s.win));
    GtkWidget *v=page();
    const char *names[]={"App / Website","Account type","Username","Email","Phone"};
    const char *vals[]={i->app,i->type,i->username,i->email,i->phone};
    for(int z=0;z<5;z++){char *line=g_strdup_printf("%s: %s",names[z],*vals[z]?vals[z]:"—");
        GtkWidget *lab=gtk_label_new(line);gtk_label_set_wrap(GTK_LABEL(lab),TRUE);gtk_widget_set_halign(lab,GTK_ALIGN_START);
        gtk_box_append(GTK_BOX(v),lab);g_free(line);}
    GtkWidget *p=field(v,"Password (click eye to reveal)",NULL,TRUE);
    gtk_editable_set_text(GTK_EDITABLE(p),i->password);
    gtk_editable_set_editable(GTK_EDITABLE(p),FALSE);
    GtkWidget *close=gtk_button_new_with_label("Close");g_signal_connect_swapped(close,"clicked",G_CALLBACK(gtk_window_destroy),w);
    gtk_box_append(GTK_BOX(v),close);gtk_window_set_child(GTK_WINDOW(w),v);gtk_window_present(GTK_WINDOW(w));
}
static void delete_click(GtkButton *b,gpointer data){(void)b;Item *i=data;
    guint idx=0;while(idx<s.items->len&&g_ptr_array_index(s.items,idx)!=i)idx++;
    if(idx==s.items->len)return;
    /* Save a rollback copy if disk writing fails. */
    g_ptr_array_steal_index(s.items,idx);
    if(!save_vault()){g_ptr_array_insert(s.items,idx,i);alert("Could not save changes. Nothing was deleted.");return;}
    item_free(i);refresh();
}
static void refresh(void){
    GtkWidget *child;
    while((child=gtk_widget_get_first_child(s.list)))gtk_list_box_remove(GTK_LIST_BOX(s.list),child);
    for(guint j=0;j<s.items->len;j++){
        Item *i=g_ptr_array_index(s.items,j);
        GtkWidget *h=gtk_box_new(GTK_ORIENTATION_HORIZONTAL,10);margins(h,7);
        GtkWidget *label=gtk_label_new(i->app);gtk_widget_set_hexpand(label,TRUE);gtk_widget_set_halign(label,GTK_ALIGN_START);
        gtk_box_append(GTK_BOX(h),label);
        GtkWidget *view=gtk_button_new_with_label("View");g_signal_connect(view,"clicked",G_CALLBACK(detail_click),i);gtk_box_append(GTK_BOX(h),view);
        GtkWidget *del=gtk_button_new_with_label("Delete");g_signal_connect(del,"clicked",G_CALLBACK(delete_click),i);gtk_box_append(GTK_BOX(h),del);
        gtk_list_box_append(GTK_LIST_BOX(s.list),h);
    }
    char *msg=g_strdup_printf("%u saved account(s)",s.items->len);
    gtk_label_set_text(GTK_LABEL(s.status),msg);g_free(msg);
}
typedef struct {GtkWidget *win,*app,*type,*user,*email,*phone,*pass;} Form;
static void save_click(GtkButton *b,gpointer data) {
    (void)b;Form *f=data;
    const char *name=gtk_editable_get_text(GTK_EDITABLE(f->app));
    const char *pwd=gtk_editable_get_text(GTK_EDITABLE(f->pass));
    if(!*name||!*pwd){alert("App name and password are required.");return;}
    Item *i=g_new0(Item,1);
    i->app=g_strdup(name);i->type=g_strdup(gtk_editable_get_text(GTK_EDITABLE(f->type)));
    i->username=g_strdup(gtk_editable_get_text(GTK_EDITABLE(f->user)));
    i->email=g_strdup(gtk_editable_get_text(GTK_EDITABLE(f->email)));
    i->phone=g_strdup(gtk_editable_get_text(GTK_EDITABLE(f->phone)));
    i->password=g_strdup(pwd);
    g_ptr_array_add(s.items,i);
    if(!save_vault()) {g_ptr_array_remove_index(s.items,s.items->len-1);alert("Save failed. Check disk permissions.");return;}
    refresh();gtk_window_destroy(GTK_WINDOW(f->win));
}
static void form_free(gpointer data,GObject *obj){(void)obj;g_free(data);}
static void add_click(GtkButton *b,gpointer data){(void)b;(void)data;
    Form *f=g_new0(Form,1);f->win=gtk_window_new();
    gtk_window_set_title(GTK_WINDOW(f->win),"Add Password");gtk_window_set_default_size(GTK_WINDOW(f->win),470,580);
    gtk_window_set_transient_for(GTK_WINDOW(f->win),GTK_WINDOW(s.win));
    GtkWidget *sc=gtk_scrolled_window_new();GtkWidget *v=page();
    f->app=field(v,"App / Website *","Example: GitHub",FALSE);
    f->type=field(v,"Account type","Personal / School / Work",FALSE);
    f->user=field(v,"Username","Optional",FALSE);
    f->email=field(v,"Email","Optional",FALSE);
    f->phone=field(v,"Phone number","Optional",FALSE);
    f->pass=field(v,"Password *","Your password",TRUE);
    GtkWidget *save=gtk_button_new_with_label("Save encrypted password");
    g_signal_connect(save,"clicked",G_CALLBACK(save_click),f);gtk_box_append(GTK_BOX(v),save);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(sc),v);gtk_window_set_child(GTK_WINDOW(f->win),sc);
    g_object_weak_ref(G_OBJECT(f->win),form_free,f);gtk_window_present(GTK_WINDOW(f->win));
}
static void generate_click(GtkButton *b,gpointer data){(void)b;GtkWidget *e=data;
    const char alphabet[]="ABCDEFGHJKLMNPQRSTUVWXYZabcdefghijkmnopqrstuvwxyz23456789!@#$%^&*";
    const size_t n=sizeof alphabet-1;char p[25];
    for(int j=0;j<24;j++)p[j]=alphabet[randombytes_uniform((uint32_t)n)];p[24]=0;
    gtk_editable_set_text(GTK_EDITABLE(e),p);sodium_memzero(p,sizeof p);
}
static void show_main(void){
    GtkWidget *v=page();
    GtkWidget *heading=gtk_label_new(NULL);gtk_label_set_markup(GTK_LABEL(heading),"<span size='xx-large' weight='bold'>Password Safe</span>");
    gtk_widget_set_halign(heading,GTK_ALIGN_START);gtk_box_append(GTK_BOX(v),heading);
    GtkWidget *add=gtk_button_new_with_label("+ Add Password");g_signal_connect(add,"clicked",G_CALLBACK(add_click),NULL);gtk_box_append(GTK_BOX(v),add);
    GtkWidget *h=gtk_label_new("Password List");gtk_widget_set_halign(h,GTK_ALIGN_START);gtk_box_append(GTK_BOX(v),h);
    s.list=gtk_list_box_new();gtk_list_box_set_selection_mode(GTK_LIST_BOX(s.list),GTK_SELECTION_NONE);
    GtkWidget *sc=gtk_scrolled_window_new();gtk_widget_set_vexpand(sc,TRUE);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(sc),s.list);gtk_box_append(GTK_BOX(v),sc);
    s.status=gtk_label_new("");gtk_widget_set_halign(s.status,GTK_ALIGN_START);gtk_box_append(GTK_BOX(v),s.status);
    GtkWidget *tips=gtk_label_new("Create strong passwords: 16+ random characters, unique for each account.\nUse a long, memorable master passphrase. Enable 2FA when available.");
    gtk_label_set_wrap(GTK_LABEL(tips),TRUE);gtk_widget_set_halign(tips,GTK_ALIGN_START);gtk_box_append(GTK_BOX(v),tips);
    GtkWidget *gen=gtk_expander_new("How to Create a Password / Generator");GtkWidget *inside=vertical(8);
    GtkWidget *entry=gtk_password_entry_new();gtk_password_entry_set_show_peek_icon(GTK_PASSWORD_ENTRY(entry),TRUE);
    gtk_box_append(GTK_BOX(inside),entry);
    GtkWidget *btn=gtk_button_new_with_label("Generate 24-character password");g_signal_connect(btn,"clicked",G_CALLBACK(generate_click),entry);
    gtk_box_append(GTK_BOX(inside),btn);gtk_expander_set_child(GTK_EXPANDER(gen),inside);gtk_box_append(GTK_BOX(v),gen);
    gtk_window_set_child(GTK_WINDOW(s.win),v);refresh();
}
static void unlock_click(GtkButton *b,gpointer data){(void)b;GtkWidget *entry=data;
    const char *master=gtk_editable_get_text(GTK_EDITABLE(entry));
    if(strlen(master)<12){alert("Master password must be at least 12 characters.");return;}
    gboolean exists=g_file_test(s.path,G_FILE_TEST_EXISTS);
    if(exists){
        gchar *file=NULL;gsize len=0;
        if(!g_file_get_contents(s.path,&file,&len,NULL)||len<MAGIC_LEN+SALT_LEN+NONCE_LEN+MAC_LEN||memcmp(file,MAGIC,MAGIC_LEN)){
            g_free(file);alert("Vault is missing, damaged, or has an invalid format.");return;
        }
        memcpy(s.salt,file+MAGIC_LEN,SALT_LEN);g_free(file);
    } else randombytes_buf(s.salt,SALT_LEN);
    if(!derive(master)){alert("Could not derive encryption key.");return;}
    if(exists && !load_vault()){sodium_memzero(s.key,sizeof s.key);alert("Wrong master password or damaged vault.");return;}
    if(!exists&&!save_vault()){sodium_memzero(s.key,sizeof s.key);alert("Could not create vault.");return;}
    s.unlocked=TRUE;show_main();
}
static void activated(GtkApplication *app,gpointer user_data){(void)user_data;
    s.win=gtk_application_window_new(app);gtk_window_set_title(GTK_WINDOW(s.win),"Password Safe");
    gtk_window_set_default_size(GTK_WINDOW(s.win),650,680);
    GtkWidget *v=page();
    GtkWidget *title=gtk_label_new("Password Safe — encrypted local vault");gtk_box_append(GTK_BOX(v),title);
    gboolean exists=g_file_test(s.path,G_FILE_TEST_EXISTS);
    GtkWidget *desc=gtk_label_new(exists?"Unlock with your master password":"First launch: create a NEW master password (12+ characters)");
    gtk_label_set_wrap(GTK_LABEL(desc),TRUE);gtk_box_append(GTK_BOX(v),desc);
    GtkWidget *pass=field(v,"Master password",NULL,TRUE);
    GtkWidget *button=gtk_button_new_with_label(exists?"Unlock":"Create vault");
    g_signal_connect(button,"clicked",G_CALLBACK(unlock_click),pass);gtk_box_append(GTK_BOX(v),button);
    gtk_window_set_child(GTK_WINDOW(s.win),v);gtk_window_present(GTK_WINDOW(s.win));
}
int main(int argc,char **argv){
    if(sodium_init()<0){g_printerr("libsodium initialization failed\n");return 1;}
    s.items=g_ptr_array_new_with_free_func(item_free);
    char *dir=g_build_filename(g_get_user_data_dir(),"password-safe",NULL);
    if(g_mkdir_with_parents(dir,0700)!=0){g_printerr("Cannot create data directory\n");g_free(dir);return 1;}
    s.path=g_build_filename(dir,"vault.bin",NULL);g_free(dir);
    GtkApplication *app=gtk_application_new("org.example.passwordsafe",G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app,"activate",G_CALLBACK(activated),NULL);
    int rc=g_application_run(G_APPLICATION(app),argc,argv);
    g_object_unref(app);g_ptr_array_free(s.items,TRUE);
    sodium_memzero(s.key,sizeof s.key);g_free(s.path);return rc;
}
