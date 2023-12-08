#include <Arduino.h>
#include <Servo.h>

int between_steps_delay = 15000; //15 seconds
byte max_try = 2; //max_try  times
//-----------ik--------------
const uint8_t L1 = 126; // arm length 1
const uint8_t L2 = 100; // arm length 2
//==============ik================

//------servo------
Servo myservo1;  // create servo object to control a servo
Servo myservo2;
Servo myservo3;
int servo_pin_1 = 9;
int servo_pin_2 = 10;
int servo_pin_3 = 11;
float theta1;
float theta2;
float up = 100;
// float down = 88;
// float down = 93;
float down = 85;




int error_arr[5][5][2] = {{{3,-5},{0,-5},{-2,0},{-4,5},{-2,14}},//[row][col][{x,y}]
                           {{1,-9},{0,-5},{-1,0},{0,3},{-1,10}},
                           {{-3,-6},{1,-4},{-1,-1},{0,3},{1,10}},
                           {{-5,-5},{0,-3},{-1,-2},{0,5},{3,10}},
                           {{-5,0},{-5,-3},{0,1},{0,3},{3,10}}};
//======servo========


//---------------------Algo----------------------
static char color_Set[] = {'W', 'R', 'G', 'B', 'Y', 'O', 
                    'W', 'R', 'G', 'B', 'Y', 'O',
                    'W', 'R', 'G', 'B', 'Y', 'O',
                    'W', 'R', 'G', 'B', 'Y', 'O',};

static char puzzle_pattern[5][5];
static char target_pattern[3][3];


static byte main_path_new[25][2];
static byte blank_path_new[25][2];

int block_position_img[5][5] = {
  {0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0}
};

struct Node {
  int row, col;
  int g, h, f;
  Node* parent;
  bool closed;
};

//=========================Algo=========================


//-----------------color sens----------------------
int sensorval;
int ref_RGB[3];
int RGB[3];
int devs_new[7][3];

//=================================================

void shuffle_color_Set();
void random_patterns();
void print_patterns(bool target);
void ik_new(float x, float y);
void servo_go();
bool check_puzzle();

byte nearest_color(char target_clr, byte target_index);
void block_loc(byte row, byte col, bool block);
Node* AStarAlgorithm(int startRow, int startCol, int goalRow, int goalCol);
int calculateHeuristic(int currentRow, int currentCol, int goalRow, int goalCol);
Node nodes[5][5];
byte path_array_update_count(Node* goal, bool main_path_update, bool blank_path_update);
void print_path_array_new(byte new_count ,bool main_path_update, bool blank_path_update);
void blank_path_new_method(byte count_new);
byte* find_blank_space_new();
void servo_move(byte p1_row,byte p1_col,byte p2_row,byte p2_col);
void main_algo(byte i);
void go_home(int th1, int th2);
void read_RGB();
void Read_puzzle_new();
char color_char_by_New_deviatinos();
char color_detect_new();
void calibrate_new();
void read_target_pattern();
bool check_target();

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void setup(){
  Serial.begin(9600);
  
  myservo1.attach(9);  // attaches the servo on pin 9,10,11 to the servo object
  myservo2.attach(10);  
  myservo3.attach(11);
  
  pinMode(A0,INPUT);
  pinMode(2,OUTPUT);
  pinMode(3,OUTPUT);
  pinMode(4,OUTPUT);

  // Generate random_target_pattern:
  randomSeed(analogRead(0));
  // random_patterns(); // this generates random target pattern
  delay(100);
  
  /*
  */
  // go to home position
  go_home(40,15);
  bool check = false;
  
  


  //============================================================================================

  //first calibrate_new
  Serial.println("Arrange first 7 tilse ->  B R G Y W O 0");
  Serial.println("waiting for 15 seconds");
  delay(between_steps_delay);
  calibrate_new();
  delay(1000);
  go_home(40,15);

  //Read Target Pattern  
  Serial.println("Arrange target pattern at center: ");
  Serial.println("waiting for 15 seconds");
  delay(between_steps_delay);

  byte current_try_count = 0;
  do{
    read_target_pattern(); // updates target_pattern
    check = check_target();
    if (check == false){
      current_try_count += 1;
      Serial.println("detected blank space...");
      Serial.println("Change if needed.. Waiting for 5 seconds");
      go_home(40,15);
      delay(5000);
      if (current_try_count == max_try){
        exit(1);
      }
    }
    Serial.println();
  } while (check == false);
  
  // Read Puzzle pattern 
  go_home(40,15);
  Serial.println("Arrange puzzle pattern at center: ");
  Serial.println("waiting for 20 seconds");
  delay(between_steps_delay+5000);

  current_try_count = 0;
  do //read puzzle
  {
    // calibrate_new();
    // delay(1000);
    go_home(40,15);
    Read_puzzle_new();
    check = check_puzzle();
    if (check == false){
      current_try_count += 1;
      Serial.println("detected more than 4 color tiles of some color or detected 2 blank spaces");
      Serial.println("Going around...");
      delay(1000);
      if (current_try_count == max_try){
        go_home(40,15);
        exit(1);
      }
    }
    Serial.println();
    go_home(40,15);

  } while (check == false);
  
  
  // go to home position
  go_home(40,15);

  print_patterns(true);                           
  Serial.println();
  
  for(byte i=0;i<9;i++){
    main_algo(i);
    delay(500);
  }
}


