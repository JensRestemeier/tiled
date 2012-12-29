#ifndef ERASECOLOURS_H
#define ERASECOLOURS_H

#include "undocommands.h"

#include <QRegion>
#include <QUndoCommand>

namespace Tiled {

class Tile;
class ColourLayer;

namespace Internal {

class MapDocument;

class EraseColours: public QUndoCommand
{
public:
    EraseColours(MapDocument *mapDocument,
               ColourLayer *tileLayer,
               const QRegion &region);
    ~EraseColours();

    /**
     * Sets whether this undo command can be merged with an existing command.
     */
    void setMergeable(bool mergeable)
    { mMergeable = mergeable; }

    void undo();
    void redo();

    int id() const { return Cmd_EraseColours; }
    bool mergeWith(const QUndoCommand *other);

private:
    MapDocument *mMapDocument;
    ColourLayer *mColourLayer;
    ColourLayer *mErasedCells;
    QRegion mRegion;
    bool mMergeable;
};

} // namespace Internal
} // namespace Tiled

#endif // ERASECOLOURS_H
