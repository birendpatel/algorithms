/*
* Author: Biren Patel
* Description: Implementation file for pseudo random number generator.
*/

#include "random.h"

#include <string.h>
#include <math.h>

/******************************************************************************/
//static prototypes

static uint64_t mix (uint64_t value);

/******************************************************************************/

#define CPUID(leaf, subleaf)                                                   \
        __asm__ volatile                                                       \
        (                                                                      \
            "cpuid"                                                            \
            : "=a" (reg.eax), "=b" (reg.ebx), "=c" (reg.ecx), "=d" (reg.edx)   \
            : "a" ((uint32_t) leaf), "c" ((uint32_t) subleaf)                  \
        )                                                                      \

int rng_verify_hardware(void)
{
    struct
    {
        uint32_t eax;
        uint32_t ebx;
        uint32_t edx;
        uint32_t ecx;
    }
    reg = {.eax = 0, .ebx = 0, .edx = 0, .ecx = 0};

    //EAX 0 = manufacturer id formed by EBX-EDX-ECX
    CPUID(0,0);
    if (memcmp(&reg.ebx, "GenuineIntel", 12) != 0) return RNG_NO_INTEL;

    //impossible to execute RDSEED check on this machine
    if (reg.eax < 7) return RNG_MAX_EAX_PARAMETER_TOO_LOW;

    //EAX 1 = feature flags, bit 30 of ECX register must be set
    CPUID(1,0);
    if ((reg.ecx >> 30 & 1) == 0) return RNG_NO_RDRAND;

    //EAX 7 = extended features, bit 18 of EBX register must be set
    CPUID(7,0);
    if ((reg.ebx >> 18 & 1) == 0) return RNG_NO_RDSEED;

    return RNG_YES_RDRAND_RDSEED;
}

/******************************************************************************/

bool rng_rdseed64(uint64_t *seed, const uint8_t retry)
{
    if (seed == NULL) return false;

    uint8_t carry_flag;

    __asm__ volatile
    (
        "rdseed     %[seed]\n"
        "setc       %[carry_flag]\n"
        : [seed] "=r" (*seed), [carry_flag] "=q" (carry_flag)
    );

    //first request failed, now try again with pause instructions
    if (!carry_flag)
    {
        for (uint8_t i = 0; i < retry; ++i)
        {
            __asm__ volatile
            (
                "rdseed     %[seed]\n"
                "setc       %[carry_flag]\n"
                "pause\n"
                : [seed] "=r" (*seed), [carry_flag] "=q" (carry_flag)
            );

            if (carry_flag) break;
        }
    }

    return (bool) carry_flag;
}

/******************************************************************************/
//Marsaglia 64-bit Xorshift (temporary placeholder)

uint64_t rng_generator(uint64_t *state)
{
	uint64_t x = *state;
    
	x ^= x << 13;
	x ^= x >> 7;
	x ^= x << 17;
    
	return *state = x;
}


/******************************************************************************/
//invertible mix (temporary placeholder)

static uint64_t mix (uint64_t value)
{
    value = (value >> 48) | (value << 16);
    value ^= value << 30;
    value ^= value >> 15;
    value ^= value << 45;
    value = (value >> 32) | (value << 32);
    
    return value;
}

/******************************************************************************/

random_t rng_init(const uint64_t seed, const uint8_t retry)
{
    random_t rng;
    
    //set up prototypes
    rng.next = rng_generator;
    rng.rand = rng_rand;
    rng.bias = rng_bias;
    
    //set up seed
    if (seed == 0)
    {
        if (rng_rdseed64(&rng.state, retry) == false)
        {
            rng.state = 0;
        }
    }
    else
    {
        rng.state = mix(seed);
    }
    
    return rng;
}

/******************************************************************************/

