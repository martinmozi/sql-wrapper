#include "connection_impl.h"

#ifdef PQ_BACKEND
#include "pq/connection_impl.h"
#include "pq/statement_impl.h"
#include "pq/selector_impl.h"
#endif

#ifdef SQLITE_BACKEND
#include "sqlite/sqlitedb.h"
#endif

namespace DbImpl
{
    Connection::Connection(Sql::DbType dbType, const std::string & connectionStr)
    :   dbType_(dbType),
        connectionString_(connectionStr)
    {
    }

    Connection::~Connection()
    {
        disconnect();
    }

    std::unique_ptr<Sql::Statement> Connection::createStatement()
    {
        switch (dbType_)
        {
        case Sql::DbType::Postgres:
        {
            PqImpl::Connection* pConnection = dynamic_cast<PqImpl::Connection*>(this);
            return std::make_unique<PqImpl::Statement>(pConnection->connection());
        }

        case Sql::DbType::Sqlite:
            break;

        default:
            break;
        }

        return nullptr;
    }

    std::unique_ptr<Sql::Selector> Connection::createSelector()
    {
        switch (dbType_)
        {
        case Sql::DbType::Postgres:
        {
            PqImpl::Connection* pConnection = dynamic_cast<PqImpl::Connection*>(this);
            return std::make_unique<PqImpl::Selector>(pConnection->connection(), false);
        }
        break;

        case Sql::DbType::Sqlite:
            break;

        default:
            break;
        }

        return nullptr;
    }

    std::unique_ptr<Sql::Connection> Connection::createConnection(Sql::DbType dbType, const std::string& connectionStr)
    {
            switch (dbType)
            {
            case Sql::DbType::Postgres:
                return std::make_unique<PqImpl::Connection>(connectionStr);

            case Sql::DbType::Sqlite:
                break;

            default:
                throw std::runtime_error("Unsupported database connection");
            }

        return nullptr;
    }
}

namespace Sql
{
    std::unique_ptr<Connection> createConnection(DbType dbType, const std::string& connectionStr)
    {
        std::unique_ptr<DbImpl::Connection> connection;
        switch (dbType)
        {
        case DbType::Postgres:
            connection = std::make_unique<PqImpl::Connection>(connectionStr);
            break;

        case DbType::Sqlite:
            break;

        default:
            break;
        }

        connection->connect();
        return connection;
    }
}