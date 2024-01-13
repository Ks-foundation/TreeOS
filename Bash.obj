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

int main() {
    const char *filename = "Bash.obj";
    uint32_t loadAddress = 0x10000000;

    loadElf(filename, loadAddress);

    // C++ 런타임 초기화
    __libc_init_array();

    // main 함수 호출
    int argc = 1; // 예제에서는 argc를 1로 설정
    char *argv[2] = {const_cast<char *>("Bash.obj"), nullptr}; // 예제에서는 argv[0]에 프로그램 이름을 설정
    int result = main(argc, argv);

    return result;
}
