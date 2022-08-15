#pragma once
#include "../../Math/Perlin.h"

namespace WindEngine {
	struct Packet {
		Uint32 pointer = 0;

		IPaddress address = IPaddress();

		Uint16 ack = NULL;
		Uint16 bitfield = NULL;

		Uint16 code = NULL;
		Uint16 packetID = NULL;

		byte* data = NULL;
		Uint32 len = NULL;

		static Packet* createFromUDP(UDPpacket* packet);
		static Packet create(Uint32 len, Uint32 pointer = 8);

		template <typename T>
		void write(T value) {
			if (pointer < len)
				memcpy(&data[pointer], &value, sizeof(T));
			else
				throw std::exception("index out");

			pointer += sizeof(T);
		}

		template <typename T>
		T read() {
			T value;

			if (pointer < len)
				memcpy(&value, &data[pointer], sizeof(T));
			else
				throw std::exception("index out");

			pointer += sizeof(T);
			return value;
		}
	};
}