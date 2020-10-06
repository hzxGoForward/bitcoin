#ifndef HZXLOG_H
#define HZXLOG_H


#include <fstream>
#include <cstdio>
#include <map>
#include <set>
#include <string>
#include <uint256.h>
#include <unordered_map>
#include <util/time.h>
#include <string>
#include <cstdarg>
#include <thread>
using namespace std;
static int64_t lastMempoolRecordTime = 0;
const static int64_t writeDiff = 120; // 120秒写一次
const static string dir = "/home2/zxhu/bitcoin-0.19.0_hzx/experiment20200925/";
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
10. 输出收到的交易大小时，顺带输出compact block的大小。
11. 测试在区块恒定的情况下，通过尽可能的增加交易速率，查看发送数据是否发生变化
12. 将hzxLog.h 中的static变量修改为const属性
- . 将挖矿miner.cpp中的生成区块的函数进行了修改，使得可以生成标准区块特定倍数的交易
- . 将交易大小限定在5MB中，每次收到6笔交易后模拟矿工挖矿,同时基于当前交易做出预测
- . 目录改为0925
- .  删除了生成交易时的跳过阶段
- . 新增mempool路径存放mempool的统计信息
- . 新增compactBlock和normalBlock路径
- . 不再测试交易速率的影响，测试可扩展性，路径未变
*/


/// int对应区块高度，set<uint256>表示对应某个高度下，纳入预测序列的所有交易集合，用于快速查询
static unordered_map<int, map<uint256, int>> umap_setPredictTxid;

/// int 对应区块高度，uint256对应首次预测区块中最后一笔交易的哈希值和添加进入预测序列时的费率
static unordered_map<int, pair<uint256, double>> umap_predictBlkLastTxHash;             

/// int 对应区块高度，vector<uint256> 对应在这个高度下纳入预测序列的所有交易的哈希值，前面交易按交易费排序，后面按照时间排序
static unordered_map<int, vector<uint256>> umap_vecPrecictTxid;


// TODO START BY HZX 下列代码用于模拟实验
const static int txRate = 6;            // 假设δt是3秒，最低交易生成速率为4笔每秒，即一个δt内产生12笔交易，我们假设节点收到一半交易，即一个δt中产生6笔交易
const static int multi_block = 60;     // 区块大小的倍数限制
static bool testTxRate = false;         // 配置文件，如果设置为true，则测试交易产生速率对Atom协议的影响，false则不测试影响
static bool simulate = true;            // 是否进行模拟实验
static int lastSimTime = 0;             // 上次模拟实验的时间
static const int simInterval = 60;      // 间隔为60秒
 /// int对应区块高度，set<uint256>表示对应某个高度下，纳入预测序列的所有交易集合，用于快速查询， 模拟实验用
static unordered_map<int, map<uint256, int>> umap_mapTxidIndex_simulator;

/// int 对应区块高度，uint256对应首次预测区块中最后一笔交易的哈希值和添加进入预测序列时的费率， 模拟实验用
static unordered_map<int, pair<uint256, double>> umap_LastTxFeeRate_simulator;

/// int 对应区块高度，vector<uint256> 对应在这个高度下纳入预测序列的所有交易的哈希值，前面交易按交易费排序，后面按照时间排序， 模拟实验用
static unordered_map<int, vector<uint256>> umap_vecTxid_simulator;

static unordered_map<int, int> umap_simCnt_simulator;                      // 在指定高度上模拟的次数，写文件用
static unordered_map<int, int> umap_lastPredPos_simulator;
static unordered_map<int, set<uint256>> umap_setTxid_simulator;           // 指定高度上的交易预测序列,用于生成交易序列


template <typename... Args>
static std::string format(const char* format, Args... args)
{
    int length = std::snprintf(nullptr, 0, format, args...);
    assert(length >= 0);

    char* buf = new char[length + 1];
    std::snprintf(buf, length + 1, format, args...);

    std::string str(buf);
    delete[] buf;
    return str;
}


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
        printf("打开文件失败, 文件名：%s \n", filepath.data());
    }
    printf("成功写入...%s \n", filepath.data());
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

static void printThreadID() {
    cout << "thread id: "<< std::this_thread::get_id() << "\n";
}

static void printPredMsg(int h) {
    printThreadID();
    printf("predTxidSetSize: %llu, index: %d, vecTxidSize: %llu \n", umap_setTxid_simulator[h].size(), umap_lastPredPos_simulator[h], umap_vecTxid_simulator[h].size());
}

static void deletePredTx(int blkHeight)
{
    // 删除 正常模式的预测序列
    umap_setPredictTxid.erase(blkHeight); // 删除
    umap_predictBlkLastTxHash.erase(blkHeight);
    umap_vecPrecictTxid.erase(blkHeight); // 删除

    // 删除模拟实验的预测序列
    umap_mapTxidIndex_simulator.erase(blkHeight);
    umap_LastTxFeeRate_simulator.erase(blkHeight); // 删除
    umap_vecTxid_simulator.erase(blkHeight);       // 删除
    umap_simCnt_simulator.erase(blkHeight);        // 删除为该高度预测的结果
    umap_lastPredPos_simulator.erase(blkHeight);
    umap_setTxid_simulator.erase(blkHeight); // 删除
}


#endif
