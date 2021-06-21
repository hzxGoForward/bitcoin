#include "hzxLog.h"
#include "util/time.h"
#include <unordered_map>
#include <map>
#include <vector>
#include "uint256.h"
#include <string>
#include <fstream>

/// int对应区块高度，set<uint256>表示对应某个高度下，纳入预测序列的所有交易集合，用于快速查询
std::unordered_map<int, std::map<uint256, int>> umap_setPredictTxid;

/// int 对应区块高度，uint256对应首次预测区块中最后一笔交易的哈希值和添加进入预测序列时的费率
std::unordered_map<int, std::pair<uint256, double>> umap_predictBlkLastTxHash;

/// int 对应区块高度，vector<uint256> 对应在这个高度下纳入预测序列的所有交易的哈希值，前面交易按交易费排序，后面按照时间排序
std::unordered_map<int, std::vector<uint256>> umap_vecPrecictTxid;

static int64_t writeDiff = 120; // 120秒写一次
static int64_t lastMempoolRecordTime = 0;
static std::string dir = "/home2/zxhu/bitcoin-0.19.0_hzx/experiment20210621/";

void writeFile(const std::string& filename, const std::string& msg)
{
    
    std::string filepath = dir + filename;
    std::ofstream ofs;
    ofs.open(filepath, std::ios::app);
    if (ofs.is_open()) {
        // cout << "正在打开文件写入..." << filepath << endl;
        ofs.write(msg.data(), msg.size());
        ofs.close();
    } else {
        printf("打开文件失败, 文件名：%s \n", filepath.data());
    }
    // printf("成功写入...%s \n", filepath.data());
}


// hzx 压缩区块验证情况
void writeCompctBlockValidation(const std::string& msg, const std::string& filename)
{
    writeFile(filename, msg);
    // compactBlock中交易信息为：
    /*
    发送方ip  接收时间    区块哈希    区块高度    区块大小    区块交易数    预填充交易    交易池命中数    冲突交易数  ReadStatus

    */
}

// hzx 压缩区块的接收情况
void writeCompctRecv(const std::string& msg, const std::string& filename)
{
    writeFile(filename, msg);
    // compactBlock中交易信息为：
    /*
    发送方ip  接收时间    区块哈希

    */
}

// hzx 正常区块的接收情况
void writeNormalRecv(const std::string& msg, const std::string& filename)
{
    writeFile(filename, msg);
    // compactBlock中交易信息为：
    /*
    接收时间    区块哈希    区块高度    交易池交易数  交易池存在的交易数   交易池缺失的交易数, 本地区块高度
    */
}

// mempool 变化统计情况
void writeMempoolMsg(const std::string& msg, const std::string& filename)
{
    const int64_t now = GetTime(); // 时间戳
    auto diff = now - lastMempoolRecordTime;
    // cout << "进入writeMempoolMsg函数, 前后记录时间差" << diff<<"秒"<<endl;
    if (diff >= writeDiff) {
        writeFile(filename, msg);
        lastMempoolRecordTime = now;
    }
    // 每过若干秒分钟系统统计一次交易池的情况
    /*
    时间    交易池大小   交易数
    */
}