void loop(){
}



//------------------------------------------------------------------


bool check_target(){
  byte count[7]={0,0,0,0,0,0,0}; ////B,R,G,Y,W,O,"0"
  for(byte row = 0; row <3; row++){
    for(byte col =0; col <3; col++){
      char current = puzzle_pattern[row][col];
      if (current == 'B')
      {
        count[0] +=1;
        if (count[0]>4){
          return false;
        }
      }
      else if (current == 'R')
      {
        count[1] +=1;
        if (count[1]>4){
          return false;
        }
      }
      else if (current == 'G')
      {
        count[2] +=1;
        if (count[2]>4){
          return false;
        }
      }
      else if (current == 'Y')
      {
        count[3] +=1;
        if (count[3]>4){
          return false;
        }
      }
      else if (current == 'W')
      {
        count[4] +=1;
        if (count[4]>4){
          return false;
        }
      }
      else if (current == 'O')
      {
        count[5] +=1;
        if (count[5]>4){
          return false;
        }
      }
      else if (current == '0')
      {
        return false;
      }
    }
  }
  return true;
}


void read_target_pattern(){
    Serial.println("\nTarget_pattern...");
    for(byte i=0;i<9;i++){
      byte row = (i/3)+1;
      byte col = (i%3)+1;
      
      float x = 62 + (28.2*col);// x-offset = 62
      float y = 56.4 - (28.2*row);// y-offset = 56.4
    
      x += error_arr[row][col][0];
      y += error_arr[row][col][1];

      ik_new(x,y);
      servo_go();
      char current_detected_color = color_detect_new();

      //assign this char to puzzle pattern[i][j]
      target_pattern[row][col] = current_detected_color;

      Serial.print(current_detected_color);
      Serial.print(" ");

      if (col == 3){
        Serial.println();
      }
    }

}


void calibrate_new(){
  // update devs_new according to each color:
  // go to first 6 locations: color pattern shoud be 'B', 'R', 'G', 'Y', 'W', 'O', '0' 
  // char color_arr[7] = {'B', 'R', 'G', 'Y', 'W', 'O', '0'};

  for(byte i = 0; i<7; i++){
    byte row = i/5;
    byte col = i%5;

    float x = 62 + (28.2*col);// x-offset = 62
    float y = 56.4 - (28.2*row);// y-offset = 56.4

    //Considering error array:
    x += error_arr[row][col][0];
    y += error_arr[row][col][1];

    ik_new(x,y);
    servo_go();
    delay(100);
    read_RGB();
    // update devs_new;
    for (byte j = 0; j < 3; j++)
    {
      devs_new[i][j] = RGB[j];
    }
  }


}


