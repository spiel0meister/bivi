#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#define ERROR(...) fprintf(stderr, __VA_ARGS__)

uint8_t* read_file(const char* path, int* size) {
    FILE* f = fopen(path, "rb");
    if (f == NULL) {
        ERROR("Could't open file %s: %s\n", path, strerror(errno));
        exit(1);
    }

    fseek(f, 0, SEEK_END);
    int n = ftell(f);
    fseek(f, 0, SEEK_SET);

    uint8_t* contents = malloc(sizeof(uint8_t) * n);
    fread(contents, 1, n, f);

    *size = n;
    return contents;
}

char* pop_argv(int* argc, char*** argv) {
    assert(*argc > 0);
    char* arg = **argv;
    *argv += 1;
    *argc -= 1;
    return arg;
}

static uint32_t freq[256][256] = {0};
static uint32_t pixels[256][256] = {0};

int main(int argc, char** argv) {
    const char* program = pop_argv(&argc, &argv);

    if (argc < 1) {
        ERROR("Usage: %s <file...>\n", program);
        exit(1);
    }

    while (argc > 0) {
        memset(freq, 0, sizeof(**freq) * 256 * 256);
        memset(pixels, 0, sizeof(**pixels) * 256 * 256);

        const char* path = pop_argv(&argc, &argv);

        int size = 0;
        uint8_t* contents = read_file(path, &size);
        printf("%s: %d bytes\n", path, size);

        for (int i = 0; i < size - 1; ++i) {
            uint8_t x = contents[i];
            uint8_t y = contents[i + 1];

            freq[y][x] += 1;
        }

        uint32_t max = 0;
        for (int i = 0; i < 256; ++i) {
            for (int j = 0; j < 256; ++j) {
                if (freq[j][i] == 0) continue;
                uint32_t t = logf(freq[j][i]);
                if (max < t) max = t;
            }
        }

        assert(max != 0);
        for (int i = 0; i < 256; ++i) {
            for (int j = 0; j < 256; ++j) {
                if (freq[j][i] == 0) {
                    pixels[j][i] = 0xFF000000;
                } else {
                    float t = logf((float)freq[j][i])/max;
                    uint8_t bright = t * 255;
                    pixels[j][i] = (255 << 24) | (bright << 16) | (bright << 8) | (bright << 0);
                }
            }
        }

        char buf[1024] = {0};
        snprintf(buf, 1024, "%s.bivi.png", path);
        stbi_write_png(buf, 256, 256, 4, pixels, sizeof(uint32_t) * 256);
    }

    return 0;
}
