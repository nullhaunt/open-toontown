// Bit-exact reimplementation of CPython's `random.Random` for the subset
// Toontown's seeded generation depends on: seed(int), getrandbits(k),
// _randbelow(n), randint(a, b), choice(seq), random().
//
// Algorithm mirrors CPython's _randommodule.c (MT19937 core, init_by_array
// seeding, genrand_res53) and Lib/random.py (_randbelow_with_getrandbits).
#pragma once

#include <cstdint>
#include <vector>
#include <string>

namespace Toontown::Util
{
  class PyRandom
  {
   public:
    explicit PyRandom( std::uint64_t seed )
    {
      SeedInt( seed );
    }

    // random_seed for a non-negative int: little-endian 32-bit words through
    // init_by_array (npcId fits in 32 bits in every call site).
    void SeedInt( std::uint64_t n )
    {
      std::vector<std::uint32_t> key;

      if ( n == 0 )
      {
        key.push_back( 0 );
      }
      else
      {
        while ( n )
        {
          key.push_back( static_cast<std::uint32_t>( n & 0xffffffffULL ) );
          n >>= 32;
        }
      }

      InitByArray( key );
    }

    std::uint32_t UInt32()
    {
      if ( m_Mti >= N )
      {
        constexpr std::uint32_t upper      = 0x80000000UL;
        constexpr std::uint32_t lower      = 0x7fffffffUL;
        constexpr std::uint32_t mag01[ 2 ] = { 0x0UL, 0x9908b0dfUL };

        int           kk = 0;
        std::uint32_t y  = 0;

        for ( kk = 0; kk < N - M; ++kk )
        {
          y          = m_Mt[ kk ] & upper | m_Mt[ kk + 1 ] & lower;
          m_Mt[ kk ] = m_Mt[ kk + M ] ^ y >> 1 ^ mag01[ y & 0x1UL ];
        }

        for ( ; kk < N - 1; ++kk )
        {
          y          = m_Mt[ kk ] & upper | m_Mt[ kk + 1 ] & lower;
          m_Mt[ kk ] = m_Mt[ kk + ( M - N ) ] ^ y >> 1 ^ mag01[ y & 0x1UL ];
        }

        y             = m_Mt[ kk ] & upper | m_Mt[ 0 ] & lower;
        m_Mt[ N - 1 ] = m_Mt[ M - 1 ] ^ y >> 1 ^ mag01[ y & 0x1UL ];
        m_Mti         = 0;
      }

      std::uint32_t y  = m_Mt[ m_Mti++ ];
      y               ^= y >> 11;
      y               ^= y << 7 & 0x9d2c5680UL;
      y               ^= y << 15 & 0xefc60000UL;
      y               ^= y >> 18;
      return y;
    }

    // random_getrandbits
    std::uint64_t Bits( int k )
    {
      if ( k <= 32 )
      {
        return UInt32() >> ( 32 - k );
      }

      std::uint64_t result = 0;
      int           shift  = 0;

      while ( k > 0 )
      {
        std::uint32_t r = UInt32();

        if ( k < 32 )
        {
          r >>= 32 - k;
        }

        result |= static_cast<std::uint64_t>( r ) << shift;
        shift  += 32;
        k      -= 32;
      }

      return result;
    }

    // Lib/random.py: _randbelow_with_getrandbits
    std::uint64_t Below( std::uint64_t n )
    {
      if ( n == 0 )
      {
        return 0;
      }

      const int k = BitLength( n );

      std::uint64_t r = Bits( k );
      while ( r >= n )
      {
        r = Bits( k );
      }

      return r;
    }

    std::int64_t Int( std::int64_t a, std::int64_t b )  // Inclusive
    {
      return a + static_cast<std::int64_t>(
                   Below( static_cast<std::uint64_t>( b - a + 1 ) ) );
    }

    // Choice index: seq[_randbelow(len(seq))]
    std::size_t ChoiceIndex( std::size_t length )
    {
      return Below( length );
    }

    // genrand_res53
    double Random()
    {
      const std::uint32_t a = UInt32() >> 5;
      const std::uint32_t b = UInt32() >> 6;
      return ( a * 67108864.0 + b ) * ( 1.0 / 9007199254740992.0 );
    }

   private:
    static int BitLength( std::uint64_t n )
    {
      int b = 0;
      while ( n )
      {
        ++b;
        n >>= 1;
      }

      return b;
    }

    void Init( std::uint32_t s )
    {
      m_Mt[ 0 ] = s;

      for ( int i = 1; i < N; ++i )
      {
        m_Mt[ i ] = 1812433253UL * ( m_Mt[ i - 1 ] ^ m_Mt[ i - 1 ] >> 30 ) + i;
      }

      m_Mti = N;
    }

    void InitByArray( const std::vector<uint32_t> & key )
    {
      Init( 19650218UL );
      int       i  = 1;
      int       j  = 0;
      const int kl = static_cast<int>( key.size() );

      int k = N > kl ? N : kl;
      for ( ; k; --k )
      {
        m_Mt[ i ] =
          ( m_Mt[ i ] ^ ( m_Mt[ i - 1 ] ^ m_Mt[ i - 1 ] >> 30 ) * 1664525UL ) +
          key[ j ] +
          j;

        ++i;
        ++j;

        if ( i >= N )
        {
          m_Mt[ 0 ] = m_Mt[ N - 1 ];
          i         = 1;
        }

        if ( j >= kl )
        {
          j = 0;
        }
      }

      for ( k = N - 1; k; --k )
      {
        m_Mt[ i ] = ( m_Mt[ i ] ^
                      ( m_Mt[ i - 1 ] ^ m_Mt[ i - 1 ] >> 30 ) * 1566083941UL ) -
                    i;

        ++i;

        if ( i >= N )
        {
          m_Mt[ 0 ] = m_Mt[ N - 1 ];
          i         = 1;
        }
      }

      m_Mt[ 0 ] = 0x80000000UL;
    }

    static constexpr int N = 624;
    static constexpr int M = 397;
    std::uint32_t        m_Mt[ N ];
    int                  m_Mti = N + 1;
  };
}  // namespace Toontown::Util
