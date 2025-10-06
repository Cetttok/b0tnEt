#include "crypter.h"
CryptManager * CryptManager ::_manager = nullptr;

const int AES_RANDOM_KEY_SIZE= 32;
const int AES_ADDITIONAL_PADDING_SIZE = 16;

const char * PATH_TO_RSA_PUBLIC_KEY =  "keys/public_key.pem";
const char * PATH_TO_RSA_PRIVATE_KEY =  "keys/private_key.pem";

const char KEY_AND_DATA_DELIMER = ':';
// Crypter::Crypter( Crypter * child ) {}

// std::string Crypter::crypt(std::string &data, std::string &key)
// {
//     if (_child == nullptr){
//         return data;
//     }
//     else return _child->crypt(data,key);
// }

// std::string Crypter::decrypt(std::string &data, std::string &key)
// {
//     if (_child == nullptr){
//         return data;
//     }
//     else return _child->decrypt(data,key);
// }

// std::string Base64Crypter::crypt(std::string &data, std::string &key)
// {
//     if (_child=nullptr )
// }

CryptManagerLoadedRsaKeysState CryptManager::getRsaKeysState()
{
    return _state;
}

CryptManager *CryptManager::getCryptManager()
{
    if (_manager == nullptr){
        _manager = new CryptManager();
        return _manager;
    }
    else{
        return _manager;
    }
}

std::string CryptManager::genAesRandomKey()
{
    std::string key(AES_RANDOM_KEY_SIZE, 0);
    RAND_bytes((unsigned char*)key.data(), key.size());
    return key;
}

std::string CryptManager::cryptAes(std::string data, std::string &key)
{
    unsigned char iv[AES_ADDITIONAL_PADDING_SIZE];
    RAND_bytes(iv, sizeof(iv));

    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL,
                       (const unsigned char*)key.data(), iv);

    std::vector<unsigned char> ciphertext(data.size() +  AES_RANDOM_KEY_SIZE);
    int len = 0, total_len = 0;

    EVP_EncryptUpdate(ctx, ciphertext.data(), &len,
                      (const unsigned char*)data.data(), data.size());
    total_len = len;

    EVP_EncryptFinal_ex(ctx, ciphertext.data() + len, &len);
    total_len += len;

    EVP_CIPHER_CTX_free(ctx);

    std::string result;
    result.append((char*)iv, sizeof(iv));
    result.append((char*)ciphertext.data(), total_len);
    return result;
}

std::string CryptManager::decryptAes(std::string data, std::string &key)
{
    if (data.size() < AES_ADDITIONAL_PADDING_SIZE ) {
        std::cout << "CryptManager. WARN! null message crypted!" << std::endl;
        return "";
    }
    unsigned char iv[AES_ADDITIONAL_PADDING_SIZE ];
    memcpy(iv, data.data(), sizeof(iv));

    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL,
                       (const unsigned char*)key.data(), iv);

    std::vector<unsigned char> plaintext(data.size() - AES_ADDITIONAL_PADDING_SIZE + AES_RANDOM_KEY_SIZE);
    int len = 0, total_len = 0;

    EVP_DecryptUpdate(ctx, plaintext.data(), &len,
                      (const unsigned char*)data.data() + AES_ADDITIONAL_PADDING_SIZE,
                      data.size() - AES_ADDITIONAL_PADDING_SIZE);
    total_len = len;

    EVP_DecryptFinal_ex(ctx, plaintext.data() + len, &len);
    total_len += len;

    EVP_CIPHER_CTX_free(ctx);

    return std::string((char*)plaintext.data(), total_len);
}

void CryptManager::loadRsaPublicKey(std::string file)
{
    std::ifstream input(file);
    if (!input.is_open()){
        std::cout << "Crypt manager. Cant open file = " << file << std::endl;
    }

    std::stringstream buffer;
    buffer << input.rdbuf();
    std::string key = buffer.str();
    if (key == ""){
        std::cout << "Crypt manager. Bad key it null" << std::endl;
    }

    //std::cout << "CryptManager. Public key readed! " << std::endl;
    BIO* bio = BIO_new_mem_buf(key.data(), key.size());
    _publicKeyRsa = PEM_read_bio_RSA_PUBKEY(bio, NULL, NULL, NULL);
    BIO_free(bio);
    if (!_publicKeyRsa ) {
        std::cout << "CryptManager. Error init rsa " << std::endl;
        _publicKeyRsa = nullptr;
    }
    else{
      //  std::cout << "CryptManager. Succes init public rsa key!" << std::endl;
    }

    reloadKeysRsaState();
}

