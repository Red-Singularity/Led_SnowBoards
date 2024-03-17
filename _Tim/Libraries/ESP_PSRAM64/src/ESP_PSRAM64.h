/*  ------------------  READ ME  -------------------- 
 *  This code is for interfacing with the ESP-PSRAM64: https://cdn-shop.adafruit.com/product-files/4677/4677_esp-psram64_esp-psram64h_datasheet_en.pdf
 *  It's a very light package and does not support deallocation of memory, although it does allow for overwriting previous allocations
 *  Requires the SPI.h library, which should be installed with every Arduino package
 *  This library does not use the quad-write functionality of the RAM IC and as such has only a fourth of the total possible memory size
*/

#include <Arduino.h>
#include <SPI.h>

#ifndef ESP_PSRAM64_H
#define ESP_PSRAM64_H

//-----------------  Common Structs  -------------------
struct dataRegion{
  //Values
  uint32_t      concat_address = 0; //address of the first byte of data, ordered {0x00,most significant byte,..., least significant byte}
  size_t data_size = 0;             //size of the data written in bytes
  bool          assigned = false;   //tracks if this instance has been assigned data yet
  
  //Common operators
  bool operator==(const dataRegion& cmp){
    if(concat_address != cmp.concat_address){
      return false;
    }
    if(data_size != cmp.data_size){
      return false;
    }
    if(assigned != cmp.assigned){
      return false;
    }
    return true;
  }
  bool operator!=(const dataRegion& cmp){
    return !(*this == cmp);
  }
  void operator=(const dataRegion& cmp){
    concat_address = cmp.concat_address;
    data_size = cmp.data_size;
    assigned = cmp.assigned;
  }
};

template <size_t MAX_DATA_SIZE>
struct pulledData {
  //Values
  uint8_t data_array[MAX_DATA_SIZE] = {0};  //array for the largest amount of data that will be read at once in bytes
  size_t  data_size = 0;                    //size of the data actually read in bytes

  //Common operators
  bool operator==(const pulledData& cmp){
    if(data_size != cmp.data_size){
      return false;
    }
    for(size_t i = 0; i < MAX_DATA_SIZE; i++){
      if(data_array[i] != cmp.data_array[i]){
        return false;
      }
    }
    return true;
  }
  bool operator!=(const pulledData& cmp){
    return !(*this == cmp);
  }
  void operator=(const pulledData& cmp){
    for(size_t i = 0; i < MAX_DATA_SIZE; i++){
      data_array[i] = cmp.data_array[i];
    }
    data_size = cmp.data_size;
  }
};


//----------------  Class Definition  ------------------
template<size_t MAX_DATA_SIZE = 1000, uint16_t MAX_DATA_REGIONS = 1000>  //Size of the max size of a written/read piece of information in bytes (default 1 KB)
class ESP_PSRAM64{
  private:
    //Common terms
    #define   RAM_MFID_PASS           0x0D        //manufacturer ID (see data sheet page 12)
    #define   RAM_KGD_PASS            0x5D        //known good die ID (see data sheet page 12)
    #define   RAM_WRITE_COMMAND       0x02        //up to 144 MHz (likely lower due to PCB and ESP32 limitations)
    #define   RAM_FAST_READ_COMMAND   0x03        //up to 144 MHz (likely lower due to PCB and ESP32 limitations)
    #define   RAM_READ_ID_COMMAND     0x9F        //up to 144 MHz (likely lower due to PCB and ESP32 limitations)
    #define   RAM_ADDRESS_SIZE        3           //the RAM memory addresses are 3 bytes long THIS CAN NOT BE CHANGED WITHOUT CAUSING ISSUES WITH ADDRESS CONCATENATION
    #define   RAM_MEMORY_SIZE         256*256*256 //maximum number of bytes that can be stored with 3 byte addresses
    #define   RAM_PAGE_SIZE           1000        //maximum number of bytes that can be written or read in a single operation (technically (CA[9:0]) or 1024 bytes, see data sheet page 6)

    //Error terms
    #define   RAM_PASS             0  //returned if everything went well
    #define   RAM_ERROR           -1  //returned if function failed, but RAM initialized properly
    #define   RAM_CRITICAL_ERROR  -2  //returned if RAM fails to initialize

    //Common settings
    const unsigned long SCLK_SPEED;     //SPI communication clock speed in Hz                               (default 10 MHz)
    const int           CS;             //RAM chip select GPIO number                                       (default 2)
    const SPISettings   RAM_SETTINGS;   //settings variable for SPI communication

    //Common variables
    uint16_t  next_data_region_index;                 //tracks the next available index in the data_regions array
    bool      ram_error;                              //tracks if there is an issue communicating with the RAM

