/* 
 * File: maze.cpp
 * --------------
 * A maze is mostly a grid of walls. plus a little drawing code.
 */

#include <iostream>
#include "maze.h"
#include "extgraph.h"
#include "set.h"
#include "queue.h"
#include "stack.h"
#include "random.h"

void generatePerfectMaze(Maze &m);
Stack<pointT> solvePerfectMaze(Maze &m, pointT start, pointT end);
pointT selectRandomPoint(Maze &m);
int myComp(pointT a, pointT b);
pointT pickRandomNeighbor(Maze &m, pointT currentT);
Set<pointT> generateNeighbors(Maze &m, pointT currentT);

int main() {
  Randomize();			// remember to put it here
  Maze m(50, 90, true);
  m.draw();
  generatePerfectMaze(m);

  pointT start;
  start.row = 0;
  start.col = 0;
  pointT end;
  end.row = 49;
  end.col = 89;
  Stack<pointT> final = solvePerfectMaze(m, start, end);

  while (!final.isEmpty()){
    pointT p = final.pop();
    m.drawMark(p, "Blue");
  }
  return 0;
}

void generatePerfectMaze(Maze &m){
  pointT currentT;
  Set<pointT> included(myComp);
  currentT = selectRandomPoint(m);
  included.add(currentT);

  pointT randomNeighborT;

  while (true) {
    randomNeighborT = pickRandomNeighbor(m, currentT);    
    if (!included.contains(randomNeighborT)){
      m.setWall(currentT, randomNeighborT, false);
      included.add(randomNeighborT);
    }
    currentT.row = randomNeighborT.row;
    currentT.col = randomNeighborT.col;
    if (included.size() == 4500) break;
  }

}

Stack<pointT> solvePerfectMaze(Maze &m, pointT start, pointT end) {
  Queue<Stack<pointT> > searchPath;
  Set<pointT> seenPoints(myComp);
  Stack<pointT> current;
  
  Stack<pointT> initial;
  initial.push(start);
  searchPath.enqueue(initial);
  seenPoints.add(start);

  while (!searchPath.isEmpty()){
    current = searchPath.dequeue();
    Set<pointT> neighbors(myComp);
    neighbors = generateNeighbors(m, current.peek());
    
    Set<pointT>::Iterator iter = neighbors.iterator();
    while (iter.hasNext()){
      //    foreach (pointT p in neightbors){
      pointT p = iter.next();
      if (!seenPoints.contains(p)) {
	seenPoints.add(p);
	if (p.row == end.row && p.col == end.col) {
	  current.push(p);
	  return current;
	} else {
	  Stack<pointT> copy = current;
	  copy.push(p);
	  searchPath.enqueue(copy);
	}
      }
    }
  }

  Stack<pointT> empty;		// case where no path is found
  return empty;			// should not happen for perfect maze
}


pointT selectRandomPoint(Maze &m){
  Randomize();			// this is different from the newer version
  int numRow = m.numRows();
  int numCol = m.numCols();
  pointT resultT;
  resultT.row = RandomInteger(0, numRow-1);
  resultT.col = RandomInteger(0, numCol-1);

  return resultT;
}

int myComp(pointT a, pointT b){
  //if (a.row == b.row && a.col == b.col) return true;
  if (a.row < b.row){
    return -1;
  } else if (a.row > b.row){
    return 1;
  } else if (a.col < b.col) {
    return -1;
  } else if (a.col > b.col){
    return 1;
  } else {
    return 0;
  }
}

pointT pickRandomNeighbor(Maze &m, pointT currentT){
  pointT resultT;
  int index = RandomInteger(1, 4);
  while (true){
    if (index == 1){
      resultT.row = currentT.row - 1;
      resultT.col = currentT.col;
      if (m.pointInBounds(resultT)){
	return resultT;
      } 
    } else if (index == 2){
      resultT.row = currentT.row + 1;
      resultT.col = currentT.col;
      if (m.pointInBounds(resultT)){
	return resultT;
      }
    } else if (index == 3) {
      resultT.row = currentT.row;
      resultT.col = currentT.col - 1;
      if (m.pointInBounds(resultT)){
	return resultT;
      }
    } else {
      resultT.row = currentT.row;
      resultT.col = currentT.col + 1;
      if (m.pointInBounds(resultT)) {
	return resultT;
      }
    }
    index = RandomInteger(1, 4);
  }
}

