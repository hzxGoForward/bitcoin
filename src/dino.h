#ifndef DINO_H
#define DINO_H

#include "primitives/transaction.h"
#include "serialize.h"
#include "primitives/block.h"
#include <memory>
#include <map>
#include <set>
#include <vector>
#include <string>
#include <unordered_map>
#include "uint256.h"
#include "miner.h"
#include "hzxLog.h"


class DinoBlock
{
public:
    const CBlock& minerBlk;
    const CBlock& predBlk;

    DinoBlock(const CBlock& mBlk_, const CBlock& predBlk_, const std::map<CTransactionRef, size_t>& missTx);



    std::map<uint256, size_t> umapMinerBlkTxIndex;
    std::map<uint256, size_t> umapPredBlkTxIndex;
    std::map<uint256, size_t> lcs;
    std::map<size_t, CTransactionRef> mapMissTx;
    std::set<uint256> missTxSet;
    size_t nMissTxCnt;
    size_t nMissTxSz;
    // variables about a eomplete dinoblock, send to peer.
    std::map<size_t, size_t> vChangeRecord;
    std::unordered_set<size_t> vDelIndex;
    std::pair<size_t,size_t> range;
    size_t txCnt;
    std::vector<std::string> newBlkSeq;

    // void dinoProtocol();

    ADD_SERIALIZE_METHODS;

    template <typename Stream, typename Operation>
    inline void SerializationOp(Stream& s, Operation ser_action)
    {
        READWRITE(minerBlk->GetBlockHeader());
        READWRITE();

        std::vector<std::pair<short, short>> vC;
        for (auto& e : vChangeRecord)
            vC.emplace_back(static_cast<short>(e.first), static_cast<short>(e.second));
        std::vector<short> del;
        for (auto& e : vDelIndex)
            del.emplace_back(e);
        READWRITE(vC);
        READWRITE(del);
        READWRITE(std::pair<short, short>{static_cast<short>(range.first), static_cast<short>(range.second)});
        READWRITE(static_cast<short>(nMissTxCnt));
        READWRITE(static_cast<short>(txCnt));
    }


    //// mark every transaction position in blk
    //void markTxPos();

    //// compute predSequence range in minerblk
    //void getPredictRange();

    //void greedyLCSeq();

    //void transfromTxSeq();

    ///// 重建区块
    //int64_t rebuildBlock();

    //size_t getDinosCost() const;

    //// 验证区块合法性
    //bool verifyBlock() const;

    //// 计算删除元素的耗费
    //size_t getDelIndexCost() const;

    //size_t getChangeIndexCost() const;
    //
    //std::string DinoBlock::GetWriteMsg() const;

    //std::vector<size_t> DinoBlock::getCost();

    // // 打印各个结果
    // std::string printMsg() const;

};


template <typename... Args>
std::string dino_format(const char* format, const Args&... args)
{
    int length = std::snprintf(nullptr, 0, format, args...);

    char* buf = new char[static_cast<size_t>(length) + 1];
    std::snprintf(buf, static_cast<size_t>(length) + 1, format, args...);

    std::string str(buf);
    str.resize(str.size());
    delete[] buf;
    return str;
}


#endif
