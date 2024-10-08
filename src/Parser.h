#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include <bit>
#include <memory>
#include <unordered_map>
#include <set>
#include <string.h>
#include <arpa/inet.h>

#define CODE_OFFSET 4
#define NAME_SIZE 8
#define POLYGON_CODE 5

// for polygon only
#define COLOR_NAME_OFFSET 20 - CODE_OFFSET - NAME_SIZE
#define MATERIAL_INDEX_OFFSET 30 - CODE_OFFSET - NAME_SIZE

static std::set<short> print_opcodes = {1, 2, 4, 5};

struct Record
{
	bool ParceData(std::ifstream *source) {
		// read start of the header
		source->read(reinterpret_cast<char*>(&opcode), sizeof(opcode));
		source->read(reinterpret_cast<char*>(&record_length), sizeof(record_length));

		// convert to big-endian
		opcode = ntohs(opcode);
		record_length = ntohs(record_length) - CODE_OFFSET;

		// parse name if necessary
		if (print_opcodes.find(opcode) != print_opcodes.cend()) {
			source->read(name, sizeof(name));
			record_length -= NAME_SIZE;
		}

		// read Record data
		data = std::make_unique<std::byte[]>(record_length);
		source->read(reinterpret_cast<char*>(data.get()), sizeof(std::byte) * record_length);

		return source->good();
	}

	void PrintData() const {
		std::cout << name;
		if (opcode == POLYGON_CODE) {
			unsigned short color_name_index;
			short material_index;

			// read data
			memcpy(&color_name_index, data.get() + COLOR_NAME_OFFSET, sizeof(color_name_index));
			memcpy(&material_index, data.get() + MATERIAL_INDEX_OFFSET, sizeof(material_index));

			// convert to big-endian
			color_name_index = ntohs(color_name_index);
			material_index = ntohs(material_index);

			std::cout << " color_name_index: " << color_name_index << "| material_index: " << material_index;
		}

		std::cout << std::endl;
	}

	short opcode = 0;
	unsigned short record_length = 0;
	char name[NAME_SIZE] = "\0";
	std::unique_ptr<std::byte[]> data;
};

struct Parser
{
	std::unordered_map<size_t, std::vector<Record>> opcode_to_record;

	void PrintData() const {
		for (const auto &num: print_opcodes) {
			if (opcode_to_record.count(num) != 0) {
				for (const auto &record: opcode_to_record.at(num)) {
					record.PrintData();
				}
			}
		}
	}
};
