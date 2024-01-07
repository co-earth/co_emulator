
RUNTIME_DATA_TYPE_BEGIN(HonorLevelFormula, "cabal.cabal_reputation.reputation", 64)
    RUNTIME_DATA_PROPERTY(Int32, Rank, "class")
    RUNTIME_DATA_PROPERTY(Int32, MinPoint, "min_reputation")
    RUNTIME_DATA_PROPERTY(Int32, MaxPoint, "promote_reputation")
RUNTIME_DATA_TYPE_END(HonorLevelFormula)
RUNTIME_DATA_TYPE_INDEX(HonorLevelFormula, Int32, Rank)
