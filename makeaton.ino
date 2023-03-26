// Sensors Init
int YELLOW = 4;
int NS_GREEN = 3;
int EW_GREEN = 2;
int total_led_num = 5;
int normal_delay = 5000;
int yellow_delay = 1000;
// int sound_analog = A0;
// int sound_digital = 13;
int sound_value = 0;
int noise_flag = 0;
int JOYSTICK_X = A2;
int JOYSTICK_Y = A1;
int x_value = 0;
int y_value = 0;
int timer1_counter;
int joy_stick_cars = 0;


#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Logic Init
int light_duration = 5;
int max_duration = 15;
/*
  N, S, E, W
*/
int lane_cars[4];
int density_eval[4];
int no_density_eval[4];
int rate[] = {0,0,0,0}; 

bool ew_light = false;
bool ns_light = false;

int clk=0;
int max_clk=30;

int wait_time;

int density_incoming = 0;
int no_density_incoming = 0;

int sum(int a[]){
  // int s = 0;
  // int len = sizeof(a) / sizeof(a[0]);
  // for (int i = 0; i<len; i++){
  //   s+=a[i];
  // }
  return a[0]+a[1]+a[2]+a[3];
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(NS_GREEN, OUTPUT);
  pinMode(EW_GREEN, OUTPUT);
  pinMode(YELLOW, OUTPUT);
  //pinMode(sound_analog, INPUT);
  pinMode(JOYSTICK_X, INPUT);
  pinMode(JOYSTICK_X, INPUT);

    // Display Setup
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  noDisplay();
  // Interrupt Setup
  noInterrupts();           // disable all interrupts

  TCCR1A = 0;
  TCCR1B = 0;
  timer1_counter = 31250/3; //34286;   // preload timer 65536-16MHz/256/2Hz
  TCNT1 = timer1_counter;   // preload timer
  TCCR1B |= (1 << CS12);    // 256 prescaler
  TIMSK1 |= (1 << TOIE1);   // enable timer overflow interrupt
  interrupts();             // enable all interrupts
  
  lane_cars[0] = 20;
  lane_cars[1] = 18;
  lane_cars[2] = 4;
  lane_cars[3] = 8;
  int initial = sum(lane_cars);
  //lane_cars = {20,5,10,10};
  // Serial.println("start density");
  int density_wait_time = density();
  int joystick_density = joy_stick_cars;
  joy_stick_cars = 0;

  lane_cars[0] = 20;
  lane_cars[1] = 18;
  lane_cars[2] = 4;
  lane_cars[3] = 8;
  // Serial.println("start without density");

  int no_density_wait_time = w_o_density();

  evaluation(initial,density_wait_time,no_density_wait_time,joystick_density);

  ALL_OFF();
}

void loop() {
  // if(lane_cars[0]+lane_cars[1]+lane_cars[2]+lane_cars[3] > 10){
  //   heavy();
  // }
  // else{
  //   normal();
  // }
  //  NS_FLOW();
  //  delay(5000);
  //  YELLOW_STATE();
  //  delay(1000);
  //  EW_FLOW();
  //  delay(5000);
}

void ALL_OFF(){
  for(int i = 2; i <= total_led_num+1; i++){
    digitalWrite(i, LOW);
  }
}

void NS_FLOW(){
  ALL_OFF();
  digitalWrite(NS_GREEN, HIGH);
}

void EW_FLOW(){
  ALL_OFF();
  digitalWrite(EW_GREEN, HIGH);
}

void YELLOW_STATE(){
  ALL_OFF();
  digitalWrite(YELLOW, HIGH);
}

ISR(TIMER1_OVF_vect)        // interrupt service routine every one second
{
  TCNT1 = timer1_counter;   // preload timer
  x_value = analogRead(JOYSTICK_X);
  y_value = analogRead(JOYSTICK_Y);
  if(x_value == 0){
    lane_cars[0] = lane_cars[0] + 1;
    joy_stick_cars = joy_stick_cars + 1;
    Serial.println("North +1");
  }
  if(x_value == 1023){
    lane_cars[1] = lane_cars[1] + 1;
    joy_stick_cars = joy_stick_cars + 1;
    Serial.println("South +1");
    // Serial.println(lane_cars[1]);
  }
  if(y_value == 0){
    lane_cars[2] = lane_cars[2] + 1;
    joy_stick_cars = joy_stick_cars + 1;
    Serial.println("East +1");
    // Serial.println(lane_cars[2]);
  }
  if(y_value == 1023){
    lane_cars[3] = lane_cars[3] + 1;
    joy_stick_cars = joy_stick_cars + 1;
    Serial.println("West +1");
    // Serial.println(lane_cars[3]);
  }
}

void heavy(void) {
  display.clearDisplay();
  display.setTextSize(2); // Draw 2X-scale text
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10, 0);
  display.println(F("Heavy:("));
  display.display();      // Show initial text
}

