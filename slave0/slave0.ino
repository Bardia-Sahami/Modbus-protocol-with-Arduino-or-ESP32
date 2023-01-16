// SLAVE 0
#include <string.h>

char* char_arr;

int address;
String function;
String mem_address;
String my_data;


const int N = 42;
char mystring[N];

int coil1;
int register1[16];

void read_package();
void run_read_coil();
void run_write_coil();
void run_write_register();
void run_excp(int excp);


void setup() {
 Serial.begin(115200); // For Monitor
 Serial2.begin(115200); // For transmission between boards
 pinMode(33, OUTPUT);
}
void loop() {  
if(Serial2.available())
   {
      delay(100);
      function = "";
      mem_address = "";
      my_data = "";
      Serial2.readBytes(mystring, 42);
      Serial.println(mystring);
      Serial.println(" RECEIVER");
      read_package();
      Serial.print("Address: ");
      Serial.println(address);
      Serial.print("Function: ");
      Serial.println(function);
      Serial.print("Memory Address: ");
      Serial.println(mem_address);
      Serial.print("Data: ");
      Serial.println(my_data);
      if (address == 0) {
        if (function == "00000001") {
          run_read_coil();
        } else if (function == "00000101") {
          run_write_coil();
        } else if (function == "00000110") {
          run_write_register();
        } else {
          run_excp(0);
        }
      }
   }
}

void read_package() {
  address = mystring[0]-48;

  // Read Function
  for(int i = 1; i <= 8; i++){
    function.concat(mystring[i]);
  }

  // Read Memory Address
  for(int i = 9; i <= 24; i++){
    mem_address.concat(mystring[i]);
  }

  // Read Data
  for(int i = 25; i <= 40; i++){
    my_data.concat(mystring[i]);
  }
}

void run_read_coil() {
  if (mem_address == "0000000000000000") {
    if (digitalRead(33) == HIGH) {
      String mystring2 = "00000000100000000000000001111111111111111";
      char_arr = &mystring2[0];
      Serial2.write(char_arr, 42);
    } else {
      String mystring2 = "00000000100000000000000000000000000000000";
      char_arr = &mystring2[0];
      Serial2.write(char_arr, 42);
    }
  } else {
    run_excp(1);
  }
}

void run_write_coil() {
  if (mem_address == "0000000000000000") {
    if (my_data == "1111111100000000") {
      digitalWrite(33, HIGH);
      String mystring2 = "00000010100000000000000001111111100000000";
      char_arr = &mystring2[0];
      Serial2.write(char_arr, 42);
      Serial.print('\n');
    } else {
      digitalWrite(33, LOW);
      String mystring2 = "00000010100000000000000000000000000000000";
      char_arr = &mystring2[0];
      Serial2.write(char_arr, 42);
      Serial.print('\n');
    }
  } else {
    run_excp(1);
  }
}

void run_write_register() {
  if (mem_address == "0000000000000100") {
    for (int i = 0; i < 16; i++) {
      register1[i] = my_data[i];
    }
    String mystring2 = "0000001100000000000000100"; // Write Register 8 High
    mystring2.concat(my_data);
    char_arr = &mystring2[0];
    Serial2.write(char_arr, 42);
  } else {
    run_excp(1);
  }
}

void run_excp(int excp) {
  String mystring2 = "0";
  mystring2.concat(function);
  mystring2.concat(mem_address);
  mystring2[1] = '1'; // Change function to exception code
  if (excp == 0) {
    my_data = "0000000000000000";
  } else if (excp == 1) {
    my_data = "0000000000000001";
  }
  mystring2.concat(my_data);
  char_arr = &mystring2[0];
  Serial2.write(char_arr, 42);
}
