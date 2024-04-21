#pragma once
#include "Header_Files.h"
// #include <openssl/aes.h>
// #include <openssl/rand.h>
// #include <openssl/rsa.h>
// #include <openssl/pem.h>

using namespace std;

namespace olc
{
    namespace net
    {
        class Message_Header{
            protected:
            string UserID;
            // chrono::high_resolution_clock::time_point Time;
            size_t size=0;

            public:
            Message_Header(const string& str) : UserID(str) /*,Time(chrono::high_resolution_clock::now())*/{}

            void setSize(size_t newSize){size=newSize;}
            void setUserID(string& s){UserID = s;}
            // void setTime(chrono::high_resolution_clock::time_point t){Time=t;}
            size_t getSize() const{return size;}
            string getUserID() const{return UserID;}
            // chrono::high_resolution_clock::time_point getTime() const{return Time;}

        };

        class Message : public Message_Header{

            private:
            string body;
            // string encryptionKey ; // store the encyption key
            // string IV; // Store iv
        

            public:
            // Default constructor
            Message(): Message_Header(""),body("")  /*,encryptionKey(""),IV("")*/{}
            // constrictor
            Message (const string& userId, const string& msg): Message_Header(userId){

                // encryptionKey= generateKey();
                // IV= generateIV();

                body=msg;       //encryption(msg);

                setSize(size());
            }
           
            // Return size of the entire message packet in bytes
            size_t size() const{
                return sizeof(Message_Header)+body.size();
            }

            // string encryption(const string& msg){
            //     return encryptWithAES(msg,encryptionKey,IV);
            // }

            // string decryption(const string& msg){
            //     return decryptWithAES(msg,encryptionKey,IV);
            // }

            string READ_HDR(){
                return getUserID();
            }
            string READ_MSG(){
                return body;        //decryption(body);
            }



            // Serilization and deserilizatioon of msg Part
            string SerilizeMsg() const{

                return SerilizeMsgHeader() + SerilizeMsgBody();
            }

            string SerilizeMsgHeader() const{

                return getUserID() +" "+to_string(getSize())+" "; //to_string(getTime())+" ";
            }

            string SerilizeMsgBody() const{

                return body +" ";       //+encryptionKey+" "+IV+" ";
            }

            void DeserilizeMsg(string& serializeMsg){

                stringstream ss(serializeMsg);
                string token;

                getline(ss,token,' '); //userid
                setUserID(token);

                getline(ss,token,' ');  //size
                setSize(stoi(token));

                // getline(ss,token,' ');
                // setTime(chrono::high_resolution_clock::time_point(stoi(token)));

                getline(ss,token,'\n');  //body
                body = token;

                // getline(ss,token,' ');
                // encryptionKey=token;

                // getline(ss,token,' ');
                // IV=token;
            }


            // // Function to generate a random encryption key
            // string generateKey(){
            //     unsigned char key[AES_BLOCK_SIZE];
            //     if(RAND_bytes(key,AES_BLOCK_SIZE)!= 1){
            //         // Error handling gor key genereatin failure
            //         throw runtime_error("Failed to generate encryption key");
            //     }
            //     return string(reinterpret_cast<char*>(key),AES_BLOCK_SIZE);
            // }

            // // Generate IV
            // string generateIV() {
            //     unsigned char iv[AES_BLOCK_SIZE];
            //     if (RAND_bytes(iv, AES_BLOCK_SIZE) != 1) {
            //         throw runtime_error("Failed to generate initialization vector");
            //     }
            //     return string(reinterpret_cast<char*>(iv), AES_BLOCK_SIZE);
            // }


            // // Function to encrypt plaintext using AES with CBC mode
            // string encryptWithAES(const string& plaintext, const string& key, const string& iv) {
            //     EVP_CIPHER_CTX *ctx;
            //     int len;
            //     int ciphertext_len;
            //     unsigned char ciphertext[1024]; // Adjust the size as per your needs

            //     // Create and initialize the context
            //     if(!(ctx = EVP_CIPHER_CTX_new())) {
            //         throw runtime_error("Failed to create EVP cipher context");
            //     }

            //     // Initialize the encryption operation
            //     if(1 != EVP_EncryptInit_ex(ctx, EVP_aes_128_cbc(), nullptr, reinterpret_cast<const unsigned char*>(key.c_str()), reinterpret_cast<const unsigned char*>(iv.c_str()))) {
            //         throw runtime_error("Failed to initialize encryption operation");
            //     }

            //     // Perform encryption
            //     if(1 != EVP_EncryptUpdate(ctx, ciphertext, &len, reinterpret_cast<const unsigned char*>(plaintext.c_str()), plaintext.length())) {
            //         EVP_CIPHER_CTX_free(ctx);
            //         throw runtime_error("Failed to perform encryption");
            //     }
            //     ciphertext_len = len;

            //     // Finalize encryption
            //     if(1 != EVP_EncryptFinal_ex(ctx, ciphertext + len, &len)) {
            //         EVP_CIPHER_CTX_free(ctx);
            //         throw runtime_error("Failed to finalize encryption");
            //     }
            //     ciphertext_len += len;

            //     EVP_CIPHER_CTX_free(ctx);

            //     return string(reinterpret_cast<char*>(ciphertext), ciphertext_len);
            // }

            // // Function to decrypt ciphertext using AES with CBC mode
            // string decryptWithAES(const string& ciphertext, const string& key, const string& iv) {
            //     EVP_CIPHER_CTX *ctx;
            //     int len;
            //     int plaintext_len;
            //     unsigned char plaintext[1024]; // Adjust the size as per your needs

            //     // Create and initialize the context
            //     if(!(ctx = EVP_CIPHER_CTX_new())) {
            //         throw runtime_error("Failed to create EVP cipher context");
            //     }

            //     // Initialize the decryption operation
            //     if(1 != EVP_DecryptInit_ex(ctx, EVP_aes_128_cbc(), nullptr, reinterpret_cast<const unsigned char*>(key.c_str()), reinterpret_cast<const unsigned char*>(iv.c_str()))) {
            //         throw runtime_error("Failed to initialize decryption operation");
            //     }

            //     // Perform decryption
            //     if(1 != EVP_DecryptUpdate(ctx, plaintext, &len, reinterpret_cast<const unsigned char*>(ciphertext.c_str()), ciphertext.length())) {
            //         EVP_CIPHER_CTX_free(ctx);
            //         throw runtime_error("Failed to perform decryption");
            //     }
            //     plaintext_len = len;

            //     // Finalize decryption
            //     if(1 != EVP_DecryptFinal_ex(ctx, plaintext + len, &len)) {
            //         EVP_CIPHER_CTX_free(ctx);
            //         throw runtime_error("Failed to finalize decryption");
            //     }
            //     plaintext_len += len;

            //     EVP_CIPHER_CTX_free(ctx);

            //     return string(reinterpret_cast<char*>(plaintext), plaintext_len);
            // }
        };
    }
}
