#ifndef GB_SERIALIZER_H_INCLUDED
#define GB_SERIALIZER_H_INCLUDED

#include "./gbFile_type.h"
#include "./gbFileChunkSize_type.h"

gbFile *gbSerializerOpenFileWrite(char *filename);
gbFile *gbSerializerOpenFileRead(char *filename);
void gbSerializerCloseFile(gbFile *file);

void gbSerializerWriteChunk(gbFile *file, GB_FILE_CHUNK_SIZE chunkSize, uint64_t chunk);
uint64_t gbSerializerReadChunk(gbFile *file, GB_FILE_CHUNK_SIZE chunkSize);

#endif // GB_SERIALIZER_H_INCLUDED
