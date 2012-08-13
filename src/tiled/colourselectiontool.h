#ifndef COLOURSELECTIONTOOL_H
#define COLOURSELECTIONTOOL_H

#include "abstractcolourtool.h"

namespace Tiled {
namespace Internal {

class ColourSelectionTool : public AbstractColourTool
{
    Q_OBJECT

public:
    ColourSelectionTool(QObject *parent = 0);

    void mousePressed(QGraphicsSceneMouseEvent *event);
    void mouseReleased(QGraphicsSceneMouseEvent *event);

    void languageChanged();

protected:
    void tilePositionChanged(const QPoint &tilePos);

    void updateStatusInfo();

private:
    enum SelectionMode {
        Replace,
        Add,
        Subtract,
        Intersect
    };

    QRect selectedArea() const;

    QPoint mSelectionStart;
    SelectionMode mSelectionMode;
    bool mSelecting;
};

} // namespace Internal
} // namespace Tiled

#endif // COLOURSELECTIONTOOL_H
