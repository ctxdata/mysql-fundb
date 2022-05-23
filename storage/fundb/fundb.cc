#include "storage/fundb/fundb.h"

bool fundb_table::add(uint32_t id, uint32_t val)  {
    fundb_data_buffer* idx = data;
    while (idx != nullptr) {
        if (idx->id == id) {
            if (idx->count < 1024) {
                idx->data[idx->count++] = val;
                return true;
            } else {
                return false;
            }
        }
        idx = idx->next;
    }

    fundb_data_buffer* temp = new fundb_data_buffer;
    temp->id = id;
    temp->count = 0;
    temp->data[temp->count++] = val;
    temp->next = data;
    data = temp;
    ++rows;

    return true;
}

const fundb_table::iterator& fundb_table::begin() {
    return iterator(data);
}

const fundb_table::iterator& fundb_table::end() {
    return last;
}

int fundb_table::create() {
    ofstream dataOut, out;
    dataOut.open(tableName + FDB_EXT, ios::out | ios::binary);
    out.open(tableName + FMD_EXT, std::ios::out | ios::binary);

    metadata_header metadata;
    metadata.count = 0;
    strcpy(metadata.reserved, FUNDB_RESERVED_STRING);
    metadata.reserved[10] = '\0';
    metadata.reserved[11] = 0xFC;
    out.write((char*)&metadata, sizeof(metadata_header));

    dataOut.close();
    out.close();
    return 0;
}

void fundb_table::open() {
    metadata_header metadata;
    ifstream in, dataIn;
    in.open(tableName + FMD_EXT, ios::in | ios::binary);
    dataIn.open(tableName + FDB_EXT, ios::in | ios::binary);
    if (in.is_open()) {
        in.read((char*)&metadata, sizeof(metadata_header));
        if (strcasecmp(metadata.reserved, FUNDB_RESERVED_STRING) != 0) {
            std::cout << "Invalid metadata file" << std::endl;
            return;
        }

        rows = metadata.count;

        uint32_t pt[1024];
        for (int i=0; i<metadata.count; i++) {
            metadata_data d;
            in.read((char*)&d, sizeof(metadata_data));
            if (d.length > sizeof(uint32_t) * 1024) {
                // invalid
                return;
            }
            dataIn.read((char*)pt, d.length);

            fundb_data_buffer* temp = data;
            data = new fundb_data_buffer;
            data->id = d.id;
            data->next = temp;
            data->count = d.length / sizeof(uint32_t);
            memcpy(data->data, pt, d.length);
        }
    }
}

int fundb_table::drop() {
    close();
    return 0;
}

int fundb_table::remove(uint32_t id, uint32_t val) {
    fundb_data_buffer* idx = data;
    fundb_data_buffer* pp = nullptr;
    while (idx != nullptr) {
        if (idx->id == id) {
            for (int i=0; i<idx->count; i++) {
                if (idx->data[i] == val) {
                    if (idx->count-1-i > 0)
                        memcpy(idx->data+i, idx->data+i+1, sizeof(*(idx->data)) * (idx->count-1-i));
                    idx->count--;

                    break;
                }
            }

            if (idx->count == 0) {
                if (pp == nullptr) {
                    data = data->next;
                } else {
                    pp->next = idx->next;
                }
                delete idx;
            }
            return 0;
        }
        pp = idx;
        idx = idx->next;
    }

    return -1;
}

void fundb_table::close() {
    metadata_header metadata;
    metadata.count = rows;
    strcpy(metadata.reserved, FUNDB_RESERVED_STRING);
    metadata.reserved[10] = '\0';
    metadata.reserved[11] = 0xFC;

    ofstream dataOut, out;
    dataOut.open(tableName + FDB_EXT, ios::out | ios::binary);
    out.open(tableName + FMD_EXT, std::ios::out | ios::binary);

    out.write((char*)&metadata, sizeof(metadata_header));

    uint32_t offset = 0;

    fundb_data_buffer* idx = data;
    for (int i=0; i<rows; i++) {
        metadata_data md;
        md.id = idx->id;
        md.offset = offset;
        md.length = sizeof(uint32_t) * idx->count;
        dataOut.write((char*)idx->data, md.length);

        offset += md.length;
        out.write((char*)&md, sizeof(metadata_data));
        idx = idx->next;
    }

    out.close();
    dataOut.close();
}