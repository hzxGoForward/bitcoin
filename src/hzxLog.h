#ifdef _HZXlOG_H
#define _HZXLOG_H


#include <fstream>
#include <cstdio>
#include <map>
#include <set>
#include <string>
#include <uint256.h>
#include <unordered_map>
#include <chrono>
#include <string>
#include <cstdarg>
#include <vector>


/// int对应区块高度，set<uint256>表示对应某个高度下，纳入预测序列的所有交易集合，用于快速查询
extern std::unordered_map<int, std::map<uint256, int>> umap_setPredictTxid;

/// int 对应区块高度，uint256对应首次预测区块中最后一笔交易的哈希值和添加进入预测序列时的费率
extern std::unordered_map<int, std::pair<uint256, double>> umap_predictBlkLastTxHash;

/// int 对应区块高度，vector<uint256> 对应在这个高度下纳入预测序列的所有交易的哈希值，前面交易按交易费排序，后面按照时间排序
extern std::unordered_map<int, std::vector<uint256>> umap_vecPrecictTxid;

/*
1. 这个版本这种，收到交易后，尝试将本地不存在的交易放入交易池和预测序列中，再生成预测序列，然后查看此时生成的交易序列的准确性
2. 修复预测区块多余交易数中出现负数的bug
3. 将预测交易序列函数放入交易处理函数中
4. 将所有字符串操作转换为format，方便读取
5. compare函数中由于涉及到很多写的操作，这一版本在转换为多线程操作
6. 修复了validation.cpp中数据输出的格式错误
7. 新建路径9月8号√
8. 每次调整发送交易序列时，生成交易预测序列，并且与当前交易一起发送，√
9. 用已有的交易预测序列+缺失的交易预测-不应该出现的交易 预测区块中的交易。
10. 这个版本准备将完整的Dino协议进行实现，2020年12月8日
11. 20210107，准备实现完整的Dino协议，这次说到做到！
*/


//template <typename... Args>
// std::string format(const char* format, Args... args)
//{
//    int length = std::snprintf(nullptr, 0, format, args...);
//    assert(length >= 0);
//
//    char* buf = new char[length + 1];
//    std::snprintf(buf, length + 1, format, args...);
//
//    std::string str(buf);
//    delete[] buf;
//    return str;
//}


//// 写文件
void writeFile(const std::string& filename, const std::string& msg);

// hzx 压缩区块验证情况
void writeCompctBlockValidation(const std::string& msg, const std::string& filename);

// hzx 压缩区块的接收情况
 void writeCompctRecv(const std::string& msg, const std::string& filename);

// hzx 正常区块的接收情况
 void writeNormalRecv(const std::string& msg, const std::string& filename);

// mempool 变化统计情况
 void writeMempoolMsg(const std::string& msg, const std::string& filename);

#endif