    void val_to_address(uint8_t *p_address, uint32_t concat_address){
      /* Arguments:
       *  - p_address is a pointer to the address array, that will be modified to return the address info
       *  - concat_address is in uint32_t form: {0x00,most significant byte,..., least significant byte}
       * 
       * Converts uint32_t concat_address to a 3-byte array stored using the p_address pointer */

      *p_address        = (concat_address >> 16) & 0xFF;
      *(p_address + 1)  = (concat_address >> 8) & 0xFF;
      *(p_address + 2)  = concat_address & 0xFF;
    }
    
    void data_transfer(uint8_t *p_data_array, size_t data_size){
      /* Arguments:
       *  - data_array is a pointer to the data to be transfered
       *  - data_size is the size of the data to be transfered in bytes
       * 
       * Uses SPI.transfer to communicate the information in data_array to the RAM IC. SPI.transfer will
       * update the data_array (buffer) as data is transmitted, so no need for a return type */

      digitalWrite(CS,LOW);
      SPI.beginTransaction(RAM_SETTINGS);
      SPI.transfer(p_data_array,data_size);
      SPI.endTransaction();
      digitalWrite(CS,HIGH);
    }

    void write_page(const uint8_t* data_array, size_t data_size, uint32_t concat_address){
      /* Arguments:
       *  - data_array is a pointer to the data to be transfered
       *  - data_size is the size of the data to be transfered in bytes (no more than RAM_PAGE_SIZE)
       *  - concat_address holds the 3-byte RAM memory address to write to in its 3 least significant bytes
       * 
       * Adds the write command and address to the write buffer and transfers that data to RAM */

      uint8_t address[RAM_ADDRESS_SIZE];
      val_to_address(address,concat_address);
      
      uint8_t write_buff[4 + RAM_PAGE_SIZE];
      unsigned long buff_index = 0;
      
      write_buff[buff_index] = RAM_WRITE_COMMAND;
      buff_index++;
      
      for(int i = 0; i < RAM_ADDRESS_SIZE; i++){
        write_buff[buff_index] = address[i];
        buff_index++;
      }
      
      for(size_t i = 0; i < data_size; i++){
        write_buff[buff_index] = *(data_array + i);
        buff_index++;
      }
    
      //send SPI write buffer
      data_transfer(write_buff,4 + data_size);
    }

    void read_page(uint8_t* p_data_array, size_t data_size, uint32_t concat_address){
      /* Arguments:
       *  - p_data_array is a pointer to where data will be read to (on arduino side)
       *  - data_size is the size of the data to be transfered in bytes (no more than RAM_PAGE_SIZE)
       *  - concat_address holds the 3-byte RAM memory address to write to in its 3 least significant bytes
       * 
       * Adds the read command and address to the read buffer and fetches that data from RAM. Stores that
       * data in the location pointed to by p_data_array */
       
      uint8_t address[RAM_ADDRESS_SIZE];
      val_to_address(address,concat_address);
      
      uint8_t read_buff[4 + RAM_PAGE_SIZE];
      unsigned long buff_index = 0;
      
      read_buff[buff_index] = RAM_FAST_READ_COMMAND;
      buff_index++;
      
      for(int i = 0; i < RAM_ADDRESS_SIZE; i++){
        read_buff[buff_index] = address[i];
        buff_index++;
      }
    
      //send SPI read buffer
      data_transfer(read_buff,4 + data_size);

      //update values in data_array
      buff_index = 4;
      for(size_t i = 0; i < data_size; i++){
        *(p_data_array + i) = read_buff[buff_index];
        buff_index++;
      }
    }
    
  public:
    //Constructor (initializes common settings)
    ESP_PSRAM64(unsigned long sclk_speed = 10000000, int cs = 2) : SCLK_SPEED(sclk_speed), CS(cs), RAM_SETTINGS(SPISettings(SCLK_SPEED, MSBFIRST, SPI_MODE0)){
      next_data_region_index = 1;
      ram_error = true;
    }

    //User variables
    dataRegion data_regions[MAX_DATA_REGIONS + 1];  //stores each written piece of information as an element in this statically-allocated array (+1 because the 0 index is unused)
    
