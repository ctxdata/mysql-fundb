#pragma once

#include <sys/types.h>
#include <vector>
#include "mysql/psi/mysql_file.h"
#include "my_io.h"
#include <fstream>
#include <iostream>
#include <string>
#include <tuple>
using std::tuple;
using std::string;
using std::ofstream;
using std::ifstream;
using std::ios;

#define PAGE_ROW_LIMIT 32
#define ROW_VAL_LIMIT 15

#define FUNDB_RESERVED_STRING "FunDB@2022"
#define FDB_EXT ".fdb"
#define FMD_EXT ".fmd"

typedef struct metadata_header {
    char reserved[12];
    uint32_t count;
} metadata_header;

typedef struct metadata_data {
    uint32_t id;
    uint32_t offset;
    uint32_t length;
} metadata_data;

/**
 * @brief This is memory buffer representative of row data
 * 
 */
typedef struct fundb_data_buffer {
    uint32_t id;
    uint32_t count;
    uint32_t data[1024];
    struct fundb_data_buffer* next;
} fundb_data_buffer;

/**
 * @brief For future use, we may need page
 * 

struct fundb_page {
    uint32_t index;
    uint32_t count;
    struct fundb_row* first;
    struct fundb_page* prev;
    struct fundb_page* next;
}; */

class row_iterator{
private:
    fundb_data_buffer* data;
    uint32_t idx;
public:
    row_iterator(): data(nullptr), idx(0) {}
    row_iterator(fundb_data_buffer* ptr) : data(ptr), idx(0) {}
    tuple<uint32_t, uint32_t> operator*() {
        if (data == nullptr || idx >= data->count) {
            return std::make_tuple(-1, -1);
        }

        return std::make_tuple(data->id, data->data[idx]);
    }

    row_iterator& operator++(int) {
        if (++idx >= data->count) {
            data = data->next;
            idx = 0;
        }

        return *this;
    }

    bool operator==(const row_iterator& rit) {
        if (data == rit.data && idx == rit.idx) {
            return true;
        }

        return false;
    }
};

class fundb_table {
private:
    fundb_data_buffer* data;
    uint32_t rows;
    string tableName;
public:
    typedef row_iterator iterator;
    const fundb_table::iterator last;
    fundb_table(const string& table): data(nullptr), last(nullptr), rows(0), tableName(table) {}
    bool add(uint32_t id, uint32_t val);
    int remove(uint32_t id, uint32_t val);

    void open();

    const fundb_table::iterator& begin();
    const fundb_table::iterator& end();

    int create();

    int drop();

    void close();
};