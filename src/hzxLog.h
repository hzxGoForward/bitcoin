#include <fstream>
#include <iostream>
#include <map>
#include <set>
#include <string>
#include <uint256.h>
#include <unordered_map>
#include <util/time.h>
using namespace std;
static int64_t writeDiff = 120; // 120秒写一次
static int64_t lastMempoolRecordTime = 0;
static string dir = "/home2/zxhu/bitcoin-0.19.0_hzx/experiment20200813_2/";


/// int对应区块高度，set<uint256>表示对应某个高度下，纳入预测序列的所有交易集合，用于快速查询
static unordered_map<int, set<uint256> > umap_setPredictTxid;

/// int 对应区块高度，uint256对应首次预测区块中最后一笔交易的哈希值和添加进入预测序列时的费率
static unordered_map<int, pair<uint256, double>> umap_predictBlkLastTxHash;             

/// int 对应区块高度，vector<uint256> 对应在这个高度下纳入预测序列的所有交易的哈希值，前面交易按交易费排序，后面按照时间排序
static unordered_map<int, vector<uint256>> umap_vecPrecictTxid;

// 写文件
static void writeFile(const string& filename, const string& msg)
{
    string filepath = dir + filename;
    ofstream ofs;
    ofs.open(filepath, ios::app);
    if (ofs.is_open()) {
        // cout << "正在打开文件写入..." << filepath << endl;
        ofs.write(msg.data(), msg.size());
        ofs.close();
    } else {
        cout << "打开文件失败, 文件名： " << filepath << endl;
    }
    cout << "成功写入..." << filepath << endl;
}

// hzx 压缩区块验证情况
static void writeCompctBlockValidation(const string& msg, const string& filename)
{
    writeFile(filename, msg);
    // compactBlock中交易信息为：
    /*
    发送方ip  接收时间    区块哈希    区块高度    区块大小    区块交易数    预填充交易    交易池命中数    冲突交易数  ReadStatus

    */
}

// hzx 压缩区块的接收情况
static void writeCompctRecv(const string& msg, const string& filename)
{
    writeFile(filename, msg);
    // compactBlock中交易信息为：
    /*
    发送方ip  接收时间    区块哈希

    */
}

// hzx 正常区块的接收情况
static void writeNormalRecv(const string& msg, const string& filename)
{
    writeFile(filename, msg);
    // compactBlock中交易信息为：
    /*
    接收时间    区块哈希    区块高度    交易池交易数  交易池存在的交易数   交易池缺失的交易数, 本地区块高度
    */
}

// mempool 变化统计情况
static void writeMempoolMsg(const string& msg, const string& filename)
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