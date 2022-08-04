#pragma once
#include "Perlin.h"

namespace EngineCore {
	struct Packet {
	public:
		Uint32 pointer = 0;

		IPaddress address;

		Uint16 ack = NULL;
		Uint16 bitfield = NULL;

		Uint16 code = NULL;
		Uint16 packetID = NULL;

		byte* data = NULL;
		Uint32 len = NULL;

		static Packet* createFromUDP(UDPpacket* packet) {
			Packet* self = new Packet();

			self->data = packet->data;
			self->len = packet->len;
			self->pointer = 0;
			self->address = packet->address;

			self->code = self->read<Uint16>();
			self->packetID = self->read<Uint16>();
			self->ack = self->read<Uint16>();
			self->bitfield = self->read<Uint16>();

			return self;
		}

		static Packet create(Uint32 len, Uint32 pointer=8) {
			Packet self;

			self.pointer = pointer;
			self.len = len + pointer;

			self.data = new byte[self.len];

			return self;
		}

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