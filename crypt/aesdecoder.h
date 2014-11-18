#ifndef AESDECODER_H
#define AESDECODER_H

#include <QByteArray>

namespace fdp {
namespace crypt {

class AESDecoder {

public:
    static bool DecodeCNL(const QByteArray crypted, const QByteArray key, QByteArray &plain);
    static bool AES128CBC(char* buffer, const int bufferLength, const char *key, const int keyLength, const char* iv, const int ivLength);

};

} // end of namespace crypt
} // end of namespace fdp

#endif // AESDECODER_H
