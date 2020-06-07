#include <stdlib.h>
#include "SDL2/SDL.h"

#include "./gbSerializer_sys.h"

#include "./gbFile_type.h"
#include "./gbFileChunkSize_type.h"

// Forward declarations
void gbSerializerWriteChunk(gbFile *file, GB_FILE_CHUNK_SIZE chunkSize, uint64_t chunk);
uint64_t gbSerializerReadChunk(gbFile *file, GB_FILE_CHUNK_SIZE chunkSize);

gbFile *gbSerializerOpenFileWrite(char *filename) {
    SDL_RWops *ops = SDL_RWFromFile(filename, "wb");

    if (!ops) { return 0; }

    gbFile *file = (gbFile *)malloc(sizeof(gbFile));

    file->endianness = SDL_BYTEORDER == SDL_BIG_ENDIAN ? GB_ENDIAN_BIG : GB_ENDIAN_LITTLE;
    file->file = ops;

    // Indicate endianness at beginning file
    gbSerializerWriteChunk(file, GB_FILE_CHUNK_SIZE_8, file->endianness);

    return file;
}

gbFile *gbSerializerOpenFileRead(char *filename) {
    SDL_RWops *ops = SDL_RWFromFile(filename, "rb");

    if (!ops) { return 0; }

    gbFile *file = (gbFile *)malloc(sizeof(gbFile));

    file->endianness = SDL_ReadU8(ops) != GB_ENDIAN_BIG ? GB_ENDIAN_LITTLE : GB_ENDIAN_BIG;
    file->file = ops;

    return file;
}

void gbSerializerCloseFile(gbFile *file) {
    SDL_RWclose(file->file);
    free(file);
}

void gbSerializerWriteChunk(gbFile *file, GB_FILE_CHUNK_SIZE chunkSize, uint64_t chunk) {
    switch (chunkSize) {
        case GB_FILE_CHUNK_SIZE_8:
            SDL_WriteU8(file->file, chunk);
            break;
        case GB_FILE_CHUNK_SIZE_16:
            if (file->endianness == GB_ENDIAN_BIG)
                SDL_WriteBE16(file->file, chunk);
            else
                SDL_WriteLE16(file->file, chunk);
            break;
        case GB_FILE_CHUNK_SIZE_32:
            if (file->endianness == GB_ENDIAN_BIG)
                SDL_WriteBE32(file->file, chunk);
            else
                SDL_WriteLE32(file->file, chunk);
            break;
        case GB_FILE_CHUNK_SIZE_64:
            if (file->endianness == GB_ENDIAN_BIG)
                SDL_WriteBE32(file->file, chunk);
            else
                SDL_WriteLE32(file->file, chunk);
            break;
    }
}
uint64_t gbSerializerReadChunk(gbFile *file, GB_FILE_CHUNK_SIZE chunkSize) {
    uint64_t chunk;

    switch (chunkSize) {
        case GB_FILE_CHUNK_SIZE_8:
            chunk = SDL_ReadU8(file->file);
            break;
        case GB_FILE_CHUNK_SIZE_16:
            if (file->endianness == GB_ENDIAN_BIG)
                chunk = SDL_ReadBE16(file->file);
            else
                chunk = SDL_ReadLE16(file->file);
            break;
        case GB_FILE_CHUNK_SIZE_32:
            if (file->endianness == GB_ENDIAN_BIG)
                chunk = SDL_ReadBE32(file->file);
            else
                chunk = SDL_ReadLE32(file->file);
            break;
        case GB_FILE_CHUNK_SIZE_64:
            if (file->endianness == GB_ENDIAN_BIG)
                chunk = SDL_ReadBE32(file->file);
            else
                chunk = SDL_ReadLE32(file->file);
            break;
    }

    return chunk;
}
