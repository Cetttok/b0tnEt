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

// class Crypter // abstract
// {
// public:
//     Crypter( Crypter * child = nullptr);
//     virtual std::string crypt(std::string & data, std::string & key);
//     virtual std::string decrypt(std::string & data, std::string & key);

// protected:
//     Crypter * _child = nullptr;
// };

// class Base64Crypter : public Crypter
// {
// public:
//     Base64Crypter( Crypter * child = nullptr);
//     virtual std::string crypt(std::string & data, std::string & key);
//     virtual std::string decrypt(std::string & data, std::string & key);
// };

// class RsaCrypter : public Crypter
// {
// public:
//     RsaCrypter( Crypter * child = nullptr);
//     virtual std::string crypt(std::string & data, std::string & key);
//     virtual std::string decrypt(std::string & data, std::string & key);
// };

// class AesCrypter : public Crypter
// {
// public:
//     AesCrypter( Crypter * child = nullptr);
//     virtual std::string crypt(std::string & data, std::string & key);
//     virtual std::string decrypt(std::string & data, std::string & key);
// };

// class RsaPrivateCrypter : public Crypter
// {
// public:
//     RsaPrivateCrypter( Crypter * child = nullptr);
//     virtual std::string crypt(std::string & data, std::string & key);
//     virtual std::string decrypt(std::string & data, std::string & key);
// };
enum CryptManagerLoadedRsaKeysState{
    PUBLIC, PRIVATE, ALL, NONE
};

class CryptManager
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
class Crypter
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
