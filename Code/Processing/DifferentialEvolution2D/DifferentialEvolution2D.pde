// Differential Evolution + Marching Squares contour visualization
// Paste into Processing (Java mode)

int popSize = 100;
float F = 0.8;
float CR = 0.9;
int dim = 2;
int maxGen = 1000;

class Individual {
  float[] pos;
  float fitness;

  Individual(float[] pos) {
    this.pos = pos;
    fitness = objective(pos);
  }
}
ArrayList<Individual> population;
int generation = 0;

// Search bounds
float minX = -5;
float maxX = 5;
float minY = -5;
float maxY = 5;

// marching squares grid resolution (pixels per grid cell)
int gridStep = 4;
float[][] grid;    // grid values at cell corners
int cols, rows;

void setup() {
  size(600, 600);
  initPopulation();
}

void initPopulation() {
  population = new ArrayList<Individual>();
  for (int i = 0; i < popSize; i++) {
    float[] pos = new float[dim];
    pos[0] = random(2, 3);
    pos[1] = random(2, 3);
    population.add(new Individual(pos));
  }
  generation = 0;
}

void draw() {
  background(0);
  drawLandscape();   // draws colored terrain + contour lines
  drawPopulation();  // overlay population points

  if (generation < maxGen) {
    evolve();
    generation++;
  }

  fill(255);
  textSize(14);
  text("Generation: " + generation, 10, height - 12);
  text("Grid step: " + gridStep + " px | Contours: marching squares", 10, height - 28);
  delay(10);
}

void drawPopulation() {
  noStroke();
  fill(0, 255, 0);
  for (Individual ind : population) {
    float sx = map(ind.pos[0], minX, maxX, 0, width);
    float sy = map(ind.pos[1], minY, maxY, height, 0);
    ellipse(sx, sy, 8, 8);
  }
}

void drawLine(float x1, float y1, float x2, float y2)
{
  float x1m = map(x1, minX, maxX, 0, width);
  float y1m = map(y1, minY, maxY, 0, height);
  float x2m = map(x2, minX, maxX, 0, width);
  float y2m = map(y2, minY, maxY, 0, height);
}

// ------------------ Differential Evolution (same as before) ------------------

void evolve() {
  ArrayList<Individual> newPop = new ArrayList<Individual>();

  for (int i = 0; i < popSize; i++) {
    // Mutation: select 3 random, distinct indices
    int a, b, c;
    do { a = int(random(popSize)); } while (a == i);
    do { b = int(random(popSize)); } while (b == i || b == a);
    do { c = int(random(popSize)); } while (c == i || c == a || c == b);

    Individual A = population.get(a);
    Individual B = population.get(b);
    Individual C = population.get(c);

    // Create donor vector
    float[] donor = new float[dim];
    for (int d = 0; d < dim; d++) {
      donor[d] = A.pos[d] + F * (B.pos[d] - C.pos[d]);
    }
    
    stroke(255,0,0);
    strokeWeight(5);
    line(A.pos[0], A.pos[1], donor[0]-A.pos[0], donor[1]-A.pos[1]);

    // Crossover
    float[] trial = new float[dim];
    for (int d = 0; d < dim; d++) {
      if (random(1) < CR) trial[d] = donor[d];
      else trial[d] = population.get(i).pos[d];
    }

    // Bound check
    trial[0] = constrain(trial[0], minX, maxX);
    trial[1] = constrain(trial[1], minY, maxY);

    // Selection
    Individual trialInd = new Individual(trial);
    if (trialInd.fitness < population.get(i).fitness) 
      newPop.add(trialInd);
    else 
      newPop.add(population.get(i));
  }
  population = newPop;
}



// Objective: Ackley (multi-modal)
float objective(float[] x) {
  /*float a = 20;
  float b = 0.2;
  float c = TWO_PI;
  float sum1 = 0;
  float sum2 = 0;
  for (int i = 0; i < x.length; i++) {
    sum1 += x[i] * x[i];
    sum2 += cos(c * x[i]);
  }
  float term1 = -a * exp(-b * sqrt(sum1 / x.length));
  float term2 = -exp(sum2 / x.length);
  return term1 + term2 + a + exp(1);*/
  
  float x1 = x[0];
  float x2 = x[1];
  
  float height = sin(x1*2)*sin(x2*2) + 0.001*(x1*x1+x2*x2);
  if(x1 < 0 && x2 < 0)
    height += 0.1*(-5+(x1+2.5)*(x1+2.5)+(x2+2.5)*(x2+2.5));
  
  
  return height;
}


// ----------------------------------------------------------------
//                         Utilities
// ----------------------------------------------------------------

// Optional: click to reset population or press +/- to change grid resolution
void mousePressed() {
  initPopulation();
}

void keyPressed() {
  if (key == '+' || key == '=') {
    gridStep = max(2, gridStep - 1); // increase grid resolution (smaller step)
  } else if (key == '-') {
    gridStep = min(20, gridStep + 1); // decrease grid resolution
  } else if (key == 'r' || key == 'R') {
    initPopulation();
  }
}


// ------------------ Landscape + Marching Squares ------------------

