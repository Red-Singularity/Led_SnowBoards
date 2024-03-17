/* ------------------  READ ME  -------------------- 
 * - This code is for interfacing with the ESP-PSRAM64: https://cdn-shop.adafruit.com/product-files/4677/4677_esp-psram64_esp-psram64h_datasheet_en.pdf
 * - It's a very light package and does not support deallocation of memory, although it does allow for overwriting previous allocations
 * - Requires the SPI.h library, which should be installed with every Arduino package
 * - This library does not support the quad-write functionality of the RAM IC and as such has only a fourth of the total possible memory size
*/
#define RAM_DEBUG                 /* - Comment this line out to stop recieving debugging messages via Serial.print
                                   * - Must be before #include */

#define RAM_DATA_SIZE 1500        /* - Maximum amount of data (in bytes) to be read/written in one operation
                                   * - Limited by size_t type, so no more than 2^15-1 or 32767
                                   * - Default value is 1000 bytes */

#define RAM_MAX_REGIONS 1000      /* - Maximum number of pieces of information that can be stored (ie maximum number of 'write()' calls allowed)
                                   * - Limited by uint16_t type, so no more than 2^16-1 or 65535
                                   * - Default value is 1000 bytes */

#define SPI_CLK_SPEED 10000000    /* - SPI clock speed for communication with RAM in Hz
                                   * - Generally limited by microcontroller or peripheral PCB design
                                   * - See data sheet page 6-7 for PSRAM IC max frequencies 
                                   * - Default value is 10MHz */

#define CS_RAM 2                  /* - Chip select pin GPIO value for the RAM IC
                                   * - Default value is 2 */

#include <ESP_PSRAM64.h>

ESP_PSRAM64<RAM_DATA_SIZE, RAM_MAX_REGIONS> ram(SPI_CLK_SPEED, CS_RAM);  //class definition with all parameters specified
//ESP_PSRAM64<>ram();                                                    //class definition using default values

typedef pulledData<RAM_DATA_SIZE> fetchedData;  /* - 'pulledData' is a struct that stores data after it's read off RAM
                                                 * - Can define a new type definition to avoid including the template parameter each time */

int Wrap(int val){
  /* Example function used to store sequential data in RAM
   * wraps input value to uint8_t size (max 255) */
 
  int wrappedval = (val % 255 + 255) % 255;
  return wrappedval;
}

uint8_t oversized_data[RAM_DATA_SIZE + 1] = {0};  //for errors example (outside of setup to save stack space)

void setup() {
  Serial.begin(115200);
  while(!Serial){}
  delay(1000);

  Serial.println("------------------  Initialization  ------------------");
  ram.begin();      //initialize RAM functionality and test connection
  
  Serial.println("----------------------  Write  -----------------------");
  uint16_t index;   /* - This will be modified by 'write()' to hold the index value passed later to 'read()' and 'overwrite()'
                     * - Refers to an index in the 'data_regions' array, which stores the location in RAM of each datum sent through 'write()' */
  
  //create data array to be stored (can be any size <= RAM_DATA_SIZE)
  uint8_t my_data[RAM_DATA_SIZE];
  for(size_t i = 0; i < RAM_DATA_SIZE; i++){
    my_data[i] = Wrap(i);
  }
  
  ram.write(&index, my_data, sizeof(my_data));    /* - Write data to RAM
                                                   * - Writes data given one byte at a time, ie will write a uint32_t type as {MSB,...,...,LSB}
                                                   * - Because of the above, make sure data is contiguous in memory
                                                   * - Stores location of written data in the next available index of the 'data_regions' array 
                                                   * - Modifies 'index' to be the index referred to above */
  
  Serial.println("----------------------  Read  ------------------------");
  fetchedData read_data;        /* - Using type definition created above 'fetchedData', could also use 'pulledData<RAM_DATA_SIZE>'
                                 * - Depending on the value of RAM_DATA_SIZE this will be a big struct and will fill the stack quickly
                                 *    - Notice that here, only one instance of 'fetchedData' is used and continuously re-written to save space
                                 *    - "***ERROR*** A stack overflow in task loopTask has been detected" will be printed on the serial line
                                 *    - 'fetchedData' instances can also be declared globally so they won't take up space on the stack
                                 * - This will be modified by 'read()' to hold the data stored in RAM
                                 *    - 'read_data.data_array' the data read as a byte-array
                                 *    - 'read_data.data_size' the size of the read data in bytes (can't be more than RAM_DATA_SIZE) */
  
  ram.read(&read_data, index);  /* - Read data from RAM
                                 * - Reads data given one byte at a time, ie will read a uint32_t type as {MSB,...,...,LSB} 
                                 * - Modifies 'read_data' to hold the data read from RAM */
  
  //print the read data
  for(size_t i = 0; i < read_data.data_size - 1; i++){
    Serial.print(read_data.data_array[i]); Serial.print(", ");
  }
  Serial.println(read_data.data_array[read_data.data_size - 1]);
  
  Serial.println("--------------------  Overwrite  ---------------------");
  //create a new data array for overwrite
  uint32_t new_data[RAM_DATA_SIZE/4];
  for(size_t i = 0; i < RAM_DATA_SIZE/4; i++){
    new_data[i] = Wrap(i)*pow(2,24);
  }
  
  ram.overwrite(index, new_data, sizeof(new_data));   /* - Write data to RAM in the same memory location as the data marked by 'index'
                                                       * - Data to be written must be exactly the same size (in bytes) as the previously written data
                                                       * - Functions the same as 'write()' except can use previously-assigned memory */
  
  ram.read(&read_data, index);  //read new data from RAM
  
  //print the read data
  for(size_t i = 0; i < read_data.data_size - 1; i++){
    Serial.print(read_data.data_array[i]); Serial.print(", ");
  }
  Serial.println(read_data.data_array[read_data.data_size - 1]);
  
  Serial.println("---------------------  Strings  ----------------------");
  String string_data = "Hello World"; //String data to be written
  
  //cast string to char array
  unsigned int len = string_data.length() + 1;
  char char_array[len];
  string_data.toCharArray(char_array,len);
  
  ram.write(&index, char_array, len);   //write char array to RAM
  
  ram.read(&read_data, index);
  
  //print the read data
  for(size_t i = 0; i < read_data.data_size - 1; i++){
    Serial.print(char(read_data.data_array[i]));
  }
  Serial.println(char(read_data.data_array[read_data.data_size - 1]));
  
  Serial.println("----------------------  Errors  ----------------------");
  int err = ram.write(&index, oversized_data, sizeof(oversized_data));  /* - User functions return integer type
                                                                         *    - 0 (RAM_PASS) means everything went well
                                                                         *    - -1 (RAM_ERROR) means the function didn't run properly but RAM initialized properly
                                                                         *    - -2 (RAM_CRITICAL_ERROR) means RAM didn't initialize properly */
  
  if(err == 0){
    Serial.println("  Write was successful");
  }
  else if(err == -1){
    Serial.println("  Write failed, but RAM initialized properly");
  }
  else if(err == -2){
    Serial.println("  Write aborted because RAM never initialized properly");
  }
}

void loop() {

}
