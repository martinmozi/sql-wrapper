#pragma once

#include "../../include/selector.h"
#include "statement_base_impl.h"

namespace PqImpl
{
    class Selector : public StatementBase, Sql::Selector
    {
    public:
        Selector(PGconn* conn, bool isSingle);
        void select(std::function<void(const Sql::DbRow & dbRow)> selectFunction) override;

    private:
        bool isSingle_;
    };
}