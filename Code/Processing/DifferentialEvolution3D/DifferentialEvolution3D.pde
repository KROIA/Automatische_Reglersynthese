// Differential Evolution + 3D Ackley Surface
// Run in Processing with P3D renderer

int popSize = 30;
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
int updateCounter = 0;

// Search bounds
float minX = -5;
float maxX = 5;
float minY = -5;
float maxY = 5;

int gridRes = 60; // resolution of surface mesh


// Camera control variables
float rotX = -PI/4;//PI/6;   // initial tilt
float rotY = 0;//PI/4;  // initial rotation
float zoom = -100;    // camera distance

float lastMouseX, lastMouseY;
boolean dragging = false;


void setup() {
  size(1500, 1500, P3D);
  frameRate(60);
  float fov = PI/4;   // ~82° wide, shows more area

  float cameraZ = (height/2.0) / tan(fov/2.0);
  perspective(fov, float(width)/float(height), 
              cameraZ/10.0, cameraZ*10.0);
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
  background(50);
  lights();

  // Setup camera
  translate(width/2, height/2, 0);   // move origin to center
  scale(10, -10, 10);                   // flip y-axis for upright
  translate(0, 0, -zoom);            // zoom (move camera back)
  rotateX(rotX);
  rotateY(rotY);

  drawSurface();
  drawPopulation();

  if(updateCounter % 10 == 0)
  {
    if (generation < maxGen) {
      evolve();
      generation++;
    }
  }
  updateCounter++;

  // Overlay text
  hint(DISABLE_DEPTH_TEST);
  camera();  // reset camera for 2D overlay
  fill(255);
  textSize(14);
  text("Generation: " + generation, 10, height - 20);
  hint(ENABLE_DEPTH_TEST);
}



// ---------------- Differential Evolution ----------------
void evolve() {
  ArrayList<Individual> newPop = new ArrayList<Individual>();

  for (int i = 0; i < popSize; i++) {
    int a, b, c;
    do { a = int(random(popSize)); } while (a == i);
    do { b = int(random(popSize)); } while (b == i || b == a);
    do { c = int(random(popSize)); } while (c == i || c == a || c == b);

    Individual A = population.get(a);
    Individual B = population.get(b);
    Individual C = population.get(c);

    float[] donor = new float[dim];
    for (int d = 0; d < dim; d++) {
      donor[d] = A.pos[d] + F * (B.pos[d] - C.pos[d]);
    }

    float[] trial = new float[dim];
    for (int d = 0; d < dim; d++) {
      if (random(1) < CR) trial[d] = donor[d];
      else trial[d] = population.get(i).pos[d];
    }

    trial[0] = constrain(trial[0], minX, maxX);
    trial[1] = constrain(trial[1], minY, maxY);

    Individual trialInd = new Individual(trial);
    if (trialInd.fitness < population.get(i).fitness) newPop.add(trialInd);
    else newPop.add(population.get(i));
  }
  population = newPop;
}






// ---------------- Objective: Ackley ----------------
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
    height += 0.3*(-10+(x1+2.5)*(x1+2.5)+(x2+2.5)*(x2+2.5));
  
  
  return height;
}


// --------------------------------------------------
//               Utilities
// --------------------------------------------------

void keyPressed() {
  if (key == 'r') {
    //println("W pressed!");
    initPopulation();
  }

}
// Mouse controls
void mousePressed() {
  lastMouseX = mouseX;
  lastMouseY = mouseY;
  dragging = true;
}

void mouseReleased() {
  dragging = false;
}

void mouseDragged() {
  if (dragging) {
    float dx = mouseX - lastMouseX;
    float dy = mouseY - lastMouseY;
    rotY += dx * 0.01;
    rotX += dy * 0.01;
    lastMouseX = mouseX;
    lastMouseY = mouseY;
  }
}

void mouseWheel(MouseEvent event) {
  zoom += event.getCount(); // scroll to zoom
  zoom = constrain(zoom, -150, 80);
}


// ---------------- Surface ----------------
void drawSurface() {
  noStroke();
  lights(); // enable default lighting
  ambientLight(80, 80, 80);
  directionalLight(200, 200, 200, -0.5, -0.8, -1);

  for (int gx = 0; gx < gridRes; gx++) {
    float x0 = map(gx, 0, gridRes, minX, maxX);
    float x1 = map(gx+1, 0, gridRes, minX, maxX);

    beginShape(QUAD_STRIP);
    for (int gy = 0; gy <= gridRes; gy++) {
      float y = map(gy, 0, gridRes, minY, maxY);

      float z0 = objective(new float[]{x0, y});
      float z1 = objective(new float[]{x1, y});

      // Map z to a color gradient
      float c0 = map(z0, -1, 1, 0, 255);
      float c1 = map(z1, -1, 1, 0, 255);

      fill(50, c0, 255 - c0);  // gradient color for first vertex
      vertex(x0, y, z0);

      fill(50, c1, 255 - c1);  // gradient color for second vertex
      vertex(x1, y, z1);
    }
    endShape();
  }
}


// ---------------- Population ----------------
void drawPopulation() {
  stroke(1000);
  fill(255, 100, 100);
  for (Individual ind : population) {
    float x = ind.pos[0];
    float y = ind.pos[1];
    float z = objective(new float[]{x, y});
    pushMatrix();
    translate(x, y, z);
    sphere(0.08);  // small sphere for each candidate
    popMatrix();
  }
}
