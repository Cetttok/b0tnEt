#ifndef CRYPTER_H
#define CRYPTER_H

#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/bio.h>
#include <openssl/err.h>
#include <openssl/rand.h>
// тут много самих функций через gpt было
/* ключи
const char * PATH_TO_RSA_PUBLIC_KEY =  "keys/public_key.pem";
const char * PATH_TO_RSA_PRIVATE_KEY =  "keys/private_key.pem";
*/
enum CryptManagerLoadedRsaKeysState{
    PUBLIC, PRIVATE, ALL, NONE
};

class CryptManager // типо метод с кучей методотов к ооп мало отношения имеет
{
public:
    CryptManagerLoadedRsaKeysState getRsaKeysState();

    //aes crypt for random key
    static CryptManager  * getCryptManager();

    std::string genAesRandomKey();
    std::string cryptAes(std::string data, std::string & key);
    std::string decryptAes(std::string data, std::string & key);


    std::string genSha512Hash(std::string data);
    bool compareWithSha512Hash(std::string data, std::string hash);

    //rsa crypt with private key
    void loadRsaPublicKey(std::string file);
    void loadRsaPrivateKey(std::string file);
    std::string cryptRsaPrivate(std::string data);
    std::string decryptRsaPublic(std::string data);


    //base64 decode
    std::string toBase(std::string data);
    std::string fromBase(std::string base);
private:
    static CryptManager * _manager;
    CryptManager();

    CryptManagerLoadedRsaKeysState _state = CryptManagerLoadedRsaKeysState::NONE;
    void reloadKeysRsaState();

    //rsa
    RSA* _publicKeyRsa = nullptr;
    //std::string _rsaPublicKey = "";

    RSA* _privateKeyRsa = nullptr;
    //std::string _rsaPrivateKey = "";
};
class Crypter // сердце криптографии
// тут как шифровать и чем устонавливается
{
public:
    Crypter();
    CryptManagerLoadedRsaKeysState load();
    std::string crypt(std::string data);
    bool checkValidity(std::string data);
    std::string decrypt(std::string crypted);
private:
    CryptManager * _manager;
};

#endif // CRYPTER_H
