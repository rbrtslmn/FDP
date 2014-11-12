#ifndef AESDECODER_H
#define AESDECODER_H

#include <QByteArray>

namespace fdp {
namespace crypt {

class AESDecoder {

public:
    static int Decode(void* buffer, int buffer_len, char* IV, char* key, int key_len);
    static bool Decode(QByteArray crypted, QByteArray key, QByteArray &plain);

};

} // end of namespace crypt
} // end of namespace fdp

#endif // AESDECODER_H
