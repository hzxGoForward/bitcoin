// #include "dino.h"

//DinoBlock::DinoBlock(const CBlock& mBlk_, const CBlock& predBlk_, const std::map<CTransactionRef, size_t>& missTx)
//    : minerBlk(mBlk_), predBlk(predBlk_)
//{
//    for (auto it = missTx.cbegin(); it != missTx.cend(); ++it) {
//        nMissTxSz += it->first->GetTotalSize();
//        missTxSet.insert(it->first->GetHash());
//        mapMissTx[it->second] = it->first;
//    }
//    txCnt = minerBlk.vtx.size();
//    range = {predBlk.vtx.size(), 0};
//}
//
//
//void DinoBlock::markTxPos()
//{
//    for (size_t idx = 0; idx < minerBlk.vtx.size(); ++idx) {
//        auto txid = minerBlk.vtx[idx]->GetHash();
//        umapMinerBlkTxIndex[txid] = idx;
//    }
//
//    for (size_t idx = 0; idx < predBlk.vtx.size(); ++idx) {
//        auto txid = predBlk.vtx[idx]->GetHash();
//        umapPredBlkTxIndex[txid] = idx;
//    }
//}
//
//
//// 获取预测序列的起始和结束
//void DinoBlock::getPredictRange()
//{
//    for (size_t i = 0; i < minerBlk.vtx.size(); ++i) {
//        const uint256 txHash = minerBlk.vtx[i]->GetHash();
//        if (umapPredBlkTxIndex.count(txHash)) {
//            const size_t idx = umapPredBlkTxIndex[txHash];
//            if (range.first > idx) {
//                range.first = idx;
//            }
//            if (range.second < idx) {
//                range.second = idx;
//            }
//        }
//    }
//}
//
//
//// 求每一个存在于vBlkTx中的交易,它的最长连续序列是多少
//// 返回结果放置于lcs中
//// 必须保证minerBlk.vtx中的交易，除了coinbase交易之外，其他交易一定存在于predBlk.vtx的[range.first, range.second]范围内
//void DinoBlock::greedyLCSeq()
//{
//    const std::vector<CTransactionRef>& vBlkTx = minerBlk.vtx;
//    const std::vector<CTransactionRef>& vPredTx = predBlk.vtx;
//    size_t i = range.first;
//    while (i <= range.second) {
//        const uint256 predTxHash = vPredTx[i]->GetHash();
//        if (umapMinerBlkTxIndex.count(predTxHash) == 0) {
//            ++i;
//            continue;
//        }
//
//        /* running before
//        *   minerIndex
//        *       ⬇ 
//        --------a--b--c--d--e--f--g-----⬅ vBlkTx
//        --------a--h--b--c--d--e--g--f--h---⬅ vPredTx
//        *       ⬆                    ⬆
//        *   predIndex           range.second
//        *
//        * running after
//        *                 minerIndex
//        *                      ⬇ 
//        --------a--b--c--d--e--f--g-----⬅ vBlkTx
//        --------a--h--b--c--d--e--g--f--h---⬅ vPredTx
//        *                         ⬆
//        *                     predIndex           
//        */
//
//        size_t minerIndex = umapMinerBlkTxIndex[predTxHash];
//
//        size_t predIndex = i, cnt = 0;
//        while (predIndex <= range.second && minerIndex < vBlkTx.size()) {
//            const uint256& predTxHash = vPredTx[predIndex]->GetHash();
//            const uint256& minerTxHash = vBlkTx[minerIndex]->GetHash();
//            if (minerTxHash == predTxHash) {
//                ++predIndex;
//                ++minerIndex;
//                ++cnt;
//            } else if (umapPredBlkTxIndex.count(predTxHash) == 0) {
//                ++predIndex;
//            } else {
//                break;
//            }
//        }
//
//        while (i < predIndex) {
//            const uint256& txHash = vPredTx[i]->GetHash();
//            if (umapMinerBlkTxIndex.count(txHash))
//                lcs.emplace(txHash, cnt--);
//            i++;
//        }
//    }
//}
//
//
//// 计算预测序列和实际区块序列误差
//void DinoBlock::transfromTxSeq()
//{
//    const std::vector<CTransactionRef>& vBlkTx = minerBlk.vtx;
//    const std::vector<CTransactionRef>& vPredTx = predBlk.vtx;
//
//    // 将区块中[0, offset）的交易标记为miss状态，或者是索引发生变化的状态
//    size_t pi = range.first;
//    size_t bj = 1; // 从1开始，跳过coinbase交易
//    std::unordered_set<uint256> visitSet;
//
//
//    while (pi <= range.second && bj < vBlkTx.size()) {
//        const uint256& piTxHash = vPredTx[pi]->GetHash();
//        const uint256& bjTxHash = vBlkTx[bj]->GetHash();
//        if (visitSet.count(bjTxHash))
//            bj++;
//        else if (visitSet.count(piTxHash))
//            pi++;
//        // miss 的交易, 除了coinbase交易之外，按道理不应该出现，但是667155文件中出现了这个情况，很怪异
//        else if (umapPredBlkTxIndex.count(bjTxHash) == 0) {
//            if (missTxSet.count(bjTxHash) == 0) {
//                mapMissTx[bj] = vBlkTx[bj];
//                nMissTxCnt++;
//                nMissTxSz += vBlkTx[bj]->GetTotalSize();
//            }
//            // throw std::runtime_error(dino_format("Dino predBlk Error, file height : %d\n",minerBlk.height));
//            bj++;
//        }
//        // 前后序列相同
//        else if (bjTxHash == piTxHash) {
//            pi++;
//            bj++;
//        }
//        // 如果是最后一个,可以直接放入vChangeRecord中,减少删除的index数量
//        else if (bj == vBlkTx.size() - 1) {
//            const size_t bj_idx = umapPredBlkTxIndex[bjTxHash];
//            vChangeRecord[bj_idx] = bj;
//            visitSet.insert(vBlkTx[bj++]->GetHash());
//        }
//        // 当前predTx序列不存在
//        else if (umapMinerBlkTxIndex.count(piTxHash) == 0)
//            vDelIndex.insert(pi++);
//        else {
//            //
//            const size_t pi_len = lcs[piTxHash];
//            const size_t bj_len = lcs[bjTxHash];
//            if (pi_len < bj_len) {
//                // 记录pi在vBlkTx中的索引,然后pi跳过
//                const size_t pi_idx = umapMinerBlkTxIndex[piTxHash];
//                vChangeRecord[pi] = pi_idx;
//                visitSet.insert(piTxHash);
//                pi++;
//            } else {
//                // 记录当前bj在predtx中的索引
//                const size_t bj_idx = umapPredBlkTxIndex[bjTxHash];
//                vChangeRecord[bj_idx] = bj;
//                visitSet.insert(bjTxHash);
//                bj++;
//            }
//        }
//    }
//}
//
///// 重建区块
//int64_t DinoBlock::rebuildBlock()
//{
//    const std::vector<CTransactionRef>& vPredTx = predBlk.vtx;
//    std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
//    // 根据得到的误差重新构建新的区块大小
//    std::string noTxId = "";
//    newBlkSeq.reserve(txCnt);
//    newBlkSeq.resize(txCnt, noTxId);
//
//    // 构建miss交易,一般来说应该是coinbase交易
//    for (auto& e : mapMissTx) {
//        newBlkSeq[e.first] = e.second->GetHash().ToString();
//    }
//
//    // 构建位置发生变化的交易
//    for (auto& e : vChangeRecord)
//        newBlkSeq[e.second] = vPredTx[e.first]->GetHash().ToString();
//
//    // 从vPredTx的range范围内构建交易
//    size_t i = range.first, j = 0;
//    while (i <= range.second && j < txCnt) {
//        // 如果j已经被填充了，则跳过j
//        while (j < txCnt && newBlkSeq[j] != noTxId)
//            j++;
//        // 如果是需要删除的交易，或者是位置发生变化的交易
//        while (i <= range.second && (vChangeRecord.count(i) || vDelIndex.count(i))) {
//            i++;
//        }
//        if (i <= range.second && j < txCnt)
//            newBlkSeq[j++] = vPredTx[i++]->GetHash().ToString();
//    }
//
//    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
//    std::chrono::milliseconds millsec = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
//    return millsec.count();
//}
//
//
//// 验证区块合法性
//bool DinoBlock::verifyBlock() const
//{
//    std::string msg = "";
//    if (newBlkSeq.size() != minerBlk.vtx.size()) {
//        std::printf("ERROR: two block with different size\n");
//        return false;
//    }
//    for (size_t i = 0; i < newBlkSeq.size(); ++i) {
//        if (newBlkSeq[i] != minerBlk.vtx[i]->GetHash().ToString()) {
//            std::cout << dino_format("ERROR: %zu index not match with\n [%s] \n [%s]\n", i, newBlkSeq[i].data(), minerBlk.vtx[i]->GetHash().ToString());
//            msg += dino_format("Construct Failed ××××××××××××××××××××××××××\n\n");
//            return false;
//        }
//    }
//    msg += dino_format("Construct Succeed √√√√√√√√√√√√√√√√√√√√√√√√√");
//    std::cout << msg << std::endl;
//    return true;
//}
//
//size_t DinoBlock::getDinosCost() const
//{
//    const size_t rangeSz = 4;           // pb 和 pe
//    const size_t vChangeTail = 4;       // {-1,-1}表示坐标变化的结尾
//    const size_t vDelIndexTail = 2;     // {-1} 表示删除索引结束
//    const size_t vDelIndexPairTail = 4; // {-1,-1} 表示删除索引std::pair的结束
//    const size_t vmissedTxSize = nMissTxCnt * 2;
//    std::cout << "vChangeRecord 开销: " << vChangeRecord.size() * 4 << std::endl;
//    std::cout << "新的计算方式的开销: " << getChangeIndexCost() << std::endl;
//    return vChangeRecord.size() * 4 + getDelIndexCost() + 4 + 4 + 2 + 4; // 4 个字节用表示Pb和Pe的范围
//}
//
//std::string DinoBlock::printMsg() const
//{
//    std::string msg = "";
//    msg += dino_format("Tx Count in New Block: %d\n", txCnt);
//    msg += dino_format("Predict Range: [%d, %d]\n", range.first, range.second);
//    msg += dino_format("Missed Tx Count: %d, costs Bytes: %d\n", nMissTxCnt, nMissTxSz);
//
//    size_t nchangeRecordTxSize = vChangeRecord.size() * 4;
//    msg += dino_format("Changed Index Count:%d , costs Bytes: %d\n", vChangeRecord.size(), nchangeRecordTxSize);
//
//    size_t ndelCost = getDelIndexCost();
//    msg += dino_format("Index Deleted Count: %d, costs Bytes: %d \n", vDelIndex.size(), ndelCost);
//    size_t rangeSz = 4;
//    size_t DinosCost = getDinosCost();
//    size_t totalCost = 80 + nMissTxSz + DinosCost;
//    msg += dino_format("total Sync Cost Bytes: %d, Missed Tx Percent %f , Dinos Cost bytes: %d \n", totalCost,
//        static_cast<double>(nMissTxSz) / totalCost, DinosCost);
//    return msg;
//}
//
//size_t DinoBlock::getDelIndexCost() const
//{
//    std::vector<size_t> tmpVDelIndex;
//    std::vector<std::pair<size_t, size_t>> tmpVDelIndexPair;
//    int pre = -1;
//
//    // 将最后一个插入其中
//    auto it = vDelIndex.begin();
//    while (it != vDelIndex.end()) {
//        size_t tmp = *it;
//        size_t last = *it;
//        while (it != vDelIndex.end() && vDelIndex.count(*it + 1) == 1) {
//            last = *it;
//            it++;
//        }
//        if (tmp == last)
//            tmpVDelIndex.push_back(tmp);
//        else {
//            tmpVDelIndexPair.emplace_back(tmp, last);
//        }
//        if (it != vDelIndex.end())
//            it++;
//    }
//    // 告诉单个需要删除的index、需要成对删除的index+8
//    return tmpVDelIndex.size() * 2 + tmpVDelIndexPair.size() * 4 + 8;
//}
//
//
//size_t DinoBlock::getChangeIndexCost() const
//{
//    std::vector<std::tuple<size_t, size_t, size_t>> tmpChangeIndexTuple;
//    std::vector<std::pair<size_t, size_t>> tmpChangeIndexPair;
//    int pre = -1;
//
//    // 将最后一个插入其中
//    auto it = vChangeRecord.begin();
//    while (it != vChangeRecord.end()) {
//        size_t tmp = it->first;
//        size_t last = it->first;
//        auto secIt = vChangeRecord.find(it->first + 1);
//        while (it != vChangeRecord.end() && vChangeRecord.count(it->first + 1) == 1 && secIt != vChangeRecord.end()) {
//            last = it->first;
//            it++;
//        }
//        if (tmp == last) {
//            auto tmpit = vChangeRecord.find(tmp);
//            tmpChangeIndexPair.emplace_back(tmp, tmpit->second);
//        } else {
//            auto tmpit = vChangeRecord.find(tmp);
//            tmpChangeIndexTuple.emplace_back(tmp, last, tmpit->second);
//        }
//        if (it != vChangeRecord.end())
//            it++;
//    }
//    // 告诉单个位置变化的pair + 连续变化的pair * 6 + 8
//    return tmpChangeIndexPair.size() * 2 + tmpChangeIndexTuple.size() * 6 + 8;
//}
//
//
//std::string DinoBlock::GetWriteMsg() const
//{
//    size_t DinosCost = getDinosCost();
//    size_t totalCost = 80 + nMissTxSz + DinosCost;
//    // txvolume missTxCnt, missTxSz,  delSize,          delCost,          ChangeSize,           DinoCost   TotalCost
//    return dino_format("%d %d %d %d %d %d %d %d",
//        txCnt, nMissTxCnt, nMissTxSz, vDelIndex.size(), getDelIndexCost(), vChangeRecord.size(), DinosCost, totalCost);
//}
//
//
//std::vector<size_t> DinoBlock::getCost()
//{
//    std::vector<size_t> vCost;
//    vCost.reserve(4);
//    size_t DinosCost = getDinosCost();
//    size_t totalCost = 80 + nMissTxSz + DinosCost;
//    vCost.push_back(totalCost);
//    vCost.push_back(DinosCost);
//    vCost.push_back(vChangeRecord.size());
//    vCost.push_back(vDelIndex.size());
//    return vCost;
//}
//
//void DinoBlock::dinoProtocol()
//{
//    // record tx's indexes
//    markTxPos();
//
//    // compute predict Range
//    getPredictRange();
//
//    // get Longest common sequence
//    greedyLCSeq();
//
//
//    transfromTxSeq();
//
//    rebuildBlock();
//
//    verifyBlock();
//
//    // std::cout << printMsg() << std::endl;
//}
