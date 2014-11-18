#include "aesdecoder.h"

#include <mcrypt.h>

namespace fdp {
namespace crypt {

bool AESDecoder::DecodeCNL(const QByteArray crypted, const QByteArray key, QByteArray &plain) {
    char buffer[crypted.length()];
    for(int i=0; i<crypted.length(); i++)
        buffer[i] = crypted[i];
    bool returnValue = AES128CBC(buffer, crypted.length(), key.data(), key.length(), key.data(), key.length());
    if(returnValue)
        plain = QByteArray(buffer, crypted.length());
    return returnValue;
}

bool AESDecoder::AES128CBC(char *buffer, const int bufferLength, const char *key, const int keyLength, const char *iv, const int ivLength) {
    // open mcrypt module
    MCRYPT td = mcrypt_module_open((char *)"rijndael-128", NULL, (char *)"cbc", NULL);
    if(td == MCRYPT_FAILED)
        return false;
    // check cipher size
    int blocksize = mcrypt_enc_get_block_size(td);
    if(bufferLength % blocksize)
        return false;
    // prepare key
    char *sizedKey = new char[keyLength];
    for(int i=0; i<keyLength; i++)
        sizedKey[i] = key[i];
    // prepare iv
    char *sizedIv = new char[mcrypt_enc_get_iv_size(td)];
    for(int i=0; i<mcrypt_enc_get_iv_size(td); i++) {
        if(i<ivLength)
            sizedIv[i] = iv[i];
        else
            sizedIv[i] = 0;
    }
    // decrypt
    mcrypt_generic_init(td, sizedKey, keyLength, sizedIv);
    mdecrypt_generic(td, buffer, bufferLength);
    mcrypt_generic_deinit (td);
    // close module
    mcrypt_module_close(td);
    delete sizedKey;
    delete sizedIv;
    return true;
}



} // end of namespace crypt
} // end of namespace fdp
