#pragma once

#include <iostream>
#include <dlfcn.h>
#include <string>
#include <unistd.h>

class SecretAPI {
 public:
    // API wrapper
    static std::string BasicEncrypt(const std::string& raw_input) {
        const char* path = "/tmp/prebuilt/libsecret_key.so";
        if (access(path, F_OK) == -1) {
            return "Error: File libsecret.so does not exist!";
        }

        void* handle = dlopen(path, RTLD_LAZY);
        if (!handle) return "Error: Load Lib Failed";

        typedef std::string (*basic_encrypt_fn)(const std::string& input);
        basic_encrypt_fn func =
            (basic_encrypt_fn)dlsym(handle, "basic_encrypt");

        std::string result = "";
        if (func) {
            result = func(raw_input);  //.so
        }

        dlclose(handle);
        return result;
    }
};