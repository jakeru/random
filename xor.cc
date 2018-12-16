// xor.cc

// 2018-12-16: Written by Jakob Ruhe (jakob.ruhe@gmail.com)

// Solves a XOR encrypted challange with a fixed key length with the
// expectation that the plain text consists of letters in [ A-Za-z]. See the
// function is_readable and the call to isalpha to adjust this.

// Build:
// g++ --std=c++17 -O2 -Wall xor.cc -o xor

// Run first time:
// echo <cipher_hex_string> | xxd -r -p | ./xor

// Run second time, to include beginning of plain text as an argument:
// echo <cipher_hex_string> | xxd -r -p | ./xor <plain text>

// Example of cipher_hex_string:
// 1b3a5577431406315b77501300255e77540e17725a225f111c725f2157134f265832120d0e284977560e08

#include <iostream>
#include <cctype>
#include <vector>
#include <iomanip>

using namespace std;

bool is_readable(const string& cipher, size_t key_pos, size_t key_len, uint8_t key_byte)
{
    for (size_t i = key_pos; i < cipher.size(); i += key_len) {
        char c = cipher[i] ^ key_byte;
        if (!isalpha(c) && c != ' ') {
            return false;
        }
    }
    return true;
}

string decrypt(const string& cipher, const vector< vector<uint8_t> >& possible_keys)
{
    string output;
    for (size_t i = 0; i < cipher.length(); i++) {
        size_t key_pos = i % possible_keys.size();
        if (possible_keys[key_pos].size() == 1) {
            output.push_back(cipher[i] ^ possible_keys[key_pos][0]);
        }
        else {
            output.push_back('_');
        }
    }
    return output;
}

string decrypt(const string& cipher, const vector<uint8_t>& key)
{
    string output;
    for (size_t i = 0; i < cipher.length(); i++) {
        size_t key_pos = i % key.size();
        output.push_back(cipher[i] ^ key[key_pos]);
    }
    return output;
}


int main(int argc, char** argv)
{
    const int KeyLength = 6;
    string cipher;
    vector< vector<uint8_t> > possible_keys(KeyLength);

    cout << "Give me the raw cipher text: " << endl;
    int c;
    while ((c = cin.get()) != EOF) {
        cipher.push_back(char(c));
    }
    cout << "Cipher length: " << cipher.length() << endl;
    cout << "Key length: " << KeyLength << endl;

    for (size_t key_pos = 0; key_pos < KeyLength; key_pos++) {
        for (int key_byte = 0; key_byte < 256; key_byte++) {
            if (is_readable(cipher, key_pos, KeyLength, key_byte)) {
                possible_keys[key_pos].push_back(key_byte);
            }
        }
        cout << "Number of possible keys at " << key_pos << ": " << possible_keys[key_pos].size() << endl;
    }

    cout << "Partly decrypted message: " << endl;
    cout << decrypt(cipher, possible_keys) << endl;

    if (argc != 2) {
        cout << "Give me the beginning of the plain text as an argument, like this:" << endl;
        cout << argv[0] << " \"hello world\"" << endl;
        return 0;
    }

    string plain = argv[1];
    if (plain.size() < KeyLength) {
        cerr << "Plain text must be at least " << KeyLength << " characters long" << endl;
        return 1;
    }

    cout << "Key: ";
    vector<uint8_t> key;
    string key_str;
    for (size_t i = 0; i < KeyLength; i++) {
        key.push_back(plain[i] ^ cipher[i]);
        key_str.push_back(key[i]);
        cout << hex << setw(2) << int(key[i]);
    }
    cout << endl;

    cout << "Key as string: " << key_str << endl;

    cout << "Cipher decrypted with key: " << endl;
    cout << decrypt(cipher, key) << endl;

    return 0;
}