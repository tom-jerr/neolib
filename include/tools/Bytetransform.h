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
#include <endian.h>
namespace neonet::socket {
inline uint64_t hostToNetwork64(uint64_t host64) { return htobe64(host64); }

inline uint32_t hostToNetwork32(uint32_t host32) { return htobe32(host32); }

inline uint16_t hostToNetwork16(uint16_t host16) { return htobe16(host16); }

inline uint64_t networkToHost64(uint64_t net64) { return be64toh(net64); }

inline uint32_t networkToHost32(uint32_t net32) { return be32toh(net32); }

inline uint16_t networkToHost16(uint16_t net16) { return be16toh(net16); }
} // namespace neonet::socket
// /**
//  * @brief 将64位网络字节序转换为主机字节序
//  *
//  * @param net64
//  * @return uint64_t
//  */
// uint64_t ntoh64(uint64_t net64);

// /**
//  * @brief 将64位主机字节序转换为网络字节序
//  *
//  * @param host64
//  * @return uint64_t
//  */
// uint64_t hton64(uint64_t host64);
