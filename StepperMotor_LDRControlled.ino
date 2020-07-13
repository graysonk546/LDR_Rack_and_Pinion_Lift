
/*defining constants*/
#define STEPS_PER_REV 32
#define GEAR_REDUX 64
const int STEPS_PER_OUT_REV = STEPS_PER_REV*GEAR_REDUX;

#define LDR_PIN A0
#define NUMBER_OF_SAMPLES 50
#define DATA_DELAY 1
#define START_BUTTON 12
#define STOP_BUTTON 7
#define ON 1
#define OFF 0

#define STEP_DELAY 1
#define UP 1
#define DOWN 2
#define STATIC 0
#define RUN_SLIDER 1
#define FIND_LIGHT_OPPOSITE 0
#define FIND_LIGHT_SAME 2
#define INITIALIZE 0
#define TAKE_MAX 1
#define PRODUCE_MAX_VALUE 2
#define STOP_AT_MAX 3
#define ONE_TIME 1

#define MAX_DISTANCE 400
#define ECHO_PIN 2
#define TRIGGER_PIN 3
#define MAX_HEIGHT 16
#define MIN_HEIGHT 2
#define MID_HEIGHT 9

/*including libraries*/
#include <Stepper.h> 
#include <NewPing.h>

/*sets up stepper motor as an object, initializes pins*/
Stepper steppermotor(STEPS_PER_REV, 8, 10, 9, 11);
NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);

/*variable used to track whether button has been pressed or not*/
int OnOrOff;

/*used in OneStep function*/
int SmallIncrement;

/*used to track direction of travel and position*/
int Direction;
int Position;
int Identifier;
int Status = RUN_SLIDER;
int count = 0;
int TakeMaxIndicator;
int OneTimeIndicator;
double Max = 0;
double Sum;
double MaxAverage;
int NumberValues;


/*used in IntensityCheck function*/
double Value;
double Avg;

/*used for search light function*/
double data1;
double data2;

/*variables for 'for' loops*/
int index;

/*variables for FindMax function*/
double maximum;

/*----------------------------------------------------------*/

void setup() {
  
  /*intitializing button pins as inputs*/
  pinMode(LDR_PIN,INPUT);
  pinMode(START_BUTTON,INPUT);
  pinMode(STOP_BUTTON,INPUT);

  /*initializing sonar pins*/
  pinMode(TRIGGER_PIN,OUTPUT);
  pinMode(ECHO_PIN,INPUT);

  /*initializing serial monitor*/
  Serial.begin(9600);
}

/*----------------------------------------------------------*/

/*function for rotating motor small amount*/
void OneStep(int UpOrDown);
/*function to execute sonar auto-centering*/
int HeightCheck(void);
/*checks variations in light intensity*/
double IntensitySample(void);
/*moves slider to closest threshold height (max or min) then to other threshold*/
void MoveSlider(int Status);
/*function for locating source and finding center*/
void FindLight(void);
/*function for finding center of light*/
double FindMax(double number1, double number2);

/*----------------------------------------------------------*/

void loop() {
  if(digitalRead(START_BUTTON) == HIGH){
      OnOrOff = ON;
      while(OnOrOff == ON){
        FindLight();
        if(digitalRead(STOP_BUTTON) == HIGH){
          OnOrOff = OFF;
        }
      }
  }
}

/*----------------------------------------------------------*/

