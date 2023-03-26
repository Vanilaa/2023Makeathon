
int light_duration = 5;
int max_duration = 15;

// struct lane{
//   int rate;
//   int cars;
// };

// lane* north;
// lane* south;
// lane* east;
// lane* west;

/*
  N, S, E, W
*/
int lane_cars[4];
int density_eval[4];
int no_density_eval[4];
int rate[] = {1,0,0,0}; //1 car per sec

int ew_total;
int ns_total;

bool ew_light;
bool ns_light;

int clk=0;
int max_clk=60;

void setup() {
  lane_cars[0] = 20;
  lane_cars[1] = 5;
  lane_cars[2] = 10;
  lane_cars[3] = 10;
  //lane_cars = {20,5,10,10};
  density();
  w_o_density();
  evaluation();

  delay(10000); //delay 10seconds between each case
}


void loop() {
  

}

void density(){
  clk = 0;
  while(clk<max_clk){
    int maxIndex = 0;
    int maxValue = lane_cars[maxIndex];
    int duration = 0;
    for(int i = 1; i < 4; i++){
      if(lane_cars[i] > maxValue) {
          maxValue = lane_cars[i];
          maxIndex = i;
      }
    }

    duration = maxValue>max_duration ? max_duration : maxValue;

    if (maxIndex == 0 || maxIndex ==1){
      ns_light = true;
      ew_light = false;
      //set lights
    }
    else{
      ns_light = false;
      ew_light = true;    
      //set lights
    }

    while(duration >0){
      if (ew_light){
          lane_cars[0] += rate[0];
          lane_cars[1] += rate[1];
          lane_cars[2] -= 1;
          lane_cars[3] -= 1;
        }
      else if (ns_light){
          lane_cars[0] -= 1;
          lane_cars[1] -= 1;
          lane_cars[2] += rate[0];
          lane_cars[3] += rate[1];
      }
      duration -=1;
      print_status();
      delay(1000);

    }
  //yellow lights
  memcpy(density_eval, lane_cars, sizeof(lane_cars));
  }
}


void w_o_density(){
  clk=0;
  while (clk < max_clk){
    if (clk % 10 <5){
      lane_cars[0]--;
      lane_cars[1]--;
      lane_cars[2]+=rate[2];
      lane_cars[3]+=rate[3];
    }
    else{
      lane_cars[0]+=rate[0];
      lane_cars[1]+=rate[1];
      lane_cars[2]--;
      lane_cars[3]--;
    }
    print_status();
    clk++;
    delay(1000);
    
    if(clk%5==0){
      //yellow light
      delay(500);
    }
  }
  memcpy(no_density_eval, lane_cars, sizeof(lane_cars));
}

void evaluation(){
  int incoming = max_clk;
  int density_sum, no_density_sum =0;
  for (int i=0; i<4;i++){
    no_density_sum += no_density_eval[i];
    density_sum += density_eval[i];  
  }
  Serial.print("with optimization: ");
  Serial.println( density_sum / incoming);  
  Serial.print("without optimization: ");
  Serial.println(no_density_sum / incoming);
}

void print_status(){
/*
      N
  W       E
      S
*/
  Serial.print("\t");
  Serial.println(lane_cars[0]);
  Serial.print(lane_cars[3]);
  Serial.print("\t\t");
  Serial.println(lane_cars[2]);
  Serial.print("\t");
  Serial.println(lane_cars[1]);
  Serial.println("\n");
}
