#ifndef CIRCULAR_BUFFER_H
#define CIRCULAR_BUFFER_H

class CircularBuffer
{
private:
    float *buffer;
    int size;
    int index;
    int count;

public:
    CircularBuffer(float *storage, int bufferSize);

    void add(float value);

    float average();

    bool isFull();
};

#endif