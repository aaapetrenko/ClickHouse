#pragma once

#include "config.h"

#if USE_AWS_S3

#include <TableFunctions/ITableFunction.h>
#include <Storages/StorageS3.h>


namespace DB
{

class Context;
class TableFunctionS3Cluster;

/* s3(source, [access_key_id, secret_access_key,] format, structure[, compression]) - creates a temporary storage for a file in S3.
 */
class TableFunctionS3 : public ITableFunction
{
public:
    static constexpr auto name = "s3";
    std::string getName() const override
    {
        return name;
    }
    bool hasStaticStructure() const override { return configuration.structure != "auto"; }

    bool needStructureHint() const override { return configuration.structure == "auto"; }

    void setStructureHint(const ColumnsDescription & structure_hint_) override { structure_hint = structure_hint_; }

    bool supportsReadingSubsetOfColumns() override;

    std::unordered_set<String> getVirtualsToCheckBeforeUsingStructureHint() const override
    {
        return {"_path", "_file"};
    }

protected:
    friend class TableFunctionS3Cluster;

    StoragePtr executeImpl(
        const ASTPtr & ast_function,
        ContextPtr context,
        const std::string & table_name,
        ColumnsDescription cached_columns) const override;

    const char * getStorageTypeName() const override { return "S3"; }

    ColumnsDescription getActualTableStructure(ContextPtr context) const override;
    void parseArguments(const ASTPtr & ast_function, ContextPtr context) override;

    static void parseArgumentsImpl(const String & error_message, ASTs & args, ContextPtr context, StorageS3::Configuration & configuration);

    mutable StorageS3::Configuration configuration;
    ColumnsDescription structure_hint;
};

class TableFunctionCOS : public TableFunctionS3
{
public:
    static constexpr auto name = "cosn";
    std::string getName() const override
    {
        return name;
    }
private:
    const char * getStorageTypeName() const override { return "COSN"; }
};

class TableFunctionOSS : public TableFunctionS3
{
public:
    static constexpr auto name = "oss";
    std::string getName() const override
    {
        return name;
    }
private:
    const char * getStorageTypeName() const override { return "OSS"; }
};

}

#endif
