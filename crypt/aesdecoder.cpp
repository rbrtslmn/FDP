#include "aesdecoder.h"

#include <stdio.h>
#include <mcrypt.h>

#include <QDebug>

namespace fdp {
namespace crypt {

int AESDecoder::Decode(void* buffer, int buffer_len, char* IV, char* key, int key_len) {
    MCRYPT td = mcrypt_module_open("rijndael-128", NULL, "cbc", NULL);
    int blocksize = mcrypt_enc_get_block_size(td);
    if(buffer_len%blocksize != 0)
      return 1;
    mcrypt_generic_init(td, key, key_len, IV);
    mdecrypt_generic(td, buffer, buffer_len);
    mcrypt_generic_deinit (td);
    mcrypt_module_close(td);
    return 0;
}

bool AESDecoder::Decode(QByteArray crypted, QByteArray key, QByteArray &plain) {
    char buffer[crypted.length()];
    char iv[key.length()];
    for(int i=0; i<crypted.length(); i++)
        buffer[i] = crypted[i];
    for(int i=0; i<key.length(); i++)
        iv[i] = key[i];
    int retVal = Decode(buffer, crypted.length(), iv, iv, key.length());
    if(!retVal) {
        plain = QByteArray(buffer, crypted.length());
        return true;
    }
    return false;
}

} // end of namespace crypt
} // end of namespace fdp
