#include "colourselectiontool.h"

#include "colourbrushitem.h"
#include "changetileselection.h"
#include "map.h"
#include "mapdocument.h"
#include "mapscene.h"
#include "colourlayer.h"

using namespace Tiled;
using namespace Tiled::Internal;

ColourSelectionTool::ColourSelectionTool(QObject *parent)
    : AbstractColourTool(tr("Rectangular Select"),
                       QIcon(QLatin1String(
                               ":images/22x22/stock-tool-rect-select.png")),
                       QKeySequence(tr("R")),
                       parent)
    , mSelectionMode(Replace)
    , mSelecting(false)
{
    setTilePositionMethod(BetweenTiles);
}

void ColourSelectionTool::tilePositionChanged(const QPoint &)
{
    if (mSelecting)
        brushItem()->setTileRegion(selectedArea());
}

void ColourSelectionTool::updateStatusInfo()
{
    if (!isBrushVisible() || !mSelecting) {
        AbstractColourTool::updateStatusInfo();
        return;
    }

    const QPoint pos = tilePosition();
    const QRect area = selectedArea();
    setStatusInfo(tr("%1, %2 - Rectangle: (%3 x %4)")
                  .arg(pos.x()).arg(pos.y())
                  .arg(area.width()).arg(area.height()));
}

void ColourSelectionTool::mousePressed(QGraphicsSceneMouseEvent *event)
{
    const Qt::MouseButton button = event->button();
    const Qt::KeyboardModifiers modifiers = event->modifiers();

    if (button == Qt::LeftButton) {
        if (modifiers == Qt::ControlModifier) {
            mSelectionMode = Subtract;
        } else if (modifiers == Qt::ShiftModifier) {
            mSelectionMode = Add;
        } else if (modifiers == (Qt::ControlModifier | Qt::ShiftModifier)) {
            mSelectionMode = Intersect;
        } else {
            mSelectionMode = Replace;
        }

        mSelecting = true;
        mSelectionStart = tilePosition();
        brushItem()->setTileRegion(QRegion());
    }
}

void ColourSelectionTool::mouseReleased(QGraphicsSceneMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        mSelecting = false;

        MapDocument *document = mapDocument();
        QRegion selection = document->tileSelection();
        const QRect area = selectedArea();

        switch (mSelectionMode) {
        case Replace:   selection = area; break;
        case Add:       selection += area; break;
        case Subtract:  selection -= area; break;
        case Intersect: selection &= area; break;
        }

        if (selection != document->tileSelection()) {
            QUndoCommand *cmd = new ChangeTileSelection(document, selection);
            document->undoStack()->push(cmd);
        }

        brushItem()->setTileRegion(QRegion());
        updateStatusInfo();
    }
}

void ColourSelectionTool::languageChanged()
{
    setName(tr("Rectangular Select"));
    setShortcut(QKeySequence(tr("R")));
}

QRect ColourSelectionTool::selectedArea() const
{
    const QPoint tilePos = tilePosition();
    const QPoint pos(qMin(tilePos.x(), mSelectionStart.x()),
                     qMin(tilePos.y(), mSelectionStart.y()));
    const QSize size(qAbs(tilePos.x() - mSelectionStart.x()),
                     qAbs(tilePos.y() - mSelectionStart.y()));

    return QRect(pos, size);
}
