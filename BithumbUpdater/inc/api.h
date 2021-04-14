#pragma once
#include "api_info.h"
#include "header.h"

#include <stdio.h>
#include <stdlib.h>
#include <curl/curl.h>
#include <sys/time.h>
#include <limits.h>
#include <openssl/hmac.h>
#include <openssl/sha.h>


// Bithumb API requset function
// 'endpoint' means request type, 'post_data' means paramater of request
// return var need to delete after use
char* api_request(char* endpoint, char* post_data);