char color_detect_new(){
  //read curret color and compare it with all parent color
  read_RGB();

  char current_color = color_char_by_New_deviatinos();
  return current_color;
  //
}


char color_char_by_New_deviatinos(){
  char color_arr[7] = {'B', 'R', 'G', 'Y', 'W', 'O', '0'};
  byte return_index = 0;
  int current_nearest_dev=5000;
  for(byte i = 0; i<7; i++){
    int dev=0;
    for(byte j =0; j<3; j++){
      dev += abs(RGB[j] - devs_new[i][j]);
    }
    if (dev<current_nearest_dev)  {
      current_nearest_dev = dev;
      return_index = i;
    }
  }
  char char_to_return = color_arr[return_index];
  return char_to_return;
}


void Read_puzzle_new(){
  for (byte i = 0; i < 25; i++)
  {
    byte row = i/5;
    byte col = i%5;

    float x = 62 + (28.2*col);// x-offset = 62
    float y = 56.4 - (28.2*row);// y-offset = 56.4
  
    x += error_arr[row][col][0];
    y += error_arr[row][col][1];

    ik_new(x,y);
    servo_go();
    char current_detected_color = color_detect_new();
    //assign this char to puzzle pattern[i][j]
    puzzle_pattern[row][col] = current_detected_color;
    
    Serial.print(current_detected_color);
    Serial.print(" ");
    
    if (col == 4){
      Serial.println();
    }
  }
  
}


void read_RGB(){
  int R,G,B;
  R = 0;
  G = 0;
  B = 0;
  byte flash_count = 3;

  for (byte i = 0; i <flash_count ; i++){
    digitalWrite(2,HIGH);
    R += analogRead(A0);
    delay(30);
    digitalWrite(2,LOW);
    delay(30);

    digitalWrite(3,HIGH);
    G += analogRead(A0);
    delay(30);
    digitalWrite(3,LOW);
    delay(30);

    digitalWrite(4,HIGH);
    sensorval = analogRead(A0);
    B+= analogRead(A0);
    delay(30);
    digitalWrite(4,LOW);
    delay(30);
  }

  RGB[0] = R/flash_count;
  RGB[1] = G/flash_count;
  RGB[2] = B/flash_count;
}


void go_home(int th1, int th2){
  myservo3.write(up);
  delay(300);
  myservo1.write(th1);
  myservo2.write(th2);
  delay(400);
}


void main_algo(byte i){ 
  Serial.print("\n-------------------------------------------------------------------------\n-------------------------------------------------------------------------  :");
  Serial.println(i);
  // print_block_position_img();
  char target_color = target_pattern[i/3][i%3]; //row,col
  byte ind = nearest_color(target_color,i);

  // only move if nearest color is not at target color location:
  if (((ind/5) == (i/3)+1) && ((ind%5) == (i%3)+1)){
    Serial.println("Nearest_color(row,col): "+String(ind/5)+","+String(ind%5)+"\n");
    Serial.println(" Nearest color is at target location.");
    
    // parmanant block
    block_loc((i/3)+1, (i%3)+1,true);
    delay(100);
    return;
  }

  // call A_* algorithm
  Serial.println("Nearest_color(row,col): "+String(ind/5)+","+String(ind%5)+"\n");
  
  Node* found_path = AStarAlgorithm((i/3)+1,(i%3)+1,ind/5,ind%5);
  // Serial.print("main_path (row,col) : ");
 
  //update main path new and get count;
  byte count_new = path_array_update_count(found_path,true,false);

  print_path_array_new(count_new,true,false);

  blank_path_new_method(count_new);
  
  if(i == 8){
    print_patterns(true);
  }
  else{
    print_patterns(false);
  }

  block_loc((i/3)+1, (i%3)+1,true);
  delay(100);
}


