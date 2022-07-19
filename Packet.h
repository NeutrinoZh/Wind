#pragma once
#include "Perlin.h"

namespace EngineCore {
	struct Packet {
	private:
		Uint32 pointer = 0;
	public:
		
		Uint16 code		= NULL;
		Uint16 packetID = NULL;
		Uint16 c_data	= NULL;

		byte*  data = NULL;
		Uint32 len = NULL;

		Packet(byte* data, Uint32 len) {
			this->data = data;
			this->len = len;
			this->pointer = 0;

			code	 = read<Uint16>();
			packetID = read<Uint16>();
			c_data	 = read<Uint16>();
		}

		Packet(Uint32 len) {
			this->pointer = 5;
			this->len = len + 6;
			data = new byte[this->len];
		}

		template <typename T>
		void write(T value) {
			memcpy(&data[pointer], &value, sizeof(T));
			pointer += sizeof(T);
		}

		template <typename T> 
		T read() {
			T value;
			memcpy(&value, &data[pointer], sizeof(T));
			pointer += sizeof(T);
			return value;
		}
	};
}