void FindLight(void){
  
  data1 = IntensitySample();

  Serial.println(" ");
  Serial.print(data1);

  MoveSlider(Status);

  data2 = IntensitySample();

  Serial.println(" ");
  Serial.print(data2);

  
  if(abs(data1-data2) < 7){
    Status = RUN_SLIDER;
    if(TakeMaxIndicator == TAKE_MAX || TakeMaxIndicator == PRODUCE_MAX_VALUE && OneTimeIndicator == ONE_TIME){
      Max = FindMax(data1,data2);
      Serial.println(" ");
      Serial.println(" ");
      Serial.print(Max);
      Serial.println(" ");
      Sum += Max;
      NumberValues++;
      if(TakeMaxIndicator == PRODUCE_MAX_VALUE && OneTimeIndicator == ONE_TIME){
        MaxAverage = Sum/NumberValues;
        Serial.println(" ");
        Serial.println(" ");
        Serial.println(" ");
        Serial.println("MAX AVERAGE");
        Serial.print(MaxAverage);
        Serial.println(" ");
        Serial.println(" ");
        Serial.println(" ");
        while(data1 >= MaxAverage - 5 || data2 >= MaxAverage - 5){
          delay(2000);
          data1 = IntensitySample();
          data2 = IntensitySample();
          /*run a function to control the servo / picking up mechanism*/
        }
        Sum = INITIALIZE;
        NumberValues = INITIALIZE;
        OneTimeIndicator = INITIALIZE;
      }
    }
  }
  else{
    
    if(data1 > data2){
      count++;
      Status = FIND_LIGHT_OPPOSITE;
      Serial.println(" ");
      Serial.print("Travel Opposite Direction");
      if(count >= 10 && count < 15){
        TakeMaxIndicator = TAKE_MAX;
      }
      else{
        if(count > 15 || count < 10){
          /*TakeMaxIndicator = INITIALIZE;
          if(count > 16){
            count = INITIALIZE;
            OneTimeIndicator = ONE_TIME;
          }*/
          if(count == 16){
            TakeMaxIndicator == STOP_AT_MAX;
          }
          else{
            TakeMaxIndicator = INITIALIZE;
            if(count > 16){
              count = INITIALIZE;
              OneTimeIndicator = ONE_TIME;
            }
          }
        }
        else{
          TakeMaxIndicator = PRODUCE_MAX_VALUE;
        }
      }
    }
    else{
      Status = FIND_LIGHT_SAME;
      Serial.println(" ");
      Serial.print("Travel Same Direction");        
    }
  }
}

/*----------------------------------------------------------*/

double FindMax(double number1, double number2){

  if(number1 > number2){
    maximum = number1;
  }
  else{
    if(number1 < number2){
      maximum = number2;
    }
    else{
      maximum = number2;
    }
  }
  return maximum;
}

/*----------------------------------------------------------*/

void MoveSlider(int Value){

  if(Value == RUN_SLIDER){
    
    Position = HeightCheck();
    OneStep(Direction);

    if(MAX_HEIGHT - Position + MIN_HEIGHT < 9 && Position < 15 && Identifier != 1 && Identifier != 2){
      Direction = UP;
    }
    else{
      if(MAX_HEIGHT - Position + MIN_HEIGHT >= 9 && Position > 2 && Identifier != 1 && Identifier != 2){
        Direction = DOWN;
      }
      else{
        if(Position == 2 || Identifier == 1 && Position < 15){
          Identifier = 1;
          Direction = UP;
        }
        else{
           if(Position > 9 && Position  == 15 || Identifier == 2){
            Identifier = 2;
            Direction = DOWN;
           }
        }
     }
   }
  }
  else{
    
    if(Value == FIND_LIGHT_OPPOSITE){
      if(Direction == UP){
        Direction = DOWN;
      }
      else{
        Direction = UP;
      }
      OneStep(Direction);
    }
    else{
      OneStep(Direction);
    }
  }
}

/*----------------------------------------------------------*/

int HeightCheck(void){
  int Height;
  
  Height = sonar.ping_cm();
  /*Serial.print(Height);
  Serial.println(" cm");*/
  return Height;
}

/*----------------------------------------------------------*/

void OneStep(int UpOrDown){
  /*smallest motor rotation is 1/100 of a revolution*/
  SmallIncrement = STEPS_PER_OUT_REV/50;

  /*checking direction and turning motor accordingly*/
  if(UpOrDown == UP){
    steppermotor.setSpeed(700);
    steppermotor.step(-SmallIncrement);
    delay(STEP_DELAY);
  }
  if(UpOrDown == DOWN){
      steppermotor.setSpeed(700);
      steppermotor.step(SmallIncrement);
      delay(STEP_DELAY);
  }
}

/*----------------------------------------------------------*/

double IntensitySample(void){
  double Sum = 0;

  for(index = 1; index <= NUMBER_OF_SAMPLES; index++){
    Value = analogRead(LDR_PIN);
    Sum += Value;
  }

  Avg = Sum / index;
  return Avg;
}
