#pragma once
#include "Perlin.h"

namespace EngineCore {
	struct Packet {
	private:
		Uint32 pointer = 0;
	public:
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

		static Packet create(Uint32 len) {
			Packet self;

			self.pointer = 8;
			self.len = len + 8;

			self.data = new byte[self.len];

			return self;
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