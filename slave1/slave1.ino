// SLAVE 1
#include <string.h>

char* char_arr;

int address;
String function;
String mem_address;
String my_data;

const int N = 42;
char mystring[N];

void read_package();
void run_read_register();
void run_excp(int excp);


void setup() {
 Serial.begin(115200); // For Monitor
 Serial2.begin(115200); // For Board communications
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
      if (address == 1) {
        if (function == "00000100") {
          run_read_register();
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
//    function[i-1] = mystring[i];
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



void run_read_register() {
  if (mem_address == "0000000000000000") {
      String mystring2 = "10000010000000000000000001111111111111111";
      char_arr = &mystring2[0];
      Serial2.write(char_arr, 42);
  } else {
    run_excp(1);
  }
}


void run_excp(int excp) {
  String mystring2 = "1"; // Slave address (1)
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