void normal(void) {
  display.clearDisplay();
  display.setTextSize(2); // Draw 2X-scale text
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10, 0);
  display.println(F("Normal:)"));
  display.display();      // Show initial text
}

void noDisplay(void) {
  display.clearDisplay();
  display.setTextSize(2); // Draw 2X-scale text
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10, 0);
  display.println(F(""));
  display.display();      // Show initial text
}

int density(){
  clk = 0;
  wait_time = 0;
  while(clk<max_clk && sum(lane_cars)!=0){
    //Serial.println("start loop density");
    //Serial.println(clk);
    double sum_x = lane_cars[2] + lane_cars[3];
    double sum_y = lane_cars[0] + lane_cars[1];
    
    int duration = 0;

    if (sum_x/sum_y <0.5 || sum_x/sum_y >2) {
      //Serial.println("sum");
      //duration = sum_priority(sum_x,sum_y);
      duration = max_priority(); 
      //Serial.println(duration);
      
    }
    else{ 
      //Serial.println("max");
      duration = max_priority(); 
      //duration = sum_priority(sum_x,sum_y);
      //Serial.println(duration);
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
        Serial.println("------------------------------");
      Serial.println(clk);
      clk++;
      
      print_status();
      //Serial.print("sum lane: ");
      //Serial.println(sum(lane_cars));
      delay(1000);

    }
    YELLOW_STATE();
    delay(500);
  }
  // memcpy(density_eval, lane_cars, sizeof(lane_cars));
  return wait_time;
}

int sum_priority(double sum_x,double sum_y){
  int duration =0;

  if (sum_x > sum_y){
    duration = sum_x / (sum_x + sum_y ) * light_duration*2;
    ns_light = false;
    ew_light = true;
    EW_FLOW();
  }
  else {
    duration = sum_y / (sum_x + sum_y ) * light_duration*2;
    ns_light = true;
    ew_light = false;
    NS_FLOW();
  }

  duration = duration > max_duration ? max_duration : duration;
  return duration;
}

int max_priority (){
  int maxIndex = 0;
  int maxValue = lane_cars[0];

  for(int i = 1; i < 4; i++){
    if(lane_cars[i] > maxValue) {
        maxValue = lane_cars[i];
        maxIndex = i;
    }
  }
  int duration = maxValue > max_duration ? max_duration : maxValue;
    
  if (maxValue == 0){
    duration = light_duration;
    if (ns_light){
      ns_light = false;
      ew_light = true;
      EW_FLOW();
    }
    else{
      ns_light = true;
      ew_light = false;
      NS_FLOW();
    }
  }
  else{
    if (maxIndex == 0 || maxIndex ==1){
      ns_light = true;
      ew_light = false;
      NS_FLOW();
      //set lights
    }
    else{
      ns_light = false;
      ew_light = true;
      EW_FLOW();   
      //set lights
    }
  }
  return duration;
}

int w_o_density(){
  clk=0;
  wait_time = 0;
  while (clk < max_clk){
    if (clk % 10 <5){
      lane_cars[0] = lane_cars[0]==0? 0: lane_cars[0]-1;
      lane_cars[1] = lane_cars[1]==0? 0: lane_cars[1]-1;
      lane_cars[2]+=rate[2];
      lane_cars[3]+=rate[3];
      NS_FLOW();
      wait_time +=sum(lane_cars);
      no_density_incoming += rate[2]+rate[3];
    }
    else{
      lane_cars[0]+=rate[0];
      lane_cars[1]+=rate[1];
      lane_cars[2] = lane_cars[2]==0? 0: lane_cars[2]-1;
      lane_cars[3] = lane_cars[3]==0? 0: lane_cars[3]-1;
      EW_FLOW();
      wait_time +=sum(lane_cars);
      no_density_incoming += rate[0]+rate[1];
    }
    Serial.println("------------------------------");
    Serial.println(clk);
    print_status();
    clk++;
    
    delay(1000);
    
    if(clk%5==0){
      //yellow light
      YELLOW_STATE();
      delay(500);
    }
  }
  memcpy(no_density_eval, lane_cars, sizeof(lane_cars));
  return wait_time;
}

void evaluation(int initial, int density_wait_time, int no_density_wait_time,int joystick_density){
  // double incoming = max_clk;
  // double density_sum, no_density_sum =0;
  // for (int i=0; i<4;i++){
  //   no_density_sum += no_density_eval[i];
  //   density_sum += density_eval[i];  
  // }

  Serial.print("average wait time with optimization: ");
  Serial.println(double(density_wait_time)/(initial+density_incoming+joystick_density));
  //Serial.print(density_wait_time);
  Serial.print("average wait time without optimization: ");
  Serial.println(double(no_density_wait_time)/(initial+no_density_incoming+joy_stick_cars));
  //Serial.print(no_density_wait_time);
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
  Serial.println("------------------------------");
  if(lane_cars[0]+lane_cars[1]+lane_cars[2]+lane_cars[3] > 25){
    heavy();
  }
  else{
    normal();
  }
}