byte nearest_color(char target_clr, byte target_index){
  /*
  ++++++++++++++++++++++++
  Need to consider block_position_img as well, update later.
  ++++++++++++++++++++++++

  */
  // target_location= [x,y]
  byte nearest_color_location_ind;
  byte row = (target_index/3)+1;
  byte col = (target_index%3)+1;
  byte target_location[2] = {row, col};
  Serial.println("target_color: "+String(target_clr)+"\nTarget_location(row,col): "+String(target_location[0])+","+(target_location[1]));
  
  byte min_dist = 25; // initialize with maximum value
  for (byte ind=0; ind<25; ind++){
    byte row = ind/5;
    byte col = ind%5;

    //check if the selected index is locked?
    if (block_position_img[row][col]==0){
      if (target_clr == puzzle_pattern[row][col]){
        // manhattan_distance
        byte dist = abs(row-target_location[0]) + abs(col-target_location[1]);
        if (dist < min_dist){
          min_dist = dist;
          nearest_color_location_ind = ind;
        }
      } 
    }
  }
  return nearest_color_location_ind;
}


void block_loc(byte row, byte col, bool block){
  if (block==true){
    block_position_img[row][col]= 1;
  }
  else{
    block_position_img[row][col]= 0;
  }
}


Node* AStarAlgorithm(int startRow, int startCol, int goalRow, int goalCol) {

  // Initialize nodes
  for (int i = 0; i < 5; ++i) {
    for (int j = 0; j < 5; ++j) {
      nodes[i][j].row = i;
      nodes[i][j].col = j;
      nodes[i][j].g = nodes[i][j].h = nodes[i][j].f = 0;
      nodes[i][j].parent = nullptr;
      nodes[i][j].closed = false;
    }
  }

  Node* startNode = &nodes[startRow][startCol];
  Node* goalNode = &nodes[goalRow][goalCol];

  startNode->g = 0;
  startNode->h = calculateHeuristic(startRow, startCol, goalRow, goalCol);
  startNode->f = startNode->g + startNode->h;

  Node* openSet[5 * 5];
  int openSetSize = 1;
  openSet[0] = startNode;

  while (openSetSize > 0) {
    // Find the node with the lowest f value in the open set
    int currentIndex = 0;
    for (int i = 1; i < openSetSize; ++i) {
      if (openSet[i]->f < openSet[currentIndex]->f) {
        currentIndex = i;
      }
    }

    Node* current = openSet[currentIndex];

    // Check if the goal is reached
    if (current == goalNode) {
      return goalNode;
    }

    // Remove the current node from the open set
    openSet[currentIndex] = openSet[openSetSize - 1];
    openSetSize--;

    // Mark the current node as closed
    current->closed = true;

    // Generate neighbors
    const int directions[4][2] = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}};
    for (const auto& direction : directions) {
      int neighborRow = current->row + direction[0];
      int neighborCol = current->col + direction[1];

      // Check if the neighbor is within the bounds of the block_position_img
      if (neighborRow >= 0 && neighborRow < 5 && neighborCol >= 0 && neighborCol < 5) {
        Node* neighbor = &nodes[neighborRow][neighborCol];

        // Check if the neighbor is an obstacle or closed
        if (!neighbor->closed && block_position_img[neighborRow][neighborCol] == 0) {
          int tentativeG = current->g + 1;

          // Check if the tentative g value is better than the current g value
          if (tentativeG < neighbor->g || !neighbor->closed) {
            neighbor->g = tentativeG;
            neighbor->h = calculateHeuristic(neighborRow, neighborCol, goalRow, goalCol);
            neighbor->f = neighbor->g + neighbor->h;
            neighbor->parent = current;

            // Add the neighbor to the open set if it's not already there
            bool inOpenSet = false;
            for (int i = 0; i < openSetSize; ++i) {
              if (openSet[i] == neighbor) {
                inOpenSet = true;
                break;
              }
            }

            if (!inOpenSet) {
              openSet[openSetSize] = neighbor;
              openSetSize++;
            }
          }
        }
      }
    }
  }

  return nullptr;  // Return nullptr if no path is found
}


int calculateHeuristic(int currentRow, int currentCol, int goalRow, int goalCol) {
  // Calculate the Manhattan distance heuristic
  return abs(goalRow - currentRow) + abs(goalCol - currentCol);
}


