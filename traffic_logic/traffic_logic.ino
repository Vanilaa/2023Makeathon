int light_duration = 5;
int max_duration = 15;
/*
  N, S, E, W
*/
int lane_cars[4];
int density_eval[4];
int no_density_eval[4];
int rate[] = {1,0,0,0}; 


bool ew_light = false;
bool ns_light = false;

int clk=0;
int max_clk=30;

int density_incoming = 0;
int no_density_incoming = 0;

int sum(int a[]){
  int s = 0;
  int len = sizeof(a) / sizeof(a[0]);
  for (int i = 0; i<len; i++){
    s+=a[i];
  }
  return s;
}

void setup() {
  Serial.begin(9600);
  lane_cars[0] = 20;
  lane_cars[1] = 5;
  lane_cars[2] = 10;
  lane_cars[3] = 10;
  int initial = sum(lane_cars);
  //lane_cars = {20,5,10,10};
  int density_wait_time = density();


  lane_cars[0] = 20;
  lane_cars[1] = 5;
  lane_cars[2] = 10;
  lane_cars[3] = 10;
  int no_density_wait_time = w_o_density();

  evaluation(initial,density_wait_time,no_density_wait_time);

  //Serial.println(density_incoming);
  //Serial.println(no_density_incoming);
  //Serial.println(density_wait_time/(initial+density_incoming));
  //Serial.println(no_density_wait_time/(initial+no_density_incoming));

  //delay(10000); //delay 10seconds between each case
}


void loop() {


}

int density(){
  clk = 0;
  int wait_time = 0;
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
    
    if (maxValue == 0){
      duration = light_duration;
      if (ns_light){
        ns_light = false;
        ew_light = true;
      }
      else{
        ns_light = true;
        ew_light = false;
      }
    }
    else{
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
    }

    while(clk<max_clk && duration >0){
      if (ew_light){
          lane_cars[0] += rate[0];
          lane_cars[1] += rate[1];
          lane_cars[2] = lane_cars[2]==0? 0: lane_cars[2]-1;
          lane_cars[3] = lane_cars[3]==0? 0: lane_cars[3]-1;

          wait_time +=sum(lane_cars);
          density_incoming += rate[0]+rate[1];
        }
      else if (ns_light){
          lane_cars[0] = lane_cars[0]==0? 0: lane_cars[0]-1;
          lane_cars[1] = lane_cars[1]==0? 0: lane_cars[1]-1;
          lane_cars[2] += rate[2];
          lane_cars[3] += rate[3];

          wait_time +=sum(lane_cars);
          density_incoming += rate[2]+rate[3];
      }
      duration -=1;
      Serial.println(clk);
      clk++;
      
      print_status();
      delay(100);

    }
  //yellow lights
  
  }
  memcpy(density_eval, lane_cars, sizeof(lane_cars));
  return wait_time;
}


int w_o_density(){
  clk=0;
  int wait_time = 0;
  while (clk < max_clk){
    if (clk % 10 <5){
      lane_cars[0] = lane_cars[0]==0? 0: lane_cars[0]-1;
      lane_cars[1] = lane_cars[1]==0? 0: lane_cars[1]-1;
      lane_cars[2]+=rate[2];
      lane_cars[3]+=rate[3];

      wait_time +=sum(lane_cars);
      no_density_incoming += rate[2]+rate[3];
    }
    else{
      lane_cars[0]+=rate[0];
      lane_cars[1]+=rate[1];
      lane_cars[2] = lane_cars[2]==0? 0: lane_cars[2]-1;
      lane_cars[3] = lane_cars[3]==0? 0: lane_cars[3]-1;

      wait_time +=sum(lane_cars);
      no_density_incoming += rate[0]+rate[1];
    }
    Serial.println(clk);
    print_status();
    clk++;
    delay(100);
    
    if(clk%5==0){
      //yellow light
      delay(500);
    }
  }
  memcpy(no_density_eval, lane_cars, sizeof(lane_cars));
  return wait_time;
}

void evaluation(int initial, int density_wait_time, int no_density_wait_time){
  // double incoming = max_clk;
  // double density_sum, no_density_sum =0;
  // for (int i=0; i<4;i++){
  //   no_density_sum += no_density_eval[i];
  //   density_sum += density_eval[i];  
  // }

  Serial.print("average wait time with optimization: ");
  Serial.println(density_wait_time/(initial+density_incoming));
  Serial.print("average wait time without optimization: ");
  Serial.println(no_density_wait_time/(initial+no_density_incoming));
}

void print_status(){
/*
      N
  W       E
      S
*/
  Serial.print("\t");
  Serial.println(lane_cars[0]);
  Serial.print(lane_cars[2]);
  Serial.print("\t\t");
  Serial.println(lane_cars[3]);
  Serial.print("\t");
  Serial.println(lane_cars[1]);
  Serial.println("\n");
}
