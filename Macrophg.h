#ifndef MACROPHG_H
#define MACROPHG_H

void MacroInit();
void MacroTerminate();
void MacroUpdate();
void MacroRender();
int GetTotalCellCount();

extern bool CellSurfaceChanged;

#endif