void CryptManager::loadRsaPrivateKey(std::string file)
{
    std::ifstream input(file);
    std::stringstream buffer;
    buffer << input.rdbuf();
    std::string key = buffer.str();
    if (key == ""){
        std::cout << "Crypt manager. Bad key it null" << std::endl;
    }
   // std::cout << "CryptManager. Private key readed! " << std::endl;
    BIO* bio = BIO_new_mem_buf(key.data(), key.size());
    _privateKeyRsa = PEM_read_bio_RSAPrivateKey(bio, NULL, NULL, NULL);
    BIO_free(bio);
    if (!_privateKeyRsa ){
        std::cout << "CryptManager. Failed to init private rsa key" << std::endl;
        _privateKeyRsa=nullptr;
    }
    else{
        //std::cout << "CryptManager. Succes init private rsa key!" << std::endl;
    }

    reloadKeysRsaState();
}

std::string CryptManager::cryptRsaPrivate(std::string data)
{

    if (_state== CryptManagerLoadedRsaKeysState::NONE |  _state  == CryptManagerLoadedRsaKeysState::PUBLIC){
        std::cout << "CryptManager. Cant crypt because private key not loaded" << std::endl;
        return "";
    }
    std::string encrypted(RSA_size(_privateKeyRsa), 0);
    int result = RSA_private_encrypt(
        data.size(),
        (const unsigned char*)data.c_str(),
        (unsigned char*)encrypted.data(),
        _privateKeyRsa,
        RSA_PKCS1_PADDING  // Обычно используют PKCS1 для подписи
        );

    //RSA_free(rsa);

    if (result == -1){
        std::cout << "CryptManager. Error while crypting" << std::endl;
        return "";
    }
    encrypted.resize(result);
    return encrypted;
}

std::string CryptManager::decryptRsaPublic(std::string data)
{
    // BIO* bio = BIO_new_mem_buf(_publicKeyRsa.data(), _rsaPublicKey.size());
    // RSA* rsa = PEM_read_bio_RSA_PUBKEY(bio, NULL, NULL, NULL);  // Читаем публичный ключ
    // BIO_free(bio);

    // if (!rsa) {
    //     std::cout << "CryptManager. Error init rsa public decrypt" << std::endl;
    //     return "";
    // }

    if (_state== CryptManagerLoadedRsaKeysState::NONE |  _state  == CryptManagerLoadedRsaKeysState::PRIVATE){
        std::cout << "CryptManager. Cant crypt because public key not loaded" << std::endl;
        return "";
    }
    std::string decrypted(RSA_size(_publicKeyRsa), 0);
    int result = RSA_public_decrypt(  // Используем функцию для публичного ключа
        data.size(),
        (const unsigned char*)data.c_str(),
        (unsigned char*)decrypted.data(),
        _publicKeyRsa,
        RSA_PKCS1_PADDING  // Должен совпадать с padding при шифровании
        );

    //RSA_free(rsa);

    if (result == -1) {
        std::cout << "CryptManager. Erro while ecrypting rsa with public key" << std::endl;
        return "";
    }
    decrypted.resize(result);
    return decrypted;
}

std::string CryptManager::toBase(std::string data)
{
    BIO *bio, *b64;
    BUF_MEM *bufferPtr;

    // Создаем цепочку BIO: base64 -> memory
    b64 = BIO_new(BIO_f_base64());
    bio = BIO_new(BIO_s_mem());
    bio = BIO_push(b64, bio);

    // Не добавлять переносы строк
    BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL);

    // Кодируем данные
    BIO_write(bio, data.data(), data.length());
    BIO_flush(bio);

    // Получаем результат из memory BIO
    BIO_get_mem_ptr(bio, &bufferPtr);

    // Копируем в строку
    std::string encoded(bufferPtr->data, bufferPtr->length);

    // Очищаем
    BIO_free_all(bio);

    return encoded;
}