byte path_array_update_count(Node* goal, bool main_path_update, bool blank_path_update){
  byte count = 0;
  while(goal!=nullptr){
    if(main_path_update==true){
      main_path_new[count][0] = goal->row;
      main_path_new[count][1] = goal->col;
    }
    else if(blank_path_update==true){
      blank_path_new[count][0] = goal->row;
      blank_path_new[count][1] = goal->col;
    }
    count++;
    goal = goal->parent;
  }
  return count;
}


void print_path_array_new(byte new_count ,bool main_path_update, bool blank_path_update){
  if (main_path_update == true){
    Serial.print("Main_path_new : ");
    for(byte i=0; i<new_count;i++){
        Serial.print("("+String(main_path_new[i][0])+","+String(main_path_new[i][1])+") ");
      }
      Serial.println();
  }

  if (blank_path_update == true){
    Serial.print("blank_path_new : ");
    for(byte i=0; i<new_count;i++){
        Serial.print("("+String(blank_path_new[i][0])+","+String(blank_path_new[i][1])+") ");
      }
      Serial.println();
  }
}


void blank_path_new_method(byte count_new){
  struct point{
    byte row;
    byte col;
  };
  
  // Serial.println("main_count :"+String(count));
  for(byte i=1; i<count_new; i++){
    point P1;
    point P2;
    point blank;


    P1.row = main_path_new[i-1][0];
    P1.col = main_path_new[i-1][1];
  
    P2.row = main_path_new[i][0];
    P2.col = main_path_new[i][1];

    //add temp block at p1---------------------------
    block_loc(P1.row,P1.col,true);
    // print_block_position_img();
    //find blank location----------------------------
    byte* bl = find_blank_space_new();
    blank.row = bl[0];
    blank.col = bl[1];
    //find blank path---------------------------------
    Node* blank_path_node = AStarAlgorithm(P2.row,P2.col,blank.row,blank.col);


    byte blank_path_count_new = path_array_update_count(blank_path_node,false,true);
    Serial.println("P1: ("+ String(P1.row)+","+String(P1.col)+")");

    //print blank_path:
    print_path_array_new(blank_path_count_new,false,true);

    //update and move puzzle_pattern--------------------------------
    for(byte j=1; j<blank_path_count_new; j++){
      point p1;
      point p2;
    
      p1.row = blank_path_new[j-1][0];
      p1.col = blank_path_new[j-1][1];

      p2.row = blank_path_new[j][0];
      p2.col = blank_path_new[j][1];
    
      //update puzzle
      char temp = puzzle_pattern[p2.row][p2.col];
      puzzle_pattern[p1.row][p1.col] = temp;
      puzzle_pattern[p2.row][p2.col] = '0';
    
      // move SERVOS from p2 to p1
      //...
      servo_move(p2.row,p2.col,p1.row,p1.col);
      delay(50);
    }

    //update and move last remaining
    //from last location of blank_path to P1
    //Update puzzle
    char temp = puzzle_pattern[P1.row][P1.col];
    puzzle_pattern[P2.row][P2.col] = temp;
    puzzle_pattern[P1.row][P1.col] = '0';
    // move Servos ...
    // servo_move(P2.row,P2.col,P1.row,P1.col);
    servo_move(P1.row,P1.col,P2.row,P2.col);

    delay(50);

    //print puzzle_pattern
    // print_patterns(false);
    
    //remove temp block at p1
    block_loc(P1.row,P1.col,false);
  }
}


byte* find_blank_space_new(){
  static byte blank[2];
  for(byte i=0; i<25;i++){
    if(puzzle_pattern[i/5][i%5]=='0'){
      blank[0] = i/5;
      blank[1] = i%5;
      break;
    }
  }
  return blank;
}


