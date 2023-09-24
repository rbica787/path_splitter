/*********************************************************************************************
 * DESCRIPTION:
 * this code is for automatically generating 
 * a center point and a lead-out/lead-in trend  
 * between two 3-dimensional coordinates.
 * This code is intended for use prior to any
 * other directional program manipulation.
 * In other words, the lead-in path it generates
 * would normally become the lead-out path
 * after reversal.
 * 
 * 
 * TO USE:
 * take 2 coordinates in a 
 * trim program, copy and 
 * paste them into the 
 * Serial monitor (Ctrl+Shift+M),
 * press enter
 * [remember the sketch needs to be
 * uploaded (Ctrl+U)to an 
 * arduino in order to run].
 * 
 * 
 * AFTER GENERATING A PATH, MAKE SURE TO VERIFY:
 * points A and B (in the printed header)
 * the blade directional description (in the printed header)
 * 
 * 
 * Last tried and true update:             
 * 65
 * 
 * Last Update:
 * 03/25/2022 - added an itty bitty println for an unlikely event
 *              
 *******************************************************************************************/
//#define DEBUG
#define increment 0.050 //this is the modifiable difference
                        //between trend points in mm

const float safety_Factor = 2;    //this can be pretty much anything greater than 1
float difference = 0;        //this is just to wipe any data that could be stored at this memory
float minDiff;

#define TOTAL_AXES 6
#define trend_count 5    //the amount of points in my lead in/lead out trend plus 1

float X1;
float Y1;
float Z1;

float X2;
float Y2;
float Z2;



float A_B_coordinate[6];
float centerPoint[3];
float Z_trend[3];
const float Z_increment[3] = {10.000, 30.000, 60.000};


int counter = 0;


void setup() {
  Serial.begin(9600);
}

void loop() {
   int incoming_data = Serial.available();
   while(incoming_data > 0){
      if(counter < TOTAL_AXES){
       A_B_coordinate[counter] = Serial.parseFloat();     
      }
      else{
       Serial.parseFloat();
      }
      counter++;
      incoming_data = Serial.available();
     }
  boolean buffer_is_empty = Serial.available() < 1;
  boolean not_enough = 0 < counter && counter < TOTAL_AXES + 1;
  boolean too_many = counter > TOTAL_AXES + 1;
  boolean just_right = counter == TOTAL_AXES + 1;

  if(buffer_is_empty){
    if(not_enough){
      counter = 0;
      delete_values();
      print_notEnough();;
    }
    else if(too_many){
      counter = 0;
      delete_values();
      print_tooMany();
    }
    else if (just_right){
      counter = 0;
      create_path();
      delete_values();
    }
  }
}





void delete_values(){
  memset(A_B_coordinate, 0, sizeof(A_B_coordinate));
  memset(centerPoint, 0, sizeof(centerPoint));
  memset(Z_trend, 0, sizeof(Z_trend));
  difference = 0;
}


float x_value(float the_slope, float b, float y){
  float find_x = (y - b)/the_slope;
  return find_x;
}

float y_value(float theSlope, float bb, float x){
  float find_y = (theSlope * x) + bb;
  return find_y;
}

float midpoint(float n1, float n2){
  float center = (n1 + n2)/2;
  return center;
}

float distance_2Dim(float x1val, float x2val, float y1val, float y2val){
  float squareX = pow((x2val - x1val), 2);
  float squareY = pow((y2val - y1val), 2);
  float D_2d = sqrt(squareX + squareY);
  return D_2d;
}

float distance_1Dim(float point1, float point2){
  float D_1d = point1 - point2;
  D_1d = abs(D_1d);
  return D_1d;
}

void printDistance(){
  if(X1 != X2 && Y1 == Y2){     //horizontal line; 1-dimensional
    difference = distance_1Dim(X1, X2);
  }
    else if(X1 == X2 && Y1 != Y2){    //vertical line; 1-dimensional
      difference = distance_1Dim(Y1, Y2);
  }
      else{
        difference = distance_2Dim(X1, X2, Y1, Y2);    //2-dimensional line
  }
  const float safeZone = increment*(trend_count*2);
  minDiff = safeZone*safety_Factor;
  Serial.print("difference between points: ");
  Serial.print(difference, 3);
  Serial.println("mm");
  Serial.print("minimum distance needed: ");
  Serial.print(minDiff, 3);
  Serial.println("mm");
}

