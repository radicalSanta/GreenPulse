#include "CircularBuffer.h"

CircularBuffer::CircularBuffer(float *storage, int bufferSize)
{
    buffer = storage;
    size = bufferSize;
    index = 0;
    count = 0;
}

void CircularBuffer::add(float value)
{
    buffer[index] = value;

    index++;

    if (index >= size)
    {
        index = 0;
    }

    if (count < size)
    {
        count++;
    }
}

float CircularBuffer::average()
{
    if (count == 0)
    {
        return 0;
    }

    float sum = 0;

    for (int i = 0; i < count; i++)
    {
        sum += buffer[i];
    }

    return sum / count;
}

bool CircularBuffer::isFull()
{
    return count == size;
}