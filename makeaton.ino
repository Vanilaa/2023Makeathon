int N_RED = 2;
int N_YELLOW = 3;
int N_GREEN = 4;
int total_led_num = 3;
int normal_delay = 5000;
int yellow_delay = 1000;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(N_GREEN, OUTPUT);
  pinMode(N_YELLOW, OUTPUT);
  pinMode(N_RED, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  NS_FLOW();
  delay(normal_delay);
  YELLOW_STATE();
  delay(yellow_delay);
  EW_FLOW();
  delay(normal_delay);
}

void ALL_OFF(){
  for(int i = 2; i <= total_led_num+1; i++){
    digitalWrite(i, LOW);
  }
}

void NS_FLOW(){
  ALL_OFF();
  digitalWrite(N_GREEN, HIGH);
}

void EW_FLOW(){
  ALL_OFF();
  digitalWrite(N_RED, HIGH);
}

void YELLOW_STATE(){
  ALL_OFF();
  digitalWrite(N_YELLOW, HIGH);
}