void servo_move(byte p1_row,byte p1_col,byte p2_row,byte p2_col){
  float x1,y1,x2,y2;
  x1 = 62 + (28.2*p1_col);
  y1 = 56.4 - (28.2*p1_row);
  
  x1 += error_arr[p1_row][p1_col][0];
  y1 += error_arr[p1_row][p1_col][1];

  x2 = 62 + (28.2*p2_col);
  y2 = 56.4 - (28.2*p2_row);

  x2 += error_arr[p2_row][p2_col][0];
  y2 += error_arr[p2_row][p2_col][1];

  ik_new(x1,y1);
  myservo1.write(theta1);
  myservo2.write(theta2);
  delay(400);
  myservo3.write(down);
  delay(300);

  ik_new(x2,y2);
  myservo1.write(theta1);
  myservo2.write(theta2);
  delay(400);
  myservo3.write(up);
  delay(400);
}


void shuffle_color_Set(){
  // Fisher-Yates shuffle algorithm
  for (byte i = 23; i > 0; i--) {
    byte randomIndex = random(i + 1);
    char temp = color_Set[i];
    color_Set[i] = color_Set[randomIndex];
    color_Set[randomIndex] = temp;
  }
}


void random_patterns(){
  shuffle_color_Set();
  for(byte i=0;i<9;i++){
    target_pattern[i/3][i%3] = color_Set[i];
  }
  shuffle_color_Set();
  for(byte i=0; i<24;i++){
    puzzle_pattern[i/5][i%5] = color_Set[i];
  }

  byte randomIndex_i = random(5);
  byte randomIndex_j = random(5);
  if (randomIndex_i != 4 || randomIndex_j != 4){
    char temp = puzzle_pattern[randomIndex_i][randomIndex_j];
    puzzle_pattern[randomIndex_i][randomIndex_j]='0';
    puzzle_pattern[4][4]=temp;
  }
  else{
    puzzle_pattern[4][4]='0';
  }  
}


void ik_new(float x, float y){
  double r = sqrt(x*x + y*y);
  // Convert angles to degrees
  theta1 = degrees(atan2(y, x) - acos((L1*L1 + r*r - L2*L2) / (2 * L1 * r)));
  theta2 = degrees(M_PI - acos((L1*L1 + L2*L2 - r*r) / (2 * L1 * L2)));

  theta1 = round(130 + theta1);
  theta2 = round(175 - theta2);
}


void servo_go(){
  myservo3.write(up);
  delay(300);
  myservo1.write(theta1);
  myservo2.write(theta2);
  delay(500);
  myservo3.write(down);
  delay(300);
}



void print_patterns(bool target){
  if(target == true){
    Serial.println("Target_pattern\n..................");
    for(byte i =0; i<3; i++){
      Serial.print("\t");
      for(byte j=0; j<3; j++){
        Serial.print(String(target_pattern[i][j]) + " ");
      }
    Serial.println();
    }
  }
  

  Serial.println("\nPuzzle_pattern\n..................");
  for(byte i =0; i<5; i++){
    Serial.print("\t");
    for(byte j=0; j<5; j++){
      Serial.print(puzzle_pattern[i][j]);
      Serial.print(" ");
    }
    Serial.println();
  }
  Serial.println("..................");
}


bool check_puzzle(){
  byte count[7]={0,0,0,0,0,0,0}; ////B,R,G,Y,W,O,"0"
  for(byte row = 0; row <5; row++){
    for(byte col =0; col <5; col++){
      char current = puzzle_pattern[row][col];
      if (current == 'B')
      {
        count[0] +=1;
        if (count[0]>4){
          return false;
        }
      }
      else if (current == 'R')
      {
        count[1] +=1;
        if (count[1]>4){
          return false;
        }
      }
      else if (current == 'G')
      {
        count[2] +=1;
        if (count[2]>4){
          return false;
        }
      }
      else if (current == 'Y')
      {
        count[3] +=1;
        if (count[3]>4){
          return false;
        }
      }
      else if (current == 'W')
      {
        count[4] +=1;
        if (count[4]>4){
          return false;
        }
      }
      else if (current == 'O')
      {
        count[5] +=1;
        if (count[5]>4){
          return false;
        }
      }
      else if (current == '0')
      {
        count[6] +=1;
        if (count[6]>1){
          return false;
        }
      }
    }
  }
  return true;
}