Set<pointT> generateNeighbors(Maze &m, pointT currentT) {
  Set<pointT> result(myComp);
  pointT resultT;

  resultT.row = currentT.row - 1;
  resultT.col = currentT.col;
  if (m.pointInBounds(resultT) && !m.isWall(currentT, resultT)){
    result.add(resultT);
  } 

  resultT.row = currentT.row + 1;
  resultT.col = currentT.col;
  if (m.pointInBounds(resultT) && !m.isWall(currentT, resultT)){
    result.add(resultT);
  }

  resultT.row = currentT.row;
  resultT.col = currentT.col - 1;
  if (m.pointInBounds(resultT) && !m.isWall(currentT, resultT)){
    result.add(resultT);
  }

  resultT.row = currentT.row;
  resultT.col = currentT.col + 1;
  if (m.pointInBounds(resultT) && !m.isWall(currentT, resultT)) {
    result.add(resultT);
  }

  return result;
}



Maze::Maze(int numRows, int numCols, bool hasWalls) : cells(numRows, numCols) 
{
  for (int r = 0; r < numRows; r++)
    for (int c = 0; c < numCols; c++)
      for (int d = 0; d < NumDirs; d++)
	cells(r, c).walls[d] = hasWalls;
  configured = false;
}

int Maze::numRows() 
{ 
  return cells.numRows(); 
}

int Maze::numCols() 
{ 
  return cells.numCols(); 
}

bool Maze::pointInBounds(pointT p)
{
  return (p.row >= 0 && p.row < numRows() && p.col >=0 && p.col < numCols());
}


void Maze::draw()
{
  InitGraphics();	// this erases entire graphics window
  if (!configured) configureGraphics();
  for (int r = 0; r < cells.numRows(); r++) {
    for (int c = 0; c < cells.numCols(); c++) {
      pointT p = {r, c};
      drawWallsForCell(p);
    }
  }
  UpdateDisplay();
}

bool Maze::isWall(pointT p1, pointT p2)
{
  if (!pointInBounds(p1) || !pointInBounds(p2))
    Error("Point is not in bounds for maze");
  return cells(p1.row, p1.col).walls[neighborDir(p1, p2)];
}

void Maze::setWall(pointT p1, pointT p2, bool state)
{
  if (!pointInBounds(p1) || !pointInBounds(p2))
    Error("Point is not in bounds for maze");
  cells(p1.row, p1.col).walls[neighborDir(p1, p2)] = state;
  cells(p2.row, p2.col).walls[neighborDir(p2, p1)] = state;
  if (!configured) configureGraphics();
  drawWallsForCell(p1);
  UpdateDisplay();
}
void Maze::drawMark(pointT p, string color)
{
  if (!pointInBounds(p))
    Error("Point is not in bounds for maze");
  if (!configured) configureGraphics();
  double margin = cellSize*.3;
  double length = cellSize - 2*margin;
  SetPenColor(color);
  MovePen(originX + p.col*cellSize + margin, originY + p.row*cellSize + margin);
  DrawLine(length, length);
  MovePen(originX + p.col*cellSize + margin, originY + p.row*cellSize + cellSize - margin);
  DrawLine(length, -length);
  UpdateDisplay();
}


Maze::dirT Maze::neighborDir(pointT p1, pointT p2)
{
  if ((abs(p1.row-p2.row) + abs(p1.col-p2.col)) != 1)
    Error("Points are not neighbors");
  if (p1.row != p2.row)
    return (p1.row < p2.row ? North : South);
  else
    return (p1.col < p2.col ? East : West);
}

void Maze::drawWallsForCell(pointT p)
{
  MovePen(originX + p.col*cellSize, originY + p.row*cellSize);
  SetPenColor(cells(p.row, p.col).walls[South] ? "Black" : "White");
  DrawLine(cellSize, 0);
  SetPenColor(cells(p.row, p.col).walls[East] ? "Black" : "White");
  DrawLine(0, cellSize);
  SetPenColor(cells(p.row, p.col).walls[North] ? "Black" : "White");
  DrawLine(-cellSize, 0);
  SetPenColor(cells(p.row, p.col).walls[West] ? "Black" : "White");
  DrawLine(0, -cellSize);
}

void Maze::configureGraphics()
{
  cellSize = min(GetWindowWidth()/numCols(), GetWindowHeight()/numRows());
  originX = (GetWindowWidth() - numCols()*cellSize)/2;
  originY = (GetWindowHeight() - numRows()*cellSize)/2;
  configured = true;	
}