uint64_t rng_rand(uint64_t *state, const uint64_t min, const uint64_t max)
{    
    //errors map to zero and degenerate cases cause early stopping
    if (state == NULL || max - min == 0) return 0;
    if (max - min < 2) return min;
    
    //sample a value from [0, scaled_max)
    uint64_t sample;
    uint64_t scaled_max = max - min - 1;
    uint64_t bitmask = ~((uint64_t) 0) >> __builtin_clzll(scaled_max);
    
    //rejection sampling on the generator output
    do 
    {
        sample = rng_generator(state) & bitmask;
    } 
    while (sample > scaled_max);
    
    //scale back to user range
    return sample + min;
}

/******************************************************************************/
//at 256 bits of resolution, at most 8 rng_generator calls are required in any
//single call of this function. Alternatives to this manual bitwise table are
//to use an arithmetic decoder or tap into the AVX/AVX2/AVX512 GCC intrinsics.

uint64_t rng_bias (uint64_t *state, const uint8_t resolution)
{
    if (state == NULL) return 0;
    
    #define b(i) (rng_generator(state))
    
    switch (resolution)
    {
        case 0  : //0.0
                return (uint64_t) 0;
        case 1  : //0.00390625
                return b(1) & b(2) & b(3) & b(4) & b(5) & b(6) & b(7) & b(8);
        case 2  : //0.0078125
                return b(1) & b(2) & b(3) & b(4) & b(5) & b(6) & b(7);
        case 3  : //0.01171875
                return b(1) & b(2) & b(3) & b(4) & b(5) & b(6) & (b(7) | b(8));
        case 4  : //0.015625
                return b(1) & b(2) & b(3) & b(4) & b(5) & b(6);
        case 5  : //0.01953125
                return (((b(1) & b(2)) | b(3)) & b(4)) & b(5) & b(6) & b(7) & b(8);
        case 6  : //0.0234375
                return b(1) & b(2) & b(3) & b(4) & b(5) & (b(6) | b(7));
        case 7  : //0.02734375
                return (b(1) | b(2) | b(3)) & b(4) & b(5) & b(6) & b(7) & b(8);
        case 8  : //0.03125
                return b(1) & b(2) & b(3) & b(4) & b(5);
        case 9  : //0.03515625
                return 0;
        case 10 : //0.0390625
                return 0;
        case 11 : //0.04296875
                return 0;
        case 12 : //0.046875
                return (b(1) | b(2)) & b(3) & b(4) & b(5) & b(6);
        case 13 : //0.05078125
                return 0;
        case 14 : //0.0546875
                return (b(1) | b(2) | b(3)) & b(4) & b(5) & b(6) & b(7);
        case 15 : //0.05859375
                return (b(1) | b(2) | b(3) | b(4)) & b(5) & b(6) & b(7) & b(8);
        case 16 : //0.0625
                return b(1) & b(2) & b(3) & b(4);
        case 17 : //0.06640625
                return 0;
        case 18 : //0.0703125
                return 0;
        case 19 : //0.07421875
                return 0;
        case 20 : //0.078125
                return 0;
        case 21 : //0.08203125
                return 0;
        case 22 : //0.0859375
                return 0;
        case 23 : //0.08984375
                return 0;
        case 24 : //0.09375
                return (b(1) | b(2)) & b(3) & b(4) & b(5);
        case 25 : //0.09765625
                return 0;
        case 26 : //0.1015625
                return 0;
        case 27 : //0.10546875
                return 0;
        case 28 : //0.109375
                return (b(1) | b(2) | b(3)) & b(4) & b(5) & b(6);
        case 29 : //0.11328125
                return 0;
        case 30 : //0.1171875
                return (b(1) | b(2) | b(3) | b(4)) & b(5) & b(6) & b(7);
        case 31 : //0.12109375
                return (b(1) | b(2) | b(3) | b(4) | b(5)) & b(6) & b(7) & b(8);
        case 32 : //0.125
                return b(1) & b(2) & b(3);
        case 33 : //0.12890625
                return ((b(1) & b(2) & b(3) & b(4) & b(5)) | b(6)) & b(7) & b(8);
        case 34 : //0.1328125
                return 0;
        case 35 : //0.13671875
                return 0;
        case 36 : //0.140625
                return 0;
        case 37 : //0.14453125
                return 0;
        case 38 : //0.1484375
                return 0;
        case 39 : //0.15234375
                return 0;
        case 40 : //0.15625
                return 0;
        case 41 : //0.16015625
                return 0;
        case 42 : //0.1640625
                return 0;
        case 43 : //0.16796875
                return 0;
        case 44 : //0.171875
                return 0;
        case 45 : //0.17578125
                return 0;
        case 46 : //0.1796875
                return 0;
        case 47 : //0.18359375
                return 0;
        case 48 : //0.1875
                return (b(1) | b(2)) & b(3) & b(4);
        case 49 : //0.19140625
                return 0;
        case 50 : //0.1953125
                return 0;
        case 51 : //0.19921875
                return 0;
        case 52 : //0.203125
                return 0;
        case 53 : //0.20703125
                return 0;
        case 54 : //0.2109375
                return 0;
        case 55 : //0.21484375
                return 0;
        case 56 : //0.21875
                return (b(1) | b(2) | b(3)) & b(4) & b(5);
        case 57 : //0.22265625
                return 0;
        case 58 : //0.2265625
                return 0;
        case 59 : //0.23046875
                return 0;
        case 60 : //0.234375
                return (b(1) | b(2) | b(3) | b(4)) & b(5) & b(6);
        case 61 : //0.23828125
                return 0;
        case 62 : //0.2421875
                return (b(1) | b(2) | b(3) | b(4) | b(5)) & b(6) & b(7);
        case 63 : //0.24609375
                return (b(1) | b(2) | b(3) | b(4) | b(5) | b(6)) & b(7) & b(8);
        case 64 : //0.25
                return b(1) & b(2);
        case 65 : //0.25390625
                return ((b(1) & b(2) & b(3) & b(4) & b(5) & b(6)) | b(7)) & b(8);
        case 66 : //0.2578125
                return ((b(1) & b(2) & b(3) & b(4) & b(5)) | b(6)) & b(7);
        case 67 : //0.26171875
                return 0;
        case 68 : //0.265625
                return 0;
        case 69 : //0.26953125
                return 0;
        case 70 : //0.2734375
                return 0;
        case 71 : //0.27734375
                return 0;
        case 72 : //0.28125
                return 0;
        case 73 : //0.28515625
                return 0;
        case 74 : //0.2890625
                return 0;
        case 75 : //0.29296875
                return 0;
        case 76 : //0.296875
                return 0;
        case 77 : //0.30078125
                return 0;
        case 78 : //0.3046875
                return 0;
        case 79 : //0.30859375
                return 0;
        case 80 : //0.3125
                return 0;
        case 81 : //0.31640625
                return 0;
        case 82 : //0.3203125
                return 0;
        case 83 : //0.32421875
                return 0;
        case 84 : //0.328125
                return 0;
        case 85 : //0.33203125
                return 0;
        case 86 : //0.3359375
                return 0;
        case 87 : //0.33984375
                return 0;
        case 88 : //0.34375
                return 0;
        case 89 : //0.34765625
                return 0;
        case 90 : //0.3515625
                return 0;
        case 91 : //0.35546875
                return 0;
        case 92 : //0.359375
                return 0;
        case 93 : //0.36328125
                return 0;
        case 94 : //0.3671875
                return 0;
        case 95 : //0.37109375
                return 0;
        case 96 : //0.375
                return (b(1) | b(2)) & b(3);
        case 97 : //0.37890625
                return ((b(1) & b(2) & b(3) & b(4) & b(5)) | b(6) | b(7)) & b(8);
        case 98 : //0.3828125
                return 0;
        case 99 : //0.38671875
                return 0;
        case 100: //0.390625
                return 0;
        case 101: //0.39453125
                return 0;
        case 102: //0.3984375
                return 0;
        case 103: //0.40234375
                return 0;
        case 104: //0.40625
                return 0;
        case 105: //0.41015625
                return 0;
        case 106: //0.4140625
                return 0;
        case 107: //0.41796875
                return 0;
        case 108: //0.421875
                return 0;
        case 109: //0.42578125
                return 0;
        case 110: //0.4296875
                return 0;
        case 111: //0.43359375
                return 0;
        case 112: //0.4375
                return (b(1) | b(2) | b(3)) & b(4);
        case 113: //0.44140625
                return 0;
        case 114: //0.4453125
                return 0;
        case 115: //0.44921875
                return 0;
        case 116: //0.453125
                return 0;
        case 117: //0.45703125
                return 0;
        case 118: //0.4609375
                return 0;
        case 119: //0.46484375
                return 0;
        case 120: //0.46875
                return (b(1) | b(2) | b(3) | b(4)) & b(5);
        case 121: //0.47265625
                return 0;
        case 122: //0.4765625
                return 0;
        case 123: //0.48046875
                return 0;
        case 124: //0.484375
                return (b(1) | b(2) | b(3) | b(4) | b(5)) & b(6);
        case 125: //0.48828125
                return 0;
        case 126: //0.4921875
                return (b(1) | b(2) | b(3) | b(4) | b(5) | b(6)) & b(7);
        case 127: //0.49609375
                return (b(1) | b(2) | b(3) | b(4) | b(5) | b(6) | b(7)) & b(8);
        case 128: //0.5
                return b(1);
        case 129: //0.50390625
                return (b(1) & b(2) & b(3) & b(4) & b(5) & b(6) & b(7)) | b(8);
        case 130: //0.5078125
                return (b(1) & b(2) & b(3) & b(4) & b(5) & b(6)) | b(7);
        case 131: //0.51171875
                return (b(1) & b(2) & b(3) & b(4) & b(5) & (b(6) | b(7))) | b(8);
        case 132: //0.515625
                return (b(1) & b(2) & b(3) & b(4) & b(5)) | b(6);
        case 133: //0.51953125
                return 0;
        case 134: //0.5234375
                return 0;
        case 135: //0.52734375
                return 0;
        case 136: //0.53125
                return 0;
        case 137: //0.53515625
                return 0;
        case 138: //0.5390625
                return 0;
        case 139: //0.54296875
                return 0;
        case 140: //0.546875
                return 0;
        case 141: //0.55078125
                return 0;
        case 142: //0.5546875
                return 0;
        case 143: //0.55859375
                return 0;
        case 144: //0.5625
                return 0;
        case 145: //0.56640625
                return 0;
        case 146: //0.5703125
                return 0;
        case 147: //0.57421875
                return 0;
        case 148: //0.578125
                return 0;
        case 149: //0.58203125
                return 0;
        case 150: //0.5859375
                return 0;
        case 151: //0.58984375
                return 0;
        case 152: //0.59375
                return 0;
        case 153: //0.59765625
                return 0;
        case 154: //0.6015625
                return 0;
        case 155: //0.60546875
                return 0;
        case 156: //0.609375
                return 0;
        case 157: //0.61328125
                return 0;
        case 158: //0.6171875
                return 0;
        case 159: //0.62109375
                return 0;
        case 160: //0.625
                return 0;
        case 161: //0.62890625
                return (((b(1) & b(2) & b(3) & b(4) & b(5)) | b(6)) & b(7)) | b(8);
        case 162: //0.6328125
                return 0;
        case 163: //0.63671875
                return 0;
        case 164: //0.640625
                return 0;
        case 165: //0.64453125
                return 0;
        case 166: //0.6484375
                return 0;
        case 167: //0.65234375
                return 0;
        case 168: //0.65625
                return 0;
        case 169: //0.66015625
                return 0;
        case 170: //0.6640625
                return 0;
        case 171: //0.66796875
                return 0;
        case 172: //0.671875
                return 0;
        case 173: //0.67578125
                return 0;
        case 174: //0.6796875
                return 0;
        case 175: //0.68359375
                return 0;
        case 176: //0.6875
                return 0;
        case 177: //0.69140625
                return 0;
        case 178: //0.6953125
                return 0;
        case 179: //0.69921875
                return 0;
        case 180: //0.703125
                return 0;
        case 181: //0.70703125
                return 0;
        case 182: //0.7109375
                return 0;
        case 183: //0.71484375
                return 0;
        case 184: //0.71875
                return 0;
        case 185: //0.72265625
                return 0;
        case 186: //0.7265625
                return 0;
        case 187: //0.73046875
                return 0;
        case 188: //0.734375
                return 0;
        case 189: //0.73828125
                return 0;
        case 190: //0.7421875
                return 0;
        case 191: //0.74609375
                return 0;
        case 192: //0.75
                return b(1) | b(2);
        case 193: //0.75390625
                return (b(1) & b(2) & b(3) & b(4) & b(5) & b(6)) | b(7) | b(8);
        case 194: //0.7578125
                return (b(1) & b(2) & b(3) & b(4) & b(5)) | b(6) | b(7);
        case 195: //0.76171875
                return 0;
        case 196: //0.765625
                return 0;
        case 197: //0.76953125
                return 0;
        case 198: //0.7734375
                return 0;
        case 199: //0.77734375
                return 0;
        case 200: //0.78125
                return 0;
        case 201: //0.78515625
                return 0;
        case 202: //0.7890625
                return 0;
        case 203: //0.79296875
                return 0;
        case 204: //0.796875
                return 0;
        case 205: //0.80078125
                return 0;
        case 206: //0.8046875
                return 0;
        case 207: //0.80859375
                return 0;
        case 208: //0.8125
                return 0;
        case 209: //0.81640625
                return 0;
        case 210: //0.8203125
                return 0;
        case 211: //0.82421875
                return 0;
        case 212: //0.828125
                return 0;
        case 213: //0.83203125
                return 0;
        case 214: //0.8359375
                return 0;
        case 215: //0.83984375
                return 0;
        case 216: //0.84375
                return 0;
        case 217: //0.84765625
                return 0;
        case 218: //0.8515625
                return 0;
        case 219: //0.85546875
                return 0;
        case 220: //0.859375
                return 0;
        case 221: //0.86328125
                return 0;
        case 222: //0.8671875
                return 0;
        case 223: //0.87109375
                return 0;
        case 224: //0.875
                return b(1) | b(2) | b(3);
        case 225: //0.87890625
                return (b(1) & b(2) & b(3) & b(4) & b(5)) | b(6) | b(7) | b(8);
        case 226: //0.8828125
                return 0;
        case 227: //0.88671875
                return 0;
        case 228: //0.890625
                return 0;
        case 229: //0.89453125
                return 0;
        case 230: //0.8984375
                return 0;
        case 231: //0.90234375
                return 0;
        case 232: //0.90625
                return 0;
        case 233: //0.91015625
                return 0;
        case 234: //0.9140625
                return 0;
        case 235: //0.91796875
                return 0;
        case 236: //0.921875
                return 0;
        case 237: //0.92578125
                return 0;
        case 238: //0.9296875
                return 0;
        case 239: //0.93359375
                return 0;
        case 240: //0.9375
                return b(1) | b(2) | b(3) | b(4);
        case 241: //0.94140625
                return 0;
        case 242: //0.9453125
                return 0;
        case 243: //0.94921875
                return 0;
        case 244: //0.953125
                return 0;
        case 245: //0.95703125
                return 0;
        case 246: //0.9609375
                return 0;
        case 247: //0.96484375
                return 0;
        case 248: //0.96875
                return b(1) | b(2) | b(3) | b(4) | b(5);
        case 249: //0.97265625
                return 0;
        case 250: //0.9765625
                return 0;
        case 251: //0.98046875
                return 0;
        case 252: //0.984375
                return b(1) | b(2) | b(3) | b(4) | b(5) | b(6);
        case 253: //0.98828125
                return 0;
        case 254: //0.9921875
                return b(1) | b(2) | b(3) | b(4) | b(5) | b(6) | b(7);
        case 255: //0.99609375
                return b(1) | b(2) | b(3) | b(4) | b(5) | b(6) | b(7) | b(8);
                
        default :
                return 0;
    }
    
    #undef b
}
