#pragma once

#include <vector>
#include <string>
#include "data.h"
#include "../include/statement.h"

namespace DbImpl
{
    class Statement : public Sql::Statement
    {
    protected:
        std::string query_;
        std::vector<BindData> data_;

    protected:
        void replaceBindedParameter(const std::string& bindedName, bool isNull, int& index);
        const char* query() const;
        const std::vector<BindData>& data() const;

    public:
        Statement() = default;

        void prepare(const std::string& query) override;
        void append(const std::string& appendQuery) override;
        void bind(const std::string& key, int32_t value) override;
        void bind(const std::string& key, int64_t value) override;
        void bind(const std::string& key, bool value) override;
        void bind(const std::string& key, double value) override;
        void bind(const std::string& key, const std::string& value) override;
        void bind(const std::string& key, const char * value) override;
        void bindBlob(const std::string& key, const std::vector<unsigned char>& value) override;
        void bind(const std::string& key, int32_t value, int32_t nullValue) override;
        void bind(const std::string& key, int64_t value, int64_t nullValue) override;
        void bind(const std::string& key, bool value, bool nullValue) override;
        void bind(const std::string& key, double value, double nullValue) override;
        void bind(const std::string& key, const std::string& value, std::string nullValue) override;
        void bind(const std::string& key, const char * value, const char * nullValue) override;
        void bindBlob(const std::string& key, const std::vector<unsigned char>& value, std::vector<unsigned char> nullValue) override;
        void bindAndAppend(const std::string& appendedQuery, const std::string& key, int32_t value) override;
        void bindAndAppend(const std::string& appendedQuery, const std::string& key, int64_t value) override;
        void bindAndAppend(const std::string& appendedQuery, const std::string& key, bool value) override;
        void bindAndAppend(const std::string& appendedQuery, const std::string& key, double value) override;
        void bindAndAppend(const std::string& appendedQuery, const std::string& key, const std::string& value) override;
        void bindAndAppend(const std::string& appendedQuery, const std::string& key, const char* value) override;
        void bindBlobAndAppend(const std::string& appendedQuery, const std::string& key, const std::vector<unsigned char>& value) override;
        void bindAndAppend(const std::string& appendedQuery, const std::string& key, int32_t value, int32_t nullValue) override;
        void bindAndAppend(const std::string& appendedQuery, const std::string& key, int64_t value, int64_t nullValue) override;
        void bindAndAppend(const std::string& appendedQuery, const std::string& key, bool value, bool nullValue) override;
        void bindAndAppend(const std::string& appendedQuery, const std::string& key, double value, double nullValue) override;
        void bindAndAppend(const std::string& appendedQuery, const std::string& key, const std::string& value, std::string nullValue) override;
        void bindAndAppend(const std::string& appendedQuery, const std::string& key, const char* value, const char* nullValue) override;
        void bindBlobAndAppend(const std::string& appendedQuery, const std::string& key, const std::vector<unsigned char>& value, std::vector<unsigned char> nullValue) override;

    private:
        void bindNull(const std::string& key)
        {
            BindData data;
            data.name_ = key;
            data_.push_back(std::move(data));
        }

        template<class T> void bindData(const std::string& key, const T& value)
        {
            BindData data;
            data.setValue(key, value);
            data_.push_back(std::move(data));
        }

        template<class T> void bindData(const std::string& key, const T& value, T nullValue)
        {
            if(nullValue == value)
                bindNull(key);
            else
                bindData(key, value);
        }
    };
}