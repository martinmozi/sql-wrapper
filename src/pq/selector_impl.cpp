#include "selector_impl.h"
#include "db_row_impl.h"

namespace PqImpl
{
    Selector::Selector(PGconn* conn, bool isSingle)
    :   conn_(conn),
        statement_(conn),
        isSingle_(isSingle)
    {
    }

    void Selector::select(std::function<void(const Sql::DbRow & dbRow)> && selectFunction)
    {
        PGresult* res = statement_.execute();
        if (PQresultStatus(res) != PGRES_TUPLES_OK)
        {
            std::string errorStr = PQerrorMessage(conn_);
            PQclear(res);
            throw std::runtime_error(errorStr);
        }

        int rows = PQntuples(res);
        int nFields = PQnfields(res);

        if (isSingle_)
        {
            if (rows == 0)
            {
                PQclear(res);
                throw std::runtime_error("No data found in select");
            }
            else if (rows > 1)
            {
                PQclear(res);
                throw std::runtime_error("Multiply rows for single selection were selected");
            }
        }

        for (int i = 0; i < rows; i++)
        {
            std::unique_ptr<PqImpl::DbRow> rowData = std::make_unique<PqImpl::DbRow>();
            for (int j = 0; j < nFields; j++)
            {
                if (PQgetisnull(res, i, j) == 1)
                    rowData->appendNull();
                else
                    rowData->append(std::string(PQgetvalue(res, i, j)));
            }

            selectFunction(*rowData.get());
        }

        PQclear(res);
    }
}
