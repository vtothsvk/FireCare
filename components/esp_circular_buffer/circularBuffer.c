#include "circularBuffer.h"

void circularBufferReset(circular_buf_t* cBuffer){
    //reset head and tail
    cBuffer -> head = 0;
    cBuffer -> tail = 0;
}//reset

bool circularBufferFull(circular_buf_t* cBuffer){
    return ((cBuffer -> head + 1) % cBuffer -> max == cBuffer -> tail);
}//isFull

bool circularBufferEmpty(circular_buf_t* cBuffer){
    return (cBuffer -> head == cBuffer -> tail);
}//isEmpty

esp_err_t circularBufferInit(uint8_t* buffer, size_t size, circular_buf_t* cBuffer){
    esp_err_t ret = ESP_OK;
    
    //init internal buffer
    cBuffer -> buffer = buffer;
    //save size config
    cBuffer -> max = size;
    
    //reset
    circularBufferReset(cBuffer);

    return ret;
}//init

esp_err_t circularBufferWrite(circular_buf_t* cBuffer, uint8_t* data){
    esp_err_t ret = ESP_FAIL;

    //check if full
    if (!circularBufferFull(cBuffer)) {
        //write data to buffer[head * BUFFER_BLOCK]
        memcpy(cBuffer -> buffer + (cBuffer -> head * BUFFER_BLOCK), data, BUFFER_BLOCK);
        ret = ESP_OK;
    }//if (!circularBufferFull(cBuffer))

    //update head
    cBuffer -> head = (cBuffer -> head + 1) % cBuffer -> max;

    return ret;
}//write

esp_err_t circularBufferRead(circular_buf_t* cBuffer, uint8_t* data){

    //check if empty
    if (circularBufferEmpty(cBuffer)) {
        return ESP_FAIL;
    }//if (circularBufferEmpty(cBuffer)) {

    //read data from buffer[tail * BUFFER_BLOCK]
    memcpy(data, cBuffer -> buffer + (cBuffer -> tail * BUFFER_BLOCK), BUFFER_BLOCK);

    //update tail
    cBuffer -> tail = (cBuffer -> tail + 1) % cBuffer -> max;

    return ESP_OK;
}//read