    //User functions
    int begin(){
      /* Reads the MF ID and KGD values of the RAM IC through the read ID functionality and compares against
       * known good values for both. Returns RAM_PASS if both values are correct and RAM_CRITICAL_ERROR
       * otherwise. Sets the ram_error variable to false (non-error state) if read ID passes and to true
       * otherwise. The ram_error varaible is defined as true and this function is the only one to interact
       * with ram_error, so this function must successfully run in order for any other function to be called */

      #ifdef RAM_DEBUG
        Serial.println("Initializing RAM...");
      #endif

      //set pin states and initialize SPI
      pinMode(CS,OUTPUT);
      digitalWrite(CS,HIGH);
      SPI.begin();
    
      //read ID from RAM
      uint8_t read_IDbuff[] = {RAM_READ_ID_COMMAND, 0x00, 0x00, 0x00, 0x00, 0x00};

      data_transfer(read_IDbuff,sizeof(read_IDbuff));
    
      uint8_t mfid = read_IDbuff[4];
      uint8_t kgd = read_IDbuff[5];
      
      #ifdef RAM_DEBUG
        Serial.print("  MF ID: "); Serial.println(mfid);
        Serial.print("  KGD: "); Serial.println(kgd);
      #endif
    
      //check ID values against known good values
      if((mfid != RAM_MFID_PASS) or (kgd != RAM_KGD_PASS)){
        #ifdef RAM_DEBUG
          Serial.println("  ERROR: RAM ID read failed");
          Serial.println("  Initialization failed");
        #endif
        ram_error = true;
        return RAM_CRITICAL_ERROR;
      }
      else{    
        #ifdef RAM_DEBUG
          Serial.println("  RAM ID read passed");
          Serial.println("  Initialization done");
        #endif
        ram_error = false;
        return RAM_PASS;
      }
    }
    
    int write(uint16_t *p_data_index, void *p_inputdata, size_t data_size, uint16_t *p_override_data_index = NULL){
      /* Arguments:
       *  - p_data_index is a pointer to an external uint16_t that will hold the index of data_regions in
       *      which the data location in RAM is stored
       *  - p_inputdata is a pointer to the data that will be written to RAM. Accepts any non-String type
       *  - data_size is the size of the input data in bytes (use sizeof function)
       *  - p_override_data_index is a pointer to an index in data_regions that is already occupied. The
       *      user should never be using this argument; it is just used by the overwrite() function
       * 
       * Returns RAM_PASS if the write was successful and RAM_ERROR or RAM_CRITICAL_ERROR otherwise. Make
       * sure the data input through p_input data is not fragmented, as this function relies on the data to
       * be written being stored in contiguous memory. */

      #ifdef RAM_DEBUG
        Serial.print("Writing "); Serial.print(data_size); Serial.println(" bytes...");
        unsigned long debug_timer = micros();
      #endif
      
      //check possible errors
      if(ram_error){
        #ifdef RAM_DEBUG
          Serial.println("  ERROR: RAM error thrown. Check initialization");
        #endif
        return RAM_CRITICAL_ERROR;
      }

      if(data_size == 0){
        #ifdef RAM_DEBUG
          Serial.println("  ERROR: Write data size can't be 0");
        #endif
        return RAM_ERROR;
      }

      if(data_size > MAX_DATA_SIZE){
        #ifdef RAM_DEBUG
          Serial.println("  ERROR: Data to be written too large");
        #endif
        return RAM_ERROR;
      }

      //get data_index and test for error
      uint16_t data_index;
      if(p_override_data_index == NULL){
        data_index = next_data_region_index;
      }
      else{
        data_index = *p_override_data_index;
      }
      
      if(data_index > MAX_DATA_REGIONS){
        #ifdef RAM_DEBUG
          Serial.println("  ERROR: Out of space to store written data location in RAM. Increase MAX_DATA_REGIONS");
        #endif
        return RAM_ERROR;
      }

      #ifdef RAM_DEBUG
        Serial.print("  Index "); Serial.print(data_index); Serial.println(" in 'data_regions' array");
      #endif
      
      //get address and test for error
      uint32_t concat_address = data_regions[data_index - 1].concat_address + data_regions[data_index - 1].data_size;
      
      if(concat_address + data_size > RAM_MEMORY_SIZE){
        #ifdef RAM_DEBUG
          Serial.println("  ERROR: Not enough space in RAM for write");
        #endif
        return RAM_ERROR;
      }

      #ifdef RAM_DEBUG
        uint8_t address[RAM_ADDRESS_SIZE];
        val_to_address(address,concat_address);
        Serial.print("  Address "); Serial.print(address[0]);
        Serial.print(", "); Serial.print(address[1]);
        Serial.print(", "); Serial.println(address[2]);
      #endif
    
      //update the data_regions array
      data_regions[data_index].concat_address = concat_address;
      data_regions[data_index].data_size = data_size;
      data_regions[data_index].assigned = true;

      //write one page at a time till all data is written
      const uint8_t *p_byte = (const uint8_t*)p_inputdata;
      while(data_size > RAM_PAGE_SIZE){
        write_page(p_byte,RAM_PAGE_SIZE,concat_address);
        concat_address += RAM_PAGE_SIZE;
        p_byte += RAM_PAGE_SIZE;
        data_size -= RAM_PAGE_SIZE;
      }

      //write last partial page of data
      write_page(p_byte,data_size,concat_address);

      //modify p_data_index
      if(p_data_index != NULL){
        *p_data_index = data_index;
      }

      //update next region
      next_data_region_index++;
      
      #ifdef RAM_DEBUG
        Serial.print("  Write done in "); Serial.print(micros() - debug_timer); Serial.println(" µs");
      #endif
      
      return RAM_PASS;
    }

