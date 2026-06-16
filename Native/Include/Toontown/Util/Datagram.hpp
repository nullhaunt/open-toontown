// Minimal implementation of Panda3D's Datagram/DatagramIterator wire format --
// the byte layout that crosses the client/server boundary and is persisted
// (i.e., avatar DNA blobs), so it must stay byte-identical to PyDatagram.
// Shared by every converted module that builds a net string (e.g., SuitDNA,
// ToonDNA), replacing per-module byte packing.
//
// Surface grows with consumers, not speculatively.  Multibyte integer adds
// are little-endian (Panda3D default) when they're first needed.
#pragma once

#include <cstdint>
#include <stdexcept>
#include <string>

namespace Toontown::Util
{
  class Datagram
  {
   public:
    void UInt8( std::uint8_t v )
    {
      m_Data.push_back( static_cast<char>( v ) );
    }

    // Panda3D add_fixed_string: exactly `size` bytes -- truncate if longer,
    // null-pad if shorter.
    void FixedString( const std::string & s, std::size_t size )
    {
      if ( s.size() >= size )
      {
        m_Data.append( s, 0, size );
      }
      else
      {
        m_Data.append( s );
        m_Data.append( size - s.size(), '\0' );
      }
    }

    [[nodiscard]] const std::string & Bytes() const
    {
      return m_Data;
    }

   private:
    std::string m_Data;
  };

  class DatagramIterator
  {
   public:
    explicit DatagramIterator( const std::string & data )
      : m_Data( data )
    {
    }

    std::uint8_t GetUInt8()
    {
      if ( m_Offset + 1 > m_Data.size() )
      {
        throw std::runtime_error( "DatagramIterator::GetUInt8: past end" );
      }

      return static_cast<std::uint8_t>( m_Data[ ++m_Offset ] );
    }

    // Panda3D get_fixed_string: read `size` bytes, truncate at the first null.
    std::string GetFixedString( std::size_t size )
    {
      if ( m_Offset + size > m_Data.size() )
      {
        throw std::runtime_error(
          "DatagramIterator::GetFixedString: past end" );
      }

      std::string field  = m_Data.substr( m_Offset, size );
      m_Offset          += size;

      if ( const auto zero = field.find( '\0' ); zero != std::string::npos )
      {
        field.resize( zero );
      }

      return field;
    }

    [[nodiscard]] std::size_t GetRemainingSize() const
    {
      return m_Data.size() - m_Offset;
    }

   private:
    const std::string & m_Data;
    std::size_t         m_Offset = 0;
  };
}  // namespace Toontown::Util
