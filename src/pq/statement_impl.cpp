#include "statement_impl.h"
#include <string.h>

namespace
{
    template <class T> T swapEndian(T u)
    {
        union
        {
            T u;
            unsigned char u8[sizeof(T)];
        } source, dest;

        source.u = u;

        for (size_t k = 0; k < sizeof(T); k++)
            dest.u8[k] = source.u8[sizeof(T) - k - 1];

        return dest.u;
    }

    template <class T> void appendBinaryPqBind(T value, std::vector<char*>& values, std::vector<int>& lengths, std::vector<int>& binaries)
    {
        char* valueStr = new char[sizeof(T)];
        *((T*)valueStr) = swapEndian<T>(value);
  
        values.push_back(valueStr);
        lengths.push_back(int(sizeof(T)));
        binaries.push_back(int(1));
    }

    void appendStringPqBind(const char* value, std::vector<char*>& values, std::vector<int>& lengths, std::vector<int>& binaries)
    {
        size_t sz = strlen(value);
        char* valueStr = new char[sz + 1];
        strcpy(valueStr, value);
        values.push_back(valueStr);
        lengths.push_back(int(sz));
        binaries.push_back(int(0));
    }
}

namespace PqImpl
{
    Statement::Statement(PGconn* conn)
        : conn_(conn)
    {
    }

    PGresult* Statement::execute()
    {
        std::vector<char*> values;
        std::vector<int> lengths, binaries;
        const std::vector<DbImpl::BindData>& data = DbImpl::Statement::data();

        int index = 1;
        for (const DbImpl::BindData& _data : data)
            replaceBindedParameter(_data.name_, _data.type_ == DbImpl::DataType::Null, index);

        for (const DbImpl::BindData& _data : data)
        {
            switch (_data.type_)
            {
            case DbImpl::DataType::Null:
                break;

            case DbImpl::DataType::Bool:
                appendBinaryPqBind(_data.value<bool>(), values, lengths, binaries);
                break;

            case DbImpl::DataType::Int:
                appendBinaryPqBind(_data.value<int32_t>(), values, lengths, binaries);
                break;

            case DbImpl::DataType::BigInt:
                appendBinaryPqBind(_data.value<int64_t>(), values, lengths, binaries);
                break;

            case DbImpl::DataType::Double:
                appendBinaryPqBind(_data.value<double>(), values, lengths, binaries);
                break;

            case DbImpl::DataType::String:
                appendStringPqBind(_data.value<std::string>().c_str(), values, lengths, binaries);
                break;

            case DbImpl::DataType::Blob:
            {
                size_t outLength = 0;
                auto dataVector = _data.value<std::vector<unsigned char>>();
                unsigned char* dataStr = PQescapeByteaConn(conn_, dataVector.data(), dataVector.size(), &outLength);
                if (dataStr == nullptr)
                {
                    throw std::runtime_error("Unable to escape blob");
                }
                else
                {
                    std::string _str((const char*)dataStr, outLength);
                    appendStringPqBind(_str.c_str(), values, lengths, binaries);
                    PQfreemem(dataStr);
                }
            }
            break;

            default:
                throw std::runtime_error("Unsupported binding parameter type");
            }
        }

        PGresult* res = nullptr;
        if (values.empty())
        {
            res = PQexec(conn_, query());
        }
        else
        {
            res = PQexecParams(conn_, query(),
                values.size(), //number of parameters
                NULL, //ignore the Oid field
                values.data(), //values to substitute $1 and $2
                lengths.data(), //the lengths, in bytes, of each of the parameter values
                binaries.data(), //whether the values are binary or not
                0); //we want the result in text format

            for (size_t i = 0; i < values.size(); i++)
            {
                char* pData = values[i];
                if (pData)
                    delete[]pData;
            }
        }

        return res;
    }
}
