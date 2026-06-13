#pragma once

#include <vector>
#include <optional>

namespace Toontown::SafeZone
{
  /**
   * A single board square.  adjacent/jump hold four entries each; a missing
   * neighbor is std::nullopt (Python's None).
   */
  class CheckerTile
  {
   public:
    explicit CheckerTile( int tile );

    [[nodiscard]] std::vector<std::optional<int>> GetAdjacent() const;
    [[nodiscard]] std::vector<std::optional<int>> GetJumps() const;
    void SetAdjacent( const std::vector<std::optional<int>>& adjList );
    void SetJumps( const std::vector<std::optional<int>>& jumpList );

    [[nodiscard]] int GetState() const;
    void              SetState( int state );
    [[nodiscard]] int GetTile() const;

    /**
     * Python's delete() frees members for garbage collection; under RAII this
     * is a no-op, kept only so the original callable interface stays intact.
     */
    void Delete();

   private:
    int                             m_Tile;
    int                             m_State;
    std::vector<std::optional<int>> m_Adjacent;
    std::vector<std::optional<int>> m_Jumps;
  };

  class Checkerboard
  {
   public:
    Checkerboard();

    /**
     * Live reference into the board; callers mutate tiles through it.
     */
    [[nodiscard]] CheckerTile& GetSquare( int arrayLoc );

    [[nodiscard]] int GetState( int square ) const;
    void              SetState( int square, int state );
    [[nodiscard]] std::vector<std::optional<int>> GetAdjacent(
      int square ) const;
    [[nodiscard]] std::vector<std::optional<int>> GetJumps( int square ) const;
    [[nodiscard]] std::vector<int>                GetStates() const;
    void SetStates( const std::vector<int>& squares );

    /**
     * See CheckersTile::Delete.
     */
    void Delete();

    /**
     * Backs Python's .squareList attribute.  Returned by reference so element
     * access yields live tiles (bound via nanobind::bind_vector).
     */
    [[nodiscard]] std::vector<CheckerTile>& SquareList();

   private:
    std::vector<CheckerTile> m_SquareList;
  };
}  // namespace Toontown::SafeZone
