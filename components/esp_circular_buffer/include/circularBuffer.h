#ifndef circularBuffer_h
#define circularBuffer_h

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <string.h>
#include "esp_err.h"

#define BUFFER_BLOCK    5

typedef struct circular_buffer{
    uint8_t* buffer;
    size_t head;
    size_t tail;
    size_t max;
}circular_buf_t;

esp_err_t circularBufferInit(uint8_t* buffer, size_t size, circular_buf_t* cBuffer);
esp_err_t circularBufferFree(circular_buf_t* cBuffer);
void circularBufferReset(circular_buf_t* cBuffer);
esp_err_t circularBufferWrite(circular_buf_t* cBuffer, uint8_t* data);
esp_err_t circularBufferRead(circular_buf_t* cBuffer, uint8_t* data);
bool circularBufferFull(circular_buf_t* cBuffer);
bool circularBufferEmpty(circular_buf_t* cBuffer);

#ifdef __cplusplus
}
#endif

#endif