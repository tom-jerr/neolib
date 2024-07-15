/**
 * @file Bytetransfrom.cpp
 * @author lzy (lzy_cs_LN@163.com)
 * @brief 字节序转换函数实现
 * @version 0.1
 * @date 2024-07-15
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "Bytetransform.h"
#include <byteswap.h>
#include <cstdint>

uint64_t ntoh64(uint64_t net64) {
  if (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
    return bswap_64(net64);
  else
    return net64;
}

uint64_t hton64(uint64_t host64) {
  if (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
    return bswap_64(host64);
  else
    return host64;
}