void drawLandscape() {
  // Prepare grid
  cols = ceil((float)width / gridStep);
  rows = ceil((float)height / gridStep);
  grid = new float[cols+1][rows+1];

  float minF = Float.MAX_VALUE;
  float maxF = -Float.MAX_VALUE;

  // compute scalar field at grid corners
  for (int i = 0; i <= cols; i++) {
    for (int j = 0; j <= rows; j++) {
      float sx = map(i * gridStep, 0, width, minX, maxX);
      float sy = map(j * gridStep, 0, height, maxY, minY); // flip Y
      float v = objective(new float[]{sx, sy});
      grid[i][j] = v;
      if (v < minF) minF = v;
      if (v > maxF) maxF = v;
    }
  }

  // draw colored background using grid blocks (fast)
  loadPixels();
  for (int i = 0; i <= cols; i++) {
    for (int j = 0; j <= rows; j++) {
      float f = grid[i][j];
      float norm = (minF == maxF) ? 0 : map(f, minF, maxF, 0, 255);
      norm = constrain(norm, 0, 255);
      int col = color(255 - norm, 90, norm); // teal-ish palette

      // fill the block of size gridStep
      int baseX = i * gridStep;
      int baseY = j * gridStep;
      for (int dx = 0; dx < gridStep; dx++) {
        for (int dy = 0; dy < gridStep; dy++) {
          int px = baseX + dx;
          int py = baseY + dy;
          if (px < width && py < height) {
            pixels[py * width + px] = col;
          }
        }
      }
    }
  }
  updatePixels();

  // Draw contour lines with marching squares
  int contourCount = 18;
  strokeWeight(1.2);
  for (int k = 1; k <= contourCount; k++) {
    float level = map(k, 0, contourCount+1, minF, maxF); // avoid exactly min/max
    // choose stroke color depending on level (darker for low)
    float t = map(k, 1, contourCount, 0, 1);
    stroke(0, 200 * (1 - t) + 50, 255 * t, 220); // subtle gradient
    marchingSquares(level);
  }
}

// marching squares implementation on the precomputed grid
void marchingSquares(float level) {
  for (int i = 0; i < cols; i++) {
    for (int j = 0; j < rows; j++) {
      // corner values
      float v0 = grid[i][j];         // top-left
      float v1 = grid[i+1][j];       // top-right
      float v2 = grid[i+1][j+1];     // bottom-right
      float v3 = grid[i][j+1];       // bottom-left

      int caseIndex = 0;
      if (v0 > level) caseIndex |= 1;
      if (v1 > level) caseIndex |= 2;
      if (v2 > level) caseIndex |= 4;
      if (v3 > level) caseIndex |= 8;

      if (caseIndex == 0 || caseIndex == 15) continue; // no segments

      // pixel coordinates of the corners
      float x0 = i * gridStep, y0 = j * gridStep;               // top-left
      float x1 = (i+1) * gridStep, y1 = j * gridStep;           // top-right
      float x2 = (i+1) * gridStep, y2 = (j+1) * gridStep;       // bottom-right
      float x3 = i * gridStep, y3 = (j+1) * gridStep;           // bottom-left

      // compute edge intersection points (only once)
      PVector p0 = interpEdge(x0, y0, x1, y1, v0, v1, level); // top edge
      PVector p1 = interpEdge(x1, y1, x2, y2, v1, v2, level); // right edge
      PVector p2 = interpEdge(x3, y3, x2, y2, v3, v2, level); // bottom edge (note v3->v2)
      PVector p3 = interpEdge(x0, y0, x3, y3, v0, v3, level); // left edge

      // Handle each of 16 cases (ambiguous cases draw both segments)
      switch (caseIndex) {
        case 1:  line(p3.x, p3.y, p0.x, p0.y); break;
        case 2:  line(p0.x, p0.y, p1.x, p1.y); break;
        case 3:  line(p3.x, p3.y, p1.x, p1.y); break;
        case 4:  line(p1.x, p1.y, p2.x, p2.y); break;
        case 5:  // ambiguous: opposite corners in
                 line(p3.x, p3.y, p0.x, p0.y);
                 line(p1.x, p1.y, p2.x, p2.y);
                 break;
        case 6:  line(p0.x, p0.y, p2.x, p2.y); break;
        case 7:  line(p3.x, p3.y, p2.x, p2.y); break;
        case 8:  line(p2.x, p2.y, p3.x, p3.y); break;
        case 9:  line(p0.x, p0.y, p2.x, p2.y); break;
        case 10: // ambiguous: opposite corners in
                 line(p0.x, p0.y, p1.x, p1.y);
                 line(p2.x, p2.y, p3.x, p3.y);
                 break;
        case 11: line(p1.x, p1.y, p3.x, p3.y); break;
        case 12: line(p1.x, p1.y, p3.x, p3.y); break;
        case 13: line(p0.x, p0.y, p1.x, p1.y); break;
        case 14: line(p0.x, p0.y, p3.x, p3.y); break;
        default: break;
      }
    }
  }
}

// linear interpolate on an edge (return screen-space PVector)
PVector interpEdge(float xA, float yA, float xB, float yB, float vA, float vB, float level) {
  float denom = (vB - vA);
  float t;
  if (abs(denom) < 1e-9) t = 0.5; // avoid divide-by-zero -> midpoint
  else t = (level - vA) / denom;
  t = constrain(t, 0, 1);
  return new PVector(lerp(xA, xB, t), lerp(yA, yB, t));
}
