#ifndef AESDECODER_H
#define AESDECODER_H

#include <QByteArray>

class AESDecoder {

public:
    static int Decode(void* buffer, int buffer_len, char* IV, char* key, int key_len);

    static bool Decode(QByteArray crypted, QByteArray key, QByteArray &plain);

};

#endif // AESDECODER_H
