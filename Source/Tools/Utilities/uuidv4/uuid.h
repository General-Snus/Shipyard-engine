#ifndef __UUID__
#define __UUID__

#include <random>
#include <string>
#include <array>

namespace uuid::v4
{
    // Encaasulate the genaeration of a Version 4 UUID object
    // A Version 4 UUID is a universally unique identifier that is generated using random numbers.
    class UUID
    {
		friend struct std::hash<uuid::v4::UUID>;
    public:
        // Factory method for creating UUID object.
        static UUID New()
        {
            UUID uuid;
            std::random_device rd;
            std::mt19937 engine{rd()};
            std::uniform_int_distribution<int> dist{0, 256}; //Limits of the interval

            for (int index = 0; index < 16; ++index)
            {
                uuid._data[index] = (unsigned char)dist(engine);
            }

            uuid._data[6] = ((uuid._data[6] & 0x0f) | 0x40); // Version 4
            uuid._data[8] = ((uuid._data[8] & 0x3f) | 0x80); // Variant is 10

            return uuid;
        }

        // Returns UUID as formatted string
        std::string String() const
        {
            // Formats to "0065e7d7-418c-4da4-b4d6-b54b6cf7466a"
            char buffer[256] = {0};
            std::snprintf(buffer, 255,
                          "%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x",
                          _data[0], _data[1], _data[2], _data[3],
                          _data[4], _data[5],
                          _data[6], _data[7],
                          _data[8], _data[9],
                          _data[10], _data[11], _data[12], _data[13], _data[14], _data[15]);

            std::string uuid = buffer;

            return uuid;
        }
		 
		friend bool operator==(const UUID& lhs,const UUID& rhs) {
			__m128i x = _mm_load_si128((const __m128i*)lhs._data.data());
			__m128i y = _mm_load_si128((const __m128i*)rhs._data.data());

			__m128i neq = _mm_xor_si128(x,y);
			return _mm_test_all_zeros(neq,neq);
		} 

        UUID() {}

    private:

		alignas(16)  std::array<unsigned char,16> _data = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    };
};

namespace std {
template <>
struct hash<uuid::v4::UUID> {
	std::size_t operator()(const uuid::v4::UUID& uuid) const {
		std::size_t seed = 0;
		for(const auto& byte : uuid._data) {
			seed ^= std::hash<unsigned char>{}(byte)+ 0x9e3779b9 + (seed << 6) + (seed >> 2);
		}
		return seed;
	}
};
}

#endif