std::string CryptManager::fromBase(std::string base)
{
    BIO *bio, *b64;

    // Выделяем буфер для результата
    std::vector<char> buffer(base.length());

    // Создаем цепочку BIO: memory -> base64
    b64 = BIO_new(BIO_f_base64());
    bio = BIO_new_mem_buf(base.data(), base.length());
    bio = BIO_push(b64, bio);

    // Не ожидать переносы строк
    BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL);

    // Декодируем
    int decoded_length = BIO_read(bio, buffer.data(), base.length());

    // Очищаем
    BIO_free_all(bio);

    if (decoded_length > 0) {
        return std::string(buffer.data(), decoded_length);
    }
    std::cout << "CryptManager. Cant decrypt base64 string" << std::endl;
    return "";
}

CryptManager::CryptManager()
{
    //std::cout << "CryptManager. CryptManagerInit complected." << std::endl;
}

void CryptManager::reloadKeysRsaState()
{
    if (_publicKeyRsa == nullptr){
        if (_privateKeyRsa == nullptr){
            _state = CryptManagerLoadedRsaKeysState::NONE;
        }
        else{
            _state =  CryptManagerLoadedRsaKeysState::PRIVATE;
        }
    }
    else{
        if (_privateKeyRsa == nullptr){
            _state = CryptManagerLoadedRsaKeysState::PUBLIC;
        }
        else{
            _state = CryptManagerLoadedRsaKeysState::ALL;
        }
    }
}

std::string toString(CryptManagerLoadedRsaKeysState from){
    switch (from){
    case CryptManagerLoadedRsaKeysState::ALL:
        return "ALL KEYS LOADED";
    case CryptManagerLoadedRsaKeysState::NONE:
        return "KEYS NOT LOADED";
    case CryptManagerLoadedRsaKeysState::PRIVATE:
        return "LOADED ONLY PRIVATE KEY";
    case CryptManagerLoadedRsaKeysState::PUBLIC:
        return "LOADED ONLY PUBLIC KEY";

    }
}
Crypter::Crypter()
{
    _manager = CryptManager::getCryptManager();
    std::cout << "Crypter. init done. working with "  << "STATUS: " << toString(load()) << std::endl;

}

CryptManagerLoadedRsaKeysState Crypter::load()
{
    _manager->loadRsaPrivateKey(PATH_TO_RSA_PRIVATE_KEY);
    _manager->loadRsaPublicKey(PATH_TO_RSA_PUBLIC_KEY);
    return _manager->getRsaKeysState();
}

std::string Crypter::crypt(std::string data)
{
    if (_manager->getRsaKeysState() == CryptManagerLoadedRsaKeysState::NONE| _manager->getRsaKeysState() == CryptManagerLoadedRsaKeysState::PUBLIC){
        std::cout << "CryptManager. ERROR! Need normal private key to crypt" << std::endl;
        return "";
    }
    std::string aesKey = _manager->genAesRandomKey();
    std::string aesCryptedData = _manager->cryptAes(data,aesKey);
    std::string aesCryptedAesKey = _manager->cryptRsaPrivate(aesKey);
    return _manager->toBase(aesCryptedAesKey)  +  KEY_AND_DATA_DELIMER + _manager->toBase(aesCryptedData);
}

std::string Crypter::decrypt(std::string crypted)
{
    if (_manager->getRsaKeysState() == CryptManagerLoadedRsaKeysState::NONE| _manager->getRsaKeysState() == CryptManagerLoadedRsaKeysState::PRIVATE){
        std::cout << "CryptManager. ERROR! Need normal public key to decrypt" << std::endl;
        return "";
    }
    std::string forKey = crypted.substr(0, crypted.find(":"));
    std::string forData = crypted.substr(forKey.size()+1 , crypted.size());
    //std::cout << crypted << " = " << forKey + KEY_AND_DATA_DELIMER + forData << std::endl;
    std::string aesKey = _manager->decryptRsaPublic(_manager->fromBase(forKey));
    return _manager->decryptAes(_manager->fromBase(forData), aesKey);
}

