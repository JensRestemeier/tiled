
#include "erasecolours.h"

#include "colourlayer.h"
#include "colourpainter.h"

#include <QCoreApplication>

using namespace Tiled;
using namespace Tiled::Internal;

EraseColours::EraseColours(MapDocument *mapDocument,
                       ColourLayer *tileLayer,
                       const QRegion &region)
    : mMapDocument(mapDocument)
    , mColourLayer(tileLayer)
    , mRegion(region)
    , mMergeable(false)
{
    setText(QCoreApplication::translate("Undo Commands", "Erase"));

    // Store the tiles that are to be erased
    const QRegion r = mRegion.translated(-mColourLayer->x(), -mColourLayer->y());
    mErasedCells = mColourLayer->copy(r);
}

EraseColours::~EraseColours()
{
    delete mErasedCells;
}

void EraseColours::undo()
{
    const QRect bounds = mRegion.boundingRect();
    ColourPainter painter(mMapDocument, mColourLayer);
    painter.drawCells(bounds.x(), bounds.y(), mErasedCells);
}

void EraseColours::redo()
{
    ColourPainter painter(mMapDocument, mColourLayer);
    painter.erase(mRegion);
}

bool EraseColours::mergeWith(const QUndoCommand *other)
{
    const EraseColours *o = static_cast<const EraseColours*>(other);
    if (!(mMapDocument == o->mMapDocument &&
          mColourLayer == o->mColourLayer &&
          o->mMergeable))
        return false;

    const QRegion combinedRegion = mRegion.united(o->mRegion);
    if (mRegion != combinedRegion) {
        const QRect bounds = mRegion.boundingRect();
        const QRect combinedBounds = combinedRegion.boundingRect();

        // Resize the erased tiles layer when necessary
        if (bounds != combinedBounds) {
            const QPoint shift = bounds.topLeft() - combinedBounds.topLeft();
            mErasedCells->resize(combinedBounds.size(), shift);
        }

        // Copy the newly erased tiles over
        const QRect otherBounds = o->mRegion.boundingRect();
        const QPoint pos = otherBounds.topLeft() - combinedBounds.topLeft();
        mErasedCells->merge(pos, o->mErasedCells);

        mRegion = combinedRegion;
    }

    return true;
}