//this verifies that the difference between points is greater then the minimum difference needed
boolean safetyCheck(){
  boolean diffComparison = difference > minDiff;
  if(!diffComparison){
   Serial.println("_______________________________________________________________________");
   Serial.println(" ");
   Serial.println(" ");
   Serial.println("INVALID; trend path exceeds safe zone");
   Serial.println(" ");
   Serial.println("**Check difference between points**");
   Serial.println(" ");
   Serial.println(" ");
  }
  return diffComparison;
}

void assign_AndPrint_midpoint(){
  centerPoint[0] = midpoint(X1, X2);
  centerPoint[1] = midpoint(Y1, Y2);
  centerPoint[2] = midpoint(Z1, Z2);
   Serial.print("center point: X");
   Serial.print(centerPoint[0], 3);
   Serial.print(" Y");
   Serial.print(centerPoint[1], 3);
   Serial.print(" Z");
   Serial.println(centerPoint[2], 3);
   Serial.print('\n');
}

void print_notEnough(){
   Serial.println(" ");
   Serial.println(" ");
   Serial.println("INVALID; not enough values");
   Serial.println(" ");
   Serial.println("**please enter 6 coordinates**");
   Serial.println(" ");
   ad_finem();
}

void print_tooMany(){
   Serial.println(" ");
   Serial.println(" ");
   Serial.println("INVALID; too many values");
   Serial.println(" ");
   Serial.println("**please enter 6 coordinates**");
   Serial.println(" ");
   Serial.println(" ");
   ad_finem();
}

void P310(){
   Serial.println("P310 FP311");
   Serial.println(";");
   Serial.println("#TIME 0.1");
   Serial.println(";");
}

void print_EndHeader(){
    Serial.println("_______________________________________________________________________");
    Serial.println(";");
    Serial.println(";TREND:");
}

void ad_finem(){
   Serial.println("_____________________________________________________________________________________________");
   Serial.println("_____________________________________________________________________________________________");
   Serial.println(" ");
}

void endLead_out(){
   Serial.println(";CAMPath End");
   Serial.println(";");
   Serial.println(";CAMPath Start");
   Serial.println(";");
}

void print_common(){
   Serial.println(";");
   Serial.print("X");
   Serial.print(centerPoint[0], 3);
   Serial.print(" Y");
   Serial.print(centerPoint[1], 3);
   Serial.print(" Z");
   Serial.println(centerPoint[2], 3);
   Serial.println(";");
}


void print_A_B_coordinates(){
    Serial.print("point A: X");
    Serial.print(A_B_coordinate[0], 3);
    Serial.print(" Y");
    Serial.print(A_B_coordinate[1], 3);
    Serial.print(" Z");
    Serial.println(A_B_coordinate[2], 3);
    Serial.print("point B: X");
    Serial.print(A_B_coordinate[3], 3);
    Serial.print(" Y");
    Serial.print(A_B_coordinate[4], 3);
    Serial.print(" Z");
    Serial.println(A_B_coordinate[5], 3);
    Serial.println(" ");
}

void print_XY(float xxx, float yyy){
     Serial.print("X");           
     Serial.print(xxx, 3);
     Serial.print(" Y");
     Serial.print(yyy, 3);
}


void create_path(){
  float Rise;
  float Run;
  float MM;
  float BB;
  print_A_B_coordinates();

     //this assigns familiar linear variables
     //for easier calculations
     X1 = A_B_coordinate[0];
     Y1 = A_B_coordinate[1];
     Z1 = A_B_coordinate[2];
     X2 = A_B_coordinate[3];
     Y2 = A_B_coordinate[4];
     Z2 = A_B_coordinate[5];

       Rise = Y2 - Y1;
       Run = X2 - X1;

    if(Run == 0 && Rise != 0){
       Serial.println("slope = VERTICAL LINE, NO SLOPE");
       Serial.println("y-intercept = DOESNT EXIST");
       assign_AndPrint_midpoint();
    }
      else if(Run == 0 && Rise == 0)     //an unlikely coincidence
       Serial.println("...a wise guy huh?... @_@ ... you know what you did");
        else{
         MM = Rise/Run;
           Serial.print("slope = ");
           Serial.println(MM,3);
           assign_AndPrint_midpoint();
    
         BB = Y1 - (MM * X1);
          Serial.print("y-intercept = ");
          Serial.println(BB,3);
        }
          
       //this will assign my Z axis trend values
       for(int j = 0; j < 3; j++){
        Z_trend[j] = centerPoint[2] + Z_increment[j];
       }

   trend_generator(MM, BB, Run);
   ad_finem();
}



