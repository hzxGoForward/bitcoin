#include <fstream>
#include <iostream>
#include <map>
#include <set>
#include <string>
#include <uint256.h>
#include <unordered_map>
#include <util/time.h>
#include <string>
#include <cstdarg>
using namespace std;
static int64_t writeDiff = 120; // 120秒写一次
static int64_t lastMempoolRecordTime = 0;
static string dir = "/home2/zxhu/bitcoin-0.19.0_hzx/experiment20200902/";
/*
1. 这一版本中，预测新交易时，取消了区块验证过程
2. 在收到新的区块后，增加一个新功能，指定预测交易序列数，并且可以排除某些交易
3. 每次预测后，检查生成的交易是否在预测序列中，如果在其中才算成功。
4. 每次预测一批交易的时候，记录这批交易加入区块时隶属的组合费率
5. 文件保存位置为experiment20200903
6. 修改umap_setPredictTxid中的value形式为map<uint256,int>的类型，其中int指示该元素的索引
7. umap_predictBlkLastTxHash变量暂时不再使用
8. 增加一个字符串format函数，方便输出和文件处理
*/


/// int对应区块高度，set<uint256>表示对应某个高度下，纳入预测序列的所有交易集合，用于快速查询
static unordered_map<int, map<uint256, int>> umap_setPredictTxid;

/// int 对应区块高度，uint256对应首次预测区块中最后一笔交易的哈希值和添加进入预测序列时的费率
static unordered_map<int, pair<uint256, double>> umap_predictBlkLastTxHash;             

/// int 对应区块高度，vector<uint256> 对应在这个高度下纳入预测序列的所有交易的哈希值，前面交易按交易费排序，后面按照时间排序
static unordered_map<int, vector<uint256>> umap_vecPrecictTxid;

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
