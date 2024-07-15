/**
 * @file Bytetransform.h
 * @author lzy (lzy_cs_LN@163.com)
 * @brief 字节序转换
 * @version 0.1
 * @date 2024-07-15
 *
 * @copyright Copyright (c) 2024
 *
 */

#include <cstdint>

/**
 * @brief 将64位网络字节序转换为主机字节序
 *
 * @param net64
 * @return uint64_t
 */
uint64_t ntoh64(uint64_t net64);

/**
 * @brief 将64位主机字节序转换为网络字节序
 *
 * @param host64
 * @return uint64_t
 */
uint64_t hton64(uint64_t host64);
