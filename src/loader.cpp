#include "loader.h"
#include "object.h"
#include <fstream>
#include <ios>
#include <iostream>
#include <stdexcept>

using std::ifstream, std::ios, std::streamsize, std::cerr, std::endl,
    std::ofstream;

uint32_t bytes_to_uint(vector<uint8_t> &bytes, size_t offset) {
  if (offset + 3 >= bytes.size())
    return 0;

  return static_cast<uint32_t>((bytes[offset + 3] << 24) |
                               (bytes[offset + 2] << 16) |
                               (bytes[offset + 1] << 8) | (bytes[offset]));
}

bool check_header(vector<uint8_t> &bytes) {
  if (bytes.size() < 28)
    return false;

  return bytes[0] == 0xc1 && bytes[1] == 0xa7 && bytes[2] == 0x00 &&
         bytes[3] == 0x00;
}

File load_from_file(string path) {
  ifstream file(path, ios::binary | ios::ate);
  if (!file.is_open())
    throw std::runtime_error("Failed to open file");

  streamsize size = file.tellg();
  file.seekg(0, ios::beg);

  vector<uint8_t> buffer(size);

  if (!file.read(reinterpret_cast<char *>(buffer.data()), size))
    throw std::runtime_error("Failed to read file");

  if (!check_header(buffer))
    throw std::runtime_error("Invalid clarity file");

  unsigned short major_version =
      static_cast<unsigned short>((buffer[4] << 8) | (buffer[5]));
  unsigned short minor_version =
      static_cast<unsigned short>((buffer[6] << 8) | (buffer[7]));
  uint32_t pc = bytes_to_uint(buffer, 24);
  uint32_t bytecode_offset = bytes_to_uint(buffer, 8);
  uint32_t bytecode_size = bytes_to_uint(buffer, 12);
  uint32_t const_pool_offset = bytes_to_uint(buffer, 16);
  uint32_t const_pool_size = bytes_to_uint(buffer, 20);

  vector<uint8_t> bytecode_slice(buffer.begin() + bytecode_offset,
                                 buffer.begin() + bytecode_offset +
                                     bytecode_size);
  const vector<uint8_t> bytecode = bytecode_slice;

  vector<uint8_t> const_pool_slice(buffer.begin() + const_pool_offset,
                                   buffer.begin() + const_pool_offset +
                                       const_pool_size);

  vector<Object> const_pool_decoded;

  try {
    while (const_pool_slice.size() > 0)
      const_pool_decoded.push_back(decode_object(const_pool_slice));
  } catch (const std::exception &e) {
    throw e;
  }

  const vector<Object> const_pool = const_pool_decoded;

  File file_data = {major_version, minor_version, bytecode, const_pool, pc};

  file.close();

  return file_data;
}

void generate_file(File file) {
  vector<char> file_data;
  vector<uint8_t> const_pool;

  try {
    for (auto obj : file.const_pool)
      encode_object(obj, const_pool);
  } catch (const std::exception &e) {
    cerr << e.what() << endl;
    exit(1);
  }

  uint32_t magic_number = 0xa7c1;
  uint32_t bytecode_offset = sizeof(magic_number) + sizeof(file.major_version) +
                             sizeof(file.minor_version) + 5 * sizeof(uint32_t);
  uint32_t bytecode_size = file.bytecode.size();
  uint32_t const_pool_offset = bytecode_offset + bytecode_size;
  uint32_t const_pool_size = const_pool.size();

  file_data.insert(
      file_data.end(), reinterpret_cast<const char *>(&magic_number),
      reinterpret_cast<const char *>(&magic_number) + sizeof(magic_number));

  file_data.insert(file_data.end(),
                   reinterpret_cast<const char *>(&file.major_version),
                   reinterpret_cast<const char *>(&file.major_version) +
                       sizeof(file.major_version));

  file_data.insert(file_data.end(),
                   reinterpret_cast<const char *>(&file.minor_version),
                   reinterpret_cast<const char *>(&file.minor_version) +
                       sizeof(file.minor_version));

  file_data.insert(file_data.end(),
                   reinterpret_cast<const char *>(&bytecode_offset),
                   reinterpret_cast<const char *>(&bytecode_offset) +
                       sizeof(bytecode_offset));

  file_data.insert(
      file_data.end(), reinterpret_cast<const char *>(&bytecode_size),
      reinterpret_cast<const char *>(&bytecode_size) + sizeof(bytecode_size));

  file_data.insert(file_data.end(),
                   reinterpret_cast<const char *>(&const_pool_offset),
                   reinterpret_cast<const char *>(&const_pool_offset) +
                       sizeof(const_pool_offset));

  file_data.insert(file_data.end(),
                   reinterpret_cast<const char *>(&const_pool_size),
                   reinterpret_cast<const char *>(&const_pool_size) +
                       sizeof(const_pool_size));

  file_data.insert(file_data.end(), reinterpret_cast<const char *>(&file.pc),
                   reinterpret_cast<const char *>(&file.pc) + sizeof(file.pc));

  file_data.insert(file_data.end(), file.bytecode.begin(), file.bytecode.end());
  file_data.insert(file_data.end(), const_pool.begin(), const_pool.end());

  ofstream out_file("out.bin", ios::binary);
  out_file.write(file_data.data(), file_data.size());
  out_file.close();
}
