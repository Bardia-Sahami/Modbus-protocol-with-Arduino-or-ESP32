// MASTER
#include <string.h>

const int N = 42;
int error0 = 0;
int error1 = 0;
bool s0oof = false; // Slave 0 Out of Service
bool s1oof = false; // Slave 0 Out of Service
char mystring2[N];
String mystring;
char* char_arr;


int address;
String function;
String mem_address;
String my_data;

void read_package();
void read_coil();
void write_coil(int state);
void read_register();
void write_register();
void error_handler(int slave, int excp);

void setup() {                
 Serial.begin(115200); // PC Display
 Serial2.begin(115200); // Transmission
}

void loop() {
int i = 0;
intrupt:
 if(Serial.available()) 
   {
      String userinput;
      userinput = Serial.readString();
      Serial.print("You wrote ");
      Serial.print(userinput);
      if (userinput == "writecoil0\n") {
        write_coil(0);
      } else if (userinput == "writecoil1\n") {
        write_coil(1);
      } else if (userinput == "writecoilfunc\n") {     // Just as an example for error handling
        write_coil(2);
      } else if (userinput == "writecoilop\n") {       // Just as an example for error handling
        write_coil(3);
      } else if (userinput == "writeregister\n") {
        write_register();
      }
   }
 if(Serial2.available())
   {
      Serial.println("Received Information:");
      function = "";
      mem_address = "";
      my_data = "";
      Serial2.readBytes(mystring2, 42);
      Serial.println(mystring2);
      read_package();
      if (function[0] == '1') {
        error_handler(address, my_data[15]-48);
      } else {
        Serial.println("Status: Command Performed Successfully");
      }
      Serial.print("Address: ");
      Serial.println(address);
      Serial.print("Function: ");
      Serial.println(function);
      Serial.print("Memory Address: ");
      Serial.println(mem_address);
      Serial.print("Data: ");
      Serial.println(my_data);
      Serial.println("Flushing Serial Buffers\n");
      Serial.flush();
      Serial2.flush();
      
   }
   for (i; i < 19500; i++) {
      delay(1);
      if(Serial.available() || Serial2.available()) {
        goto intrupt;
      }
   }
   Serial.println("---20secs passed---");
   read_coil();
   delay(500);
   read_register();
   i = 0;
}

void read_coil() {
  // This function works properly (hard coded)
  if (!s0oof) {
    // Read Coil -> Controller
    // Address: 0 | Function: 0000 0001 (01) | Address: 0000 0000 0000 0000 | Quantity: 0000 0000 0000 0001
    mystring = "00000000100000000000000000000000000000001"; // Read Coil 0
    char_arr = &mystring[0];
    Serial2.write(char_arr, N);
  } else {
    Serial.println("Status: !!!SLAVE 0 OUT OF SERVICE, RESET!!!");
  }
  
}
void write_coil(int state) {
  if (!s0oof) {
    // Write Single Coil -> Controller
    // Address: 0 | Function: 0000 0101 (05) | Address: 0000 0000 0000 0000 | Value: 1111 1111 0000 0000 (High) or 0000 0000 0000 0000 (Low)
    if (state == 0) {
      mystring = "00000010100000000000000000000000000000000"; // Write Coil 0 Low
    } else if (state == 1) {
      mystring = "00000010100000000000000001111111100000000"; // Write Coil 0 High
    } else if (state == 2) {
      mystring = "00000111100000000000000000000000000000000"; // Unknown Function
    } else if (state == 3) {
      mystring = "00000010100000001000000000000000000000000"; // Unknown Operation (Wrong Address)
    }
    char_arr = &mystring[0];
    Serial2.write(char_arr, N);
  } else {
    Serial.println("Status: !!!SLAVE 0 OUT OF SERVICE, RESET!!!");
  }
  
}
void read_register() {
  if (!s1oof) {
    // Read Input Register -> Temperature
    // Address: 1 | Function: 0000 0100 (04) | Address: 0000 0000 0000 0000 | Quantity: 0000 0000 0000 0001
    mystring = "10000010000000000000000000000000000000001"; //  Read 
    char_arr = &mystring[0];
    Serial2.write(char_arr, N);
  } else {
    Serial.println("Status: !!!SLAVE 1 OUT OF SERVICE, RESET!!!");
  }
  
}
void write_register() {
   if (!s0oof) {
     // Write Single Register -> Controller
     // Address: 0 | Function: 0000 0110 (06) | Address: 0000 0000 0000 0100 | Value: 0000 0000 1111 1111
     mystring = "00000011000000000000001000000000011111111"; // Write Register 8 High
     char_arr = &mystring[0];
     Serial2.write(char_arr, N);
   } else {
    Serial.println("Status: !!!SLAVE 0 OUT OF SERVICE, RESET!!!");
   }
   
}
void error_handler(int slave, int excp) {
  if (slave == 0) {
    if (excp == -1) {
      error0++;
      if (error0 >= 3) {
        s0oof = true;
        Serial.println("Status: !!!SLAVE 0 OUT OF SERVICE, RESET!!!");
      }
    } else if (excp == 0) {
      Serial.println("Status: !Slave 0: Wrong Function!");
    } else if (excp == 1) {
      Serial.println("Status: !Slave 0: Operation Failed!");
    }
    
  }
  if (slave == 1) {
    if (excp == -1) {
      error1++;
      if (error1 >= 3) {
        s1oof = true;
        Serial.println("Status: !!!SLAVE 1 OUT OF SERVICE, RESET!!!");
      }
    } else if (excp == 0) {
      Serial.println("Status: !Slave 1: Wrong Function!");
    } else if (excp == 1) {
      Serial.println("Status: !Slave 1: Operation Failed!");
    }
    
  }
}

void read_package() {
  address = mystring2[0]-48; //ASCI value to real value (int)

  // Read Function
  for(int i = 1; i <= 8; i++){
    function.concat(mystring2[i]);
  }

  // Read Memory Address
  for(int i = 9; i <= 24; i++){
    mem_address.concat(mystring2[i]);
  }

  // Read Data
  for(int i = 25; i <= 40; i++){
    my_data.concat(mystring2[i]);
  }
}