    int overwrite(uint16_t data_index, void *p_inputdata, size_t data_size){
      /* Arguments:
       *  - data_index is the index of data_regions in which the location in RAM to be overwritten is stored
       *  - p_inputdata is a pointer to the data that will be written to RAM. Accepts any non-String type
       *  - data_size is the size of the input data in bytes (use sizeof function)
       * 
       * Returns RAM_PASS if the write was successful and RAM_ERROR or RAM_CRITICAL_ERROR otherwise. Make
       * sure the data input through p_input data is not fragmented, as this function relies on the data to
       * be written being stored in contiguous memory. */

      //check for errors
      if(data_regions[data_index].assigned == false){
        #ifdef RAM_DEBUG
          Serial.println("  ERROR: Overwrite location not previously assigned");
        #endif
        return RAM_ERROR;
      }

      if(data_regions[data_index].data_size != data_size){
        #ifdef RAM_DEBUG
          Serial.println("  ERROR: Overwrite data size not equal to previously assigned size");
        #endif
        return RAM_ERROR;
      }

      return write(NULL,p_inputdata,data_size,&data_index);
    }
    
    int read(pulledData<MAX_DATA_SIZE>* p_pulled_data, uint16_t data_index){
      /* Arguments:
       *  - p_pulled_data is a pointer to an external pulledData type, which will be modified
       *  - data_index is the index of data_regions in which the location in RAM to be read is stored
       *
       * Returns RAM_PASS  if the read was successful and RAM_ERROR or RAM_CRITICAL_ERROR otherwise */

      #ifdef RAM_DEBUG
        unsigned long debug_timer = micros();
      #endif
      
      //check primary errors
      if(ram_error){
        #ifdef RAM_DEBUG
          Serial.println("Reading...");
          Serial.println("  ERROR: RAM error thrown. Check initialization");
        #endif
        return RAM_CRITICAL_ERROR;
      }
      
      if(data_index > MAX_DATA_REGIONS){
        #ifdef RAM_DEBUG
          Serial.println("Reading...");
          Serial.println("  ERROR: Read index not in range");
        #endif
        return RAM_ERROR;
      }

      
      //extract from data_regions
      dataRegion read_region = data_regions[data_index];
      uint32_t concat_address = read_region.concat_address;
      size_t data_size = read_region.data_size;
      bool assigned = read_region.assigned;
      
      #ifdef RAM_DEBUG
        Serial.print("Reading "); Serial.print(data_size); Serial.println(" bytes...");
      #endif
      
      //check secondary errors
      if(data_size == 0){
        #ifdef RAM_DEBUG
          Serial.println("  ERROR: Data to read can't have a size of zero");
        #endif
        return RAM_ERROR;
      }

      if(data_size > MAX_DATA_SIZE){
        #ifdef RAM_DEBUG
          Serial.println("  ERROR: Data to be read too large");
        #endif
        return RAM_ERROR;
      }

      if(concat_address + data_size > RAM_MEMORY_SIZE){
        #ifdef RAM_DEBUG
          Serial.println("  ERROR: Not enough space in RAM for write");
        #endif
        return RAM_ERROR;
      }

      if(!assigned){
        #ifdef RAM_DEBUG
          Serial.println("  ERROR: Region to be read never assigned");
        #endif
        return RAM_ERROR;
      }

      #ifdef RAM_DEBUG
        uint8_t address[RAM_ADDRESS_SIZE];
        val_to_address(address,concat_address);
        Serial.print("  Address "); Serial.print(address[0]);
        Serial.print(", "); Serial.print(address[1]);
        Serial.print(", "); Serial.println(address[2]);
      #endif
    
      //update pulled data size
      p_pulled_data->data_size = data_size;
      
      //read one page at a time till all data is read
      uint8_t *p_data_array = p_pulled_data->data_array;
      while(data_size > RAM_PAGE_SIZE){
        read_page(p_data_array,RAM_PAGE_SIZE,concat_address);
        
        concat_address += RAM_PAGE_SIZE;
        p_data_array += RAM_PAGE_SIZE;
        data_size -= RAM_PAGE_SIZE;
      }

      //write last partial page of data
      read_page(p_data_array,data_size,concat_address);
      
      #ifdef RAM_DEBUG
        Serial.print("  Read done in "); Serial.print(micros() - debug_timer); Serial.println(" µs");
      #endif
      
      return RAM_PASS;
    }
};

#endif