//this is where the magic happens
void trend_generator(float M, float B, float RUN){
  boolean vertical_line = RUN == 0;
  boolean greater_then_45degrees = M < -1 || M > 1;
  boolean less_then_45degrees = M > -1 && M < 1;
    
   if(vertical_line){
    if(Y1 < Y2){    //THIS IS A VERTICAL LINE GOING UP
      printDistance();
      Serial.println("THE BLADE IS TRAVELING POSITIVE ON THE Y AXES");
      if (!safetyCheck())
         return;
      print_EndHeader();
      print_common();
      for(int i = 1; i < trend_count; i++){   //lead-out
        float xx1 = centerPoint[0];
        float yy1 = centerPoint[1] + (i * increment);
        print_XY(xx1, yy1);
           Serial.print(" Z");
        if(i < trend_count-1){
           Serial.println(Z_trend[i-1], 3);
          }
        else{
          P310();
          endLead_out();
          }
      }                                    //end of lead-out
           
           float xx2 = centerPoint[0];     //lead-in
           float yy2 = centerPoint[1] - (increment *(trend_count-1));
           print_XY(xx2, yy2);
           Serial.println(" ZP310 FP311");           
      for(int q = 8; q > trend_count; q--){
        int f = q - trend_count;
        float xx3 = centerPoint[0];
        float yy3 = centerPoint[1] - (f * increment);
        print_XY(xx3, yy3);
          Serial.print(" Z");
          Serial.println(Z_trend[f-1], 3);
      }                                  //end lead-in
          print_common();
    }
      else if(Y1 > Y2){   //THIS IS A VERTICAL LINE GOING DOWN
        printDistance();
        Serial.println("THE BLADE IS TRAVELING NEGATIVE ON THE Y AXES");
        if (!safetyCheck())
           return;
        print_EndHeader();
        print_common();
        for(int i = 1; i < trend_count; i++){   //lead-out
          float xx4 = centerPoint[0];
          float yy4 = centerPoint[1] - (i * increment);
          print_XY(xx4, yy4);
           Serial.print(" Z");
          if(i < trend_count-1){
            Serial.println(Z_trend[i-1], 3);
          }
           else{
          P310();
          endLead_out();
          }
      }                                //end of lead-out

           float xx5 = centerPoint[0];           //lead-in
           float yy5 = centerPoint[1] + (increment *(trend_count-1));
           print_XY(xx5, yy5);
           Serial.println(" ZP310 FP311");
      for(int q = 8; q > trend_count; q--){
        int f = q - trend_count;
        float xx6 = centerPoint[0]; 
        float yy6 = centerPoint[1] + (f * increment);
        print_XY(xx6, yy6);
          Serial.print(" Z");
          Serial.println(Z_trend[f-1], 3);
      }                                  //end lead-in
          print_common();
    }
   }
   else{
     if(greater_then_45degrees){
      if(Y1 < Y2){ //THE BLADE IS TRAVELING POSITIVE ON THE Y AXES
          printDistance();
          Serial.println("THE BLADE IS TRAVELING POSITIVE ON THE Y AXES");
          if (!safetyCheck())
             return;
          print_EndHeader();
          print_common();
          for(int i = 1; i < trend_count; i++){   //lead-out
           float yy = centerPoint[1] + (i * increment);
           float xx = x_value(M, B, yy);
           print_XY(xx, yy);
           Serial.print(" Z");
          if(i < trend_count-1){
            Serial.println(Z_trend[i-1], 3);
          }
           else{
          P310();
          endLead_out();
          }
      }                                //end of lead-out
           float y3 = centerPoint[1] - (increment *(trend_count-1));   //lead-in
           float x3 = x_value(M, B, y3);
           print_XY(x3, y3);
           Serial.println(" ZP310 FP311");
      for(int q = 8; q > trend_count; q--){
        int f = q - trend_count;
        float y4 = centerPoint[1] - (f * increment);
        float x4 = x_value(M, B, y4);
          print_XY(x4, y4);
          Serial.print(" Z");
          Serial.println(Z_trend[f-1], 3);
      }                                  //end lead-in
          print_common();
        }
            else if(Y1 > Y2){  //THE BLADE IS TRAVELING NEGATIVE ON THE Y AXES
             printDistance();
             Serial.println("THE BLADE IS TRAVELING NEGATIVE ON THE Y AXES");
             if (!safetyCheck())
                return;
             print_EndHeader();
             print_common();
             for(int i = 1; i < trend_count; i++){   //lead-out
              float y5 = centerPoint[1] - (i * increment);
              float x5 = x_value(M, B, y5);
              print_XY(x5, y5);
              Serial.print(" Z");
             if(i < trend_count-1){
               Serial.println(Z_trend[i-1], 3);
             }
              else{
             P310();
             endLead_out();
               }
             }                                //end of lead-out
              float y6 = centerPoint[1] + (increment *(trend_count-1));   //lead-in
              float x6 = x_value(M, B, y6);
              print_XY(x6, y6);
              Serial.println(" ZP310 FP311");
         for(int q = 8; q > trend_count; q--){
           int f = q - trend_count;
           float y7 = centerPoint[1] + (f * increment);
           float x7 = x_value(M, B, y7);
             print_XY(x7, y7);
             Serial.print(" Z");
             Serial.println(Z_trend[f-1], 3);
      }                                  //end lead-in
             print_common();
            }
     }
      else{
                 if(X1 < X2){   //THE BLADE IS TRAVELING POSITIVE ON THE X AXIS (or positive on a 45 degree angle)
                    printDistance();
                    if(less_then_45degrees)
                      Serial.println("THE BLADE IS TRAVELING POSITIVE ON THE X AXIS");
                     else
                      Serial.println("THE BLADE IS TRAVELING POSITIVE ON THE X AXIS AT A 45 DEGREE ANGLE");
                    if (!safetyCheck())
                      return;
                    print_EndHeader();
                    print_common();
                    for(int i = 1; i < trend_count; i++){   //lead-out
                      float x8 = centerPoint[0] + (i * increment);
                      float y8 = y_value(M, B, x8);
                      print_XY(x8, y8);
                      Serial.print(" Z");
                      if(i < trend_count-1){
                        Serial.println(Z_trend[i-1], 3);
                     }
                      else{
                       P310();
                       endLead_out();
                     }
                   }                                //end of lead-out
                     float x9 = centerPoint[0] - (increment *(trend_count-1));   //lead-in
                     float y9 = y_value(M, B, x9);
                     print_XY(x9, y9);
                     Serial.println(" ZP310 FP311");
                     for(int q = 8; q > trend_count; q--){
                       int f = q - trend_count;
                       float x33 = centerPoint[0] - (f * increment);
                       float y33 = y_value(M, B, x33);
                       print_XY(x33, y33);
                       Serial.print(" Z");
                       Serial.println(Z_trend[f-1], 3);
                       }                                  //end lead-in
                       print_common();
                    }
                      else if(X1 > X2){     //THE BLADE IS TRAVELING NEGATIVE ON THE X AXIS (or negative on a 45 degree angle)
                         printDistance();
                         if(less_then_45degrees)
                          Serial.println("THE BLADE IS TRAVELING NEGATIVE ON THE X AXIS");
                          else
                           Serial.println("THE BLADE IS TRAVELING NEGATIVE ON THE X AXIS AT A 45 DEGREE ANGLE");
                         if (!safetyCheck())
                           return;
                         print_EndHeader();
                         print_common();
                         for(int i = 1; i < trend_count; i++){   //lead-out
                         float x44 = centerPoint[0] - (i * increment);
                         float y44 = y_value(M, B, x44);
                         print_XY(x44, y44);
                         Serial.print(" Z");
                         if(i < trend_count-1){
                           Serial.println(Z_trend[i-1], 3);
                        }
                         else{
                          P310();
                          endLead_out();
                        }
                      }                                //end of lead-out
                        float x55 = centerPoint[0] + (increment *(trend_count-1));   //lead-in
                        float y55 = y_value(M, B, x55);
                        print_XY(x55, y55);
                        Serial.println(" ZP310 FP311");
                        for(int q = 8; q > trend_count; q--){
                          int f = q - trend_count;
                          float x66 = centerPoint[0] + (f * increment);
                          float y66 = y_value(M, B, x66);
                          print_XY(x66, y66);
                          Serial.print(" Z");
                          Serial.println(Z_trend[f-1], 3);
                          }                                  //end lead-in
                          print_common();
                       }
                   } 
                }  
            } 
