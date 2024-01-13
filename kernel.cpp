#include <gtk/gtk.h>
#include <iostream>
#include <cstdlib>

// ELF 파일 헤더 구조체
typedef struct {
    uint8_t  e_ident[16];
    uint16_t e_type;
    uint16_t e_machine;
    uint32_t e_version;
    uint32_t e_entry;
    uint32_t e_phoff;
    uint32_t e_shoff;
    uint32_t e_flags;
    uint16_t e_ehsize;
    uint16_t e_phentsize;
    uint16_t e_phnum;
    uint16_t e_shentsize;
    uint16_t e_shnum;
    uint16_t e_shstrndx;
} ElfHeader;

// ELF 프로그램 헤더 구조체
typedef struct {
    uint32_t p_type;
    uint32_t p_offset;
    uint32_t p_vaddr;
    uint32_t p_paddr;
    uint32_t p_filesz;
    uint32_t p_memsz;
    uint32_t p_flags;
    uint32_t p_align;
} ProgramHeader;

extern "C" void __libc_init_array(void); // C++ 런타임 초기화 함수
extern "C" int main(int argc, char **argv); // C++ main 함수

// "끄기" 명령을 처리하는 함수
extern "C" void handle_shutdown_request() {
    std::cout << "운영체제를 종료합니다." << std::endl;
    
    // 종료 작업을 수행한 후 시스템 종료
    std::exit(0);
}

void loadElf(const char *filename, uint32_t loadAddress) {
    FILE *file = fopen(filename, "rb");
    if (!file) {
        std::cout << "파일을 열 수 없습니다." << std::endl;
        return;
    }

    ElfHeader elfHeader;
    fread(&elfHeader, sizeof(ElfHeader), 1, file);

    if (memcmp(elfHeader.e_ident, "\x7F\x45\x4C\x46", 4) != 0) {
        std::cout << "유효한 ELF 파일이 아닙니다." << std::endl;
        fclose(file);
        return;
    }

    if (elfHeader.e_type != 2) {
        std::cout << "실행 파일이 아닙니다." << std::endl;
        fclose(file);
        return;
    }

    fseek(file, elfHeader.e_phoff, SEEK_SET);

    for (int i = 0; i < elfHeader.e_phnum; i++) {
        ProgramHeader programHeader;
        fread(&programHeader, sizeof(ProgramHeader), 1, file);

        if (programHeader.p_type == 1) {
            fseek(file, programHeader.p_offset, SEEK_SET);
            fread((void *)loadAddress, 1, programHeader.p_filesz, file);
        }
    }

    fclose(file);
}

// GTK+ 윈도우 생성 및 설정
static void activate(GtkApplication *app, gpointer user_data) {
    GtkWidget *window;

    // 윈도우 생성
    window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "My Application");
    gtk_window_set_default_size(GTK_WINDOW(window), 200, 200);

    // "끄기" 버튼 추가
    GtkWidget *button = gtk_button_new_with_label("끄기");
    g_signal_connect(button, "clicked", G_CALLBACK(handle_shutdown_request), NULL);

    // 레이아웃 설정
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_box_pack_start(GTK_BOX(box), button, TRUE, TRUE, 0);
    gtk_container_add(GTK_CONTAINER(window), box);

    gtk_widget_show_all(window);
}

int main(int argc, char **argv) {
    // GTK+ 응용 프로그램 초기화
    GtkApplication *app;
    int status;

    app = gtk_application_new("org.example.myapp", G_APPLICATION_FLAGS_NONE);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);

    return